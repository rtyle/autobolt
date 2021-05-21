# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS
	ADC.cpp
	AsioTask.cpp
	LEDC.cpp
	main.cpp
	Pin.cpp
	Qio.cpp
	Switch.cpp
	Task.cpp
	Timer.cpp
)
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
