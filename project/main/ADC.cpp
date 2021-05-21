#include "ADC.h"
#include "Error.h"

namespace ADC {

Unit::Channel::Calibration::Calibration(
    adc_unit_t		id,
    adc_bits_width_t	width,
    adc_atten_t		attenuation,
    uint32_t		vref)
:
    characteristics	{},
    type		{
	esp_adc_cal_characterize(id, attenuation, width, vref, &characteristics)
    }
{}

Unit::Channel::Channel(
    Unit &		unit,
    adc_bits_width_t	width,
    adc_atten_t		attenuation,
    uint32_t		vref)
:
    calibration		{unit, width, attenuation, vref}
{}

Unit::Channel::~Channel() {}

Unit::Unit(adc_unit_t id_): id{id_} {}

Unit::operator adc_unit_t() const {return id;}

Unit1::Channel::Channel(
    Unit1 &		unit,
    adc1_channel_t	id_,
    adc_atten_t		attenuation,
    uint32_t		vref)
:
    Unit::Channel	{unit, unit.width, attenuation, vref},
    id			{id_}
{
    adc1_config_channel_atten(*this, attenuation);
}

Unit1::Channel::operator adc1_channel_t() const {return id;}

uint32_t Unit1::Channel::getRaw() const {
    return adc1_get_raw(*this);
}

uint32_t Unit1::Channel::getVoltage() const {
    return esp_adc_cal_raw_to_voltage(getRaw(), &calibration.characteristics);
}

Unit1::Unit1(adc_bits_width_t width_): Unit{ADC_UNIT_1}, width{width_} {
    adc1_config_width(width);
}

Unit2::Channel::Channel(
    Unit2 &		unit,
    adc2_channel_t	id_,
    adc_bits_width_t	width_,
    adc_atten_t		attenuation,
    uint32_t		vref)
:
    Unit::Channel	{unit, width_, attenuation, vref},
    id			{id_},
    width		{width_}
{
    adc2_config_channel_atten(*this, attenuation);
}

Unit2::Channel::operator adc2_channel_t() const {return id;}

uint32_t Unit2::Channel::getRaw() const {
    int result;
    Error::throwIf(adc2_get_raw(*this, width, &result));
    return result;
}

uint32_t Unit2::Channel::getVoltage() const {
    return esp_adc_cal_raw_to_voltage(getRaw(), &calibration.characteristics);
}

Unit2::Unit2(): Unit{ADC_UNIT_2} {}

}
