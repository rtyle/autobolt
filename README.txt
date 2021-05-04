# git

	https://github.com/rtyle?tab=repositories
		New
			Repository name:	autobolt
			Create repository

	git clone git@github.com:rtyle/autobolt.git

	cd autobolt

	git add README.txt
	git commit -m 'git'

# esp-idf

	# give $USER access to /dev/ttyUSB* and relogin
	# this is needed to allow for USB communication with esp32
	sudo usermod -a -G dialout $USER

	# toolchain prerequisites
	# https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-setup.html

		sudo dnf install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache dfu-util libusbx

	# esp-idf
	# https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#get-started-get-esp-idf

		# install esp-idf as a submodule under project
		git submodule add https://github.com/espressif/esp-idf.git

		# update now (and/or later)

			(
				cd esp-idf
				git fetch
				git checkout v4.3-beta3
				git submodule update --init --recursive
			)

	git add README.txt esp-idf
	git commit -m 'esp-idf v4.3-beta3'
