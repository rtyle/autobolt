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

    static constexpr unsigned bounceDuration	{25000};

    static constexpr ledc_timer_bit_t dutyResolutionLog2{LEDC_TIMER_8_BIT};
    static constexpr unsigned dutyResolution {1 << dutyResolutionLog2};

    asio::io_context::work	busy;

    ObservablePin::ISR		pinISR;		// install GPIO ISR service
    ObservablePin::Task		pinTask;	// buffer/relay to ObservablePin
    ObservablePin		boltPin;
    ObservablePin		doorPin;
    bool			boltClosed;
    bool			doorClosed;
    int64_t			doorClosedTime;
    Switch			boltClosedSwitch;
    Switch			doorClosedSwitch;
    Timer			doorClosedTimer;
    LEDC::Timer			pwmTimer;
    LEDC::Channel		enablePwm[bridgeCount];
    Pin				inPin[bridgeCount][2];
    ADC::Unit1			adcUnit;
    ADC::Unit1::Channel		adcChannel[bridgeCount];

    void setDuty(LEDC::Channel & pwm, float fraction) const {
	pwm.set_duty(1.0f > fraction
	    ? fraction * dutyResolution
	    : dutyResolution - 1);
	pwm.update_duty();
    }

    void boltClose(
	bool		start,
	int64_t		afterStart,
	uint32_t const	voltage[bridgeCount])
    {
	ESP_LOGI(name, "boltClose %s %lld %u %u",
	    start ? "start" : "stop", afterStart, voltage[0], voltage[1]);
	if (start) {
	    for (auto & ps: inPin) {
		bool l = 1;
		for (auto & p: ps) {
		    p.set_level(l);
		    l ^= 1;
		}
	    }
	    for (auto & pwm: enablePwm) {
		setDuty(pwm, 1.0f);
	    }
	} else {
	    doorClosedTimer.stop();
	    for (auto & pwm: enablePwm) {
		setDuty(pwm, 0.0f);
	    }
	}
    }

public:
    Main()
    :
	AsioTask	{},
	busy		{io},
	pinISR		{},
	pinTask		{"pinTask", 5, 4096, tskNO_AFFINITY, 128},
	boltPin		{control[1], GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE,
			    GPIO_PULLDOWN_DISABLE, GPIO_INTR_ANYEDGE, pinTask},
	doorPin		{control[0], GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE,
			    GPIO_PULLDOWN_DISABLE, GPIO_INTR_ANYEDGE, pinTask},
	boltClosed	{},
	doorClosed	{},
	doorClosedTime	{0},
	boltClosedSwitch{boltPin, 0, bounceDuration,
	    [this](bool on){io.post([this, on](){
		boltClosed = on;
		ESP_LOGI(name, "bolt %s", boltClosed ? "closed" : "open");
	    });}},
	doorClosedSwitch{doorPin, 0, bounceDuration,
	    [this](bool on){io.post([this, on](){
		doorClosed = on;
		ESP_LOGI(name, "door %s", doorClosed ? "closed" : "open");
		if (doorClosed) {
		    doorClosedTime = esp_timer_get_time();
		    if (!boltClosed) {
			doorClosedTimer.start();
		    }
		}
	    });}},
	doorClosedTimer{"doorClosedTimer", 1, true,
	    [this](){
		constexpr int64_t start	{1000000};
		constexpr int64_t stop	{1000000};
		int64_t afterStart{esp_timer_get_time() - start - doorClosedTime};
		uint32_t const voltage[bridgeCount]{
		    adcChannel[0].getVoltage(),
		    adcChannel[1].getVoltage(),
		};
		if (boltClosed) {
		    boltClose(false, afterStart, voltage);
		} else {
		    if (0 <= afterStart) {
			boltClose(afterStart < stop, afterStart, voltage);
		    }
		}
	    }},
	pwmTimer{LEDC_HIGH_SPEED_MODE, dutyResolutionLog2},
	enablePwm{
	    {pwmTimer, enable[0] , 0},
	    {pwmTimer, enable[1] , 0},
	},
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
