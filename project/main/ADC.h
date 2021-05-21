#pragma once

#include <cstdint>

#include "driver/adc.h"
#include "esp_adc_cal.h"

namespace ADC {

    struct Unit {
    private:
	adc_unit_t const id;
    protected:
	class Channel {
	protected:
	    struct Calibration {
		esp_adc_cal_characteristics_t	characteristics;
		esp_adc_cal_value_t const	type;
		Calibration(
		    adc_unit_t		id,
		    adc_bits_width_t	width,
		    adc_atten_t		attenuation,
		    uint32_t		vref);
	    } const calibration;
	    Channel(
		Unit &			unit,
		adc_bits_width_t	width,
		adc_atten_t		attenuation,
		uint32_t		vref);
	    virtual ~Channel();
	public:
	    virtual uint32_t getRaw() const = 0;
	    virtual uint32_t getVoltage() const = 0;
	};
	Unit(adc_unit_t id_);
	operator adc_unit_t() const;
    };

    struct Unit1: Unit {
	adc_bits_width_t const width;
	class Channel: Unit::Channel {
	private:
	    adc1_channel_t const id;
	public:
	    Channel(
		Unit1 &			unit,
		adc1_channel_t		id,
		adc_atten_t		attenuation	= ADC_ATTEN_DB_0,
		uint32_t		vref		= 1100);
	    operator adc1_channel_t() const;
	    uint32_t getRaw() const override;
	    uint32_t getVoltage() const override;
	};
	Unit1(adc_bits_width_t width);
    };

    struct Unit2: Unit {
	class Channel: Unit::Channel {
	private:
	    adc2_channel_t const id;
	    adc_bits_width_t const width;
	public:
	    Channel(
		Unit2 &			unit,
		adc2_channel_t		id,
		adc_bits_width_t	width,
		adc_atten_t		attenuation	= ADC_ATTEN_DB_0,
		uint32_t		vref		= 1100);
	    operator adc2_channel_t() const;
	    uint32_t getRaw() const override;
	    uint32_t getVoltage() const override;
	};
	Unit2();
    };

}
