#include "esp_log.h"

#include "ADC.h"
#include "AsioTask.h"
#include "Error.h"
#include "LEDC.h"
#include "Pin.h"
#include "Switch.h"
#include "Timer.h"

class Main: public AsioTask {
private:
    // motorbrainz ESP32 interface to external controls
    static constexpr size_t controlCount{5};
    static constexpr gpio_num_t control[controlCount] {
	GPIO_NUM_5,		// SW1
	GPIO_NUM_38,		// SW2
	GPIO_NUM_35,		// SW3
	GPIO_NUM_33,		// SW4
	GPIO_NUM_32,		// SW5
    };

    // motorbrainz ESP32 interface to ST L298 dual full bridge driver
    static constexpr size_t bridgeCount{2};	// dual
    static constexpr gpio_num_t enable[bridgeCount] {
	GPIO_NUM_21,		// ENA
	GPIO_NUM_19,		// ENB
    };
    static constexpr gpio_num_t in[bridgeCount][2] {
	{
	    GPIO_NUM_22,	// INA1
	    GPIO_NUM_9,		// INA2
	},
	{
	    GPIO_NUM_23,	// INB1
	    GPIO_NUM_18,	// INB2
	},
    };
    static constexpr adc1_channel_t sense[bridgeCount] {
	ADC1_CHANNEL_1,		// SENA (GPIO_NUM_37)
	ADC1_CHANNEL_6,		// SENB (GPIO_NUM_34)
    };

    static constexpr TickType_t switchPeriod{1};
    static constexpr unsigned	switchResampleCount{8};

    static constexpr TickType_t	timerStartPeriod{4000 / portTICK_PERIOD_MS};
    static constexpr TickType_t timerStopPeriod	{1000 / portTICK_PERIOD_MS};

    static constexpr ledc_timer_bit_t dutyResolutionLog2{LEDC_TIMER_8_BIT};
    static constexpr unsigned dutyResolution {1 << dutyResolutionLog2};

    asio::io_context::work	busy;

    LEDC::Timer			pwmTimer;
    LEDC::Channel		boltOpenLed;
    LEDC::Channel		doorOpenLed;
    LEDC::Channel		enablePwm[bridgeCount];

    ObservablePin::ISR		pinISR;		// install GPIO ISR service
    ObservablePin::Task		pinTask;	// buffer/relay to ObservablePin
    ObservablePin		boltPin;
    ObservablePin		doorPin;

    bool			boltOpen;
    bool			doorOpen;
    Switch			boltOpenSwitch;
    Switch			doorOpenSwitch;
    Timer			timer;
    TickType_t			timerCount;

    Pin				inPin[bridgeCount][2];
    ADC::Unit1			adcUnit;
    ADC::Unit1::Channel		adcChannel[bridgeCount];

    void setDuty(LEDC::Channel & pwm, float fraction) const {
	pwm.set_duty(1.0f > fraction
	    ? fraction * dutyResolution
	    : dutyResolution - 1);
	pwm.update_duty();
    }

    void setDutyEnablePwm(float fraction) {
	for (auto & pwm: enablePwm) {
	    setDuty(pwm, fraction);
	}
    }

public:
    Main()
    :
	AsioTask	{},
	busy		{io},

	pwmTimer{LEDC_HIGH_SPEED_MODE, dutyResolutionLog2},
	boltOpenLed{pwmTimer, control[4], 0},
	doorOpenLed{pwmTimer, control[3], 0},
	enablePwm{
	    {pwmTimer, enable[0] , 0},
	    {pwmTimer, enable[1] , 0},
	},

	pinISR		{},
	pinTask		{"pinTask", 5, 4096, tskNO_AFFINITY, 256},
	boltPin		{control[1], GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE,
			    GPIO_PULLDOWN_DISABLE, GPIO_INTR_ANYEDGE, pinTask},
	doorPin		{control[0], GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE,
			    GPIO_PULLDOWN_DISABLE, GPIO_INTR_ANYEDGE, pinTask},

	boltOpen	{false},
	doorOpen	{false},
	boltOpenSwitch{boltPin, switchPeriod, switchResampleCount,
	    [this](bool on){io.post([this, on](){
		if (timer.isActive()) timer.stop();
		setDutyEnablePwm(0.0f);
		if ((boltOpen = on)) {
		    ESP_LOGI(name, "bolt opened");
		    setDuty(boltOpenLed, 1.0f);
		} else {
		    ESP_LOGI(name, "bolt closed");
		    setDuty(boltOpenLed, 0.0f);
		}
	    });}},
	doorOpenSwitch{doorPin, switchPeriod, switchResampleCount,
	    [this](bool on){io.post([this, on](){
		if (timer.isActive()) timer.stop();
		setDutyEnablePwm(0.0f);
		if ((doorOpen = on)) {
		    ESP_LOGI(name, "door opened");
		    setDuty(doorOpenLed, 1.0f);
		} else {
		    ESP_LOGI(name, "door closed");
		    setDuty(doorOpenLed, 0.0f);
		    if (boltOpen) {
			timer.setPeriod(timerStartPeriod);	// and start
		    }
		}
	    });}},
	timer{"timer", 1, true,
	    [this](){
		if (!doorOpen && boltOpen) {	// else ignore timer (leak)
		    if (timerStartPeriod == timer.getPeriod()) {
			    setDutyEnablePwm(1.0f);
			    ESP_LOGI(name, "bolt start");
			    timerCount = timerStopPeriod;
			    timer.setPeriod(1);			// and start
		    } else {
			uint32_t const v[bridgeCount]{
			    adcChannel[0].getVoltage(),
			    adcChannel[1].getVoltage(),
			};
			if (--timerCount) {
			    ESP_LOGI(name, "bolt closing %u %u", v[0], v[1]);
			} else {
			    timer.stop();
			    setDutyEnablePwm(0.0f);
			    ESP_LOGE(name, "bolt stop %u %u", v[0], v[1]);
			}
		    }
		}
	    }},

	inPin{
	    {
		{in[0][0], GPIO_MODE_OUTPUT},
		{in[0][1], GPIO_MODE_OUTPUT},
	    },
	    {
		{in[1][0], GPIO_MODE_OUTPUT},
		{in[1][1], GPIO_MODE_OUTPUT},
	    },
	},
	adcUnit{ADC_WIDTH_BIT_12},
	adcChannel	{
	    {adcUnit, sense[0]},
	    {adcUnit, sense[1]},
	}
    {
	// set polarity once and enable later as needed
	for (auto & pair: inPin) {
	    bool level = 1;
	    for (auto & pin: pair) {
		pin.set_level(level);
		level ^= 1;
	    }
	}
	pinTask.start();
	run();
    }
};
constexpr adc1_channel_t Main::sense[];

extern "C"
void app_main()
{
    Main{};
}
