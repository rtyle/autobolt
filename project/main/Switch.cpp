#include "Switch.h"

Switch::Switch(
    ObservablePin &			observablePin,
    int					onLevel_,
    unsigned				debounceDuration_,
    std::function<void(bool)>		toggled_)
:
    ObservablePin::Observer{observablePin, [this](){update(false);}},
    onLevel		{onLevel_},
    debounceDuration	{debounceDuration_},
    toggled		{toggled_},
    on			{onLevel == observablePin.get_level()},
    mutex		{},
    timer		{"Switch", 1, true, [this](){update(true);}},
    stateTime		{esp_timer_get_time()},
    state		{idle}
{
    // generate an initial toggled event
    on ^= 1;
    update(false);
}

Switch::Switch(Switch const && move)
:
    ObservablePin::Observer{move},
    onLevel		{move.onLevel},
    debounceDuration	{move.debounceDuration},
    toggled		{std::move(move.toggled)},
    on			{move.on},
    mutex		{},
    timer		{std::move(move.timer)},
    stateTime		{move.stateTime},
    state		{move.state}
{}

// update our state machine based on what state we are in and
// why we are being called (timeout or event).
void Switch::update(bool timeout) {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    int64_t const now{esp_timer_get_time()};
    switch (state) {
    case idle:
	if (!timeout) {	// ignore timer (leak)
	    toggled(on ^= 1);
	    timer.start();
	    stateTime = now;
	    state = bounce;
	}
	break;
    case bounce:
	if (timeout && debounceDuration < now - stateTime) {
	    if (on != (onLevel == observablePin.get_level())) {
		toggled(on ^= 1);
	    }
	    timer.stop();
	    stateTime = now;
	    state = idle;
	}
	break;
    }
}

bool Switch::isOn() const {return on;}
