#pragma once

#include "Pin.h"
#include "Timer.h"

/// A Switch is an ObservablePin::Observer that, together with a timer,
/// manages a finite state machine to debounce the pin and generate
/// toggled events.
/// A switch is constructed in the idle state
/// (on or off, depending on how the pin level compares with the onLevel).
/// Pin activity in the idle state generates a toggled event (off or on)
/// and causes the Switch to enter the bounce state.
/// After debounceDuration, another toggled event will be generated if
/// the pin level warrants it and the Switch will enter the idle state.
class Switch : private ObservablePin::Observer {
private:
    enum State {
	idle,
	bounce,
    };
    int const			onLevel;
    unsigned const		debounceDuration;
    std::function<void(bool)> const	toggled;
    bool			on;
    std::recursive_mutex	mutex;
    Timer			timer;
    int64_t			stateTime;
    State			state;
    void update(bool timeout);
public:
    /// Construct a Debounce ObservablePin::Observer.
    /// Durations are expressed in microseconds.
    /// These are thresholds that are examined on tick granularity.
    Switch(
	ObservablePin &			observablePin,
	int				onLevel,
	unsigned			debounceDuration,
	std::function<void(bool)>	toggled);

    /// Define an explicit move constructor
    /// because one will not be defined implicitly
    /// due to our non-copiable/non-movable std::recursive_mutex.
    /// A move constructor is needed to support initialization of a
    /// Switch class array class element.
    Switch(Switch const && move);

    bool isOn() const;
};
