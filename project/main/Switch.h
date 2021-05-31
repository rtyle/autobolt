#pragma once

#include "Pin.h"
#include "Timer.h"

/// A Switch is an ObservablePin::Observer that
/// reports stabilized (debounced) values.
/// After construction and each level change, the current value is noted
/// and then periodically sampled until its value stabilizes
/// (resampleCount + 1 consecutive periodic samples have the same value).
/// When a stable value is reached, stabilized is called with it.
class Switch : private ObservablePin::Observer {
private:
    unsigned const			sampleCount;
    std::function<void(bool)> const	stabilized;
    bool				value;
    unsigned				resample;
    std::recursive_mutex		mutex;
    Timer				timer;
    void update(bool timeout);
public:
    Switch(
	ObservablePin &			observablePin,
	TickType_t			period,
	unsigned			resampleCount,
	std::function<void(bool)>	stablized);

    /// Define an explicit move constructor
    /// because one will not be defined implicitly
    /// due to our non-copiable/non-movable std::recursive_mutex.
    /// A move constructor is needed to support initialization of a
    /// Switch class array class element.
    Switch(Switch const && move);
};
