#include "Switch.h"

Switch::Switch(
    ObservablePin &			observablePin,
    TickType_t				period,
    unsigned				resampleCount,
    std::function<void(bool)>		stabilized_)
:
    ObservablePin::Observer{observablePin, [this](){update(false);}},
    sampleCount		{++resampleCount},
    stabilized		{stabilized_},
    value		{},
    resample		{},
    mutex		{},
    timer		{"Switch", period, true, [this](){update(true);}}
{
    update(false);
}

Switch::Switch(Switch const && move)
:
    ObservablePin::Observer{move},
    sampleCount		{move.sampleCount},
    stabilized		{std::move(move.stabilized)},
    value		{move.value},
    resample		{move.resample},
    mutex		{},
    timer		{std::move(move.timer)}
{}

void Switch::update(bool timeout) {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    bool valueNow = observablePin.get_level();
    if (timeout) {
	if (value == valueNow) {
	    if (!--resample) {
		timer.stop();
		stabilized(value);
	    }
	} else {
	    resample = sampleCount;
	}
    } else {
	resample = sampleCount;
	timer.start();
    }
    value = valueNow;
}
