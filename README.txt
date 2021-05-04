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

# esp-idf-tools

	# install tools required by esp-idf in esp-idf-tools
	IDF_TOOLS_PATH=esp-idf-tools esp-idf/install.sh

	# note environment differences made by esp-idf/export.sh
	# the output has been edited to establish and reference IDF_PATH and IDF_TOOLS_PATH
	diff <(env | sort) <(export IDF_TOOLS_PATH=$(realpath esp-idf-tools); . esp-idf/export.sh; env | sort)

		IDF_PATH=$(realpath esp-idf)
		IDF_TOOLS_EXPORT_CMD=$IDF_PATH/export.sh
		IDF_TOOLS_INSTALL_CMD=$IDF_PATH/install.sh

		IDF_TOOLS_PATH=$(realpath esp-idf-tools)
		IDF_PYTHON_ENV_PATH=$IDF_TOOLS_PATH/python_env/idf4.3_py3.9_env
		OPENOCD_SCRIPTS=$IDF_TOOLS_PATH/tools/openocd-esp32/v0.10.0-esp32-20200709/openocd-esp32/share/openocd/scripts

						PATH=\
		$IDF_PATH/components/esptool_py/esptool:\
		$IDF_PATH/components/espcoredump:\
		$IDF_PATH/components/partition_table:\
		$IDF_PATH/components/app_update:\
		$IDF_TOOLS_PATH/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin:\
		$IDF_TOOLS_PATH/tools/xtensa-esp32s2-elf/esp-2020r3-8.4.0/xtensa-esp32s2-elf/bin:\
		$IDF_TOOLS_PATH/tools/xtensa-esp32s3-elf/esp-2020r3-8.4.0/xtensa-esp32s3-elf/bin:\
		$IDF_TOOLS_PATH/tools/riscv32-esp-elf/1.24.0.123_64eb9ff-8.4.0/riscv32-esp-elf/bin:\
		$IDF_TOOLS_PATH/tools/esp32ulp-elf/2.28.51-esp-20191205/esp32ulp-elf-binutils/bin:\
		$IDF_TOOLS_PATH/tools/esp32s2ulp-elf/2.28.51-esp-20191205/esp32s2ulp-elf-binutils/bin:\
		$IDF_TOOLS_PATH/tools/openocd-esp32/v0.10.0-esp32-20200709/openocd-esp32/bin:\
		$IDF_PYTHON_ENV_PATH/bin:\
		$IDF_TOOLS_PATH:\
		$PATH

	install /dev/stdin esp-idf-export.sh <<EOF
export IDF_TOOLS_PATH=$(realpath esp-idf-tools); . esp-idf/export.sh
EOF

	# (re)establish this environment for idf.py and eclipse actions

		. esp-idf-export.sh

	echo esp-idf-tools > .gitignore

	git add README.txt esp-idf-export.sh .gitignore
	git commit -m 'esp-idf-tools'

# esp-prog - motorbrainz 1.0

	# cable between JTAG and PROG headers on esp-prog and motorbrainz boards

		esp-prog

			VJTAG jumper may be removed (not connected on motorbrainz board)
			VPROG jumper should be 5V (BOOT and RST levels will still be 3V3 based)
			IO_0 jumper must be on for esp-prog to affect download boot


		motorbrainz

			set power jumper get 5V from esp-prog
			or 5V regulated from external 12V 

	# esp-prog LED indicators

		red	3V3 power
		blue	RX from ESP32
		green	TX to ESP32

	# esp-prog FTDI F22232HL dual asynchronous serial interfaces

		# /dev/ttyUSB0	JTAG	(first, numbered 0 if no other USB ttys)
		# /dev/ttyUSB1	PROG	(first + 1)

	# connect to PROG 115200 8N1
	minicom -D /dev/ttyUSB1

		# holding the BOOT button while pushing the RST button on esp-prog
		# should reset the TinyPICO NANO and wait for new firmware download

			rst:0x1 (POWERON_RESET),boot:0x3 (DOWNLOAD_BOOT(UART0/UART1/SDIO_REI_REO_V2))
			waiting for download  

		# pushing RST button alone on esp-prog
		# should reset the TinyPICO NANO and load the resident firmware
		# MicroPython based firmware from Unexpected Maker will fail ...

			rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
			configsip: 188777542, SPIWP:0xee
			clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
			mode:DIO, clock div:1
			load:0x3fff0018,len:4
			load:0x3fff001c,len:5244
			load:0x40078000,len:14068
			load:0x40080400,len:3796
			entry 0x40080634
			Warning: SPI(-1, ...) is deprecated, use SoftSPI(...) instead

			Hello from TinyPICO!
			--------------------

			Battery Voltage is 3.7V
			Battery Charge State is True

			Memory Info - micropython.mem_info()
			------------------------------------
			stack: 752 out of 15360
			GC: total: 4098240, used: 4816, free: 4093424
			 No. of 1-blocks: 73, 2-blocks: 19, max blk sz: 25, max free sz: 255677
			Traceback (most recent call last):
			  File "main.py", line 82, in <module>
			  File "main.py", line 44, in print_temp
			AttributeError: 'module' object has no attribute 'get_internal_temp_F'
			MicroPython v1.13-283-g203e1d2a6 on 2021-01-27; TinyPICO with ESP32-PICO-D4
			Type "help()" for more information.
			>>> 

	# the USB to serial converter on the TinyPICO NANO
	# collides with that on the esp-prog
	# the CP2104 chip and its auto reset transistors must be removed

	git add README.txt
	git commit -m 'esp-prog - motorbrainz 1.0'

# idf.py create empty project, build, flash, monitor

	# create project subdirectory with CMakeLists.txt and empty main/

		idf.py create-project project

	# project development

		(cd project

			idf.py --help

		# erase, flash, monitor

			idf.py -p /dev/ttyUSB1 erase_flash
			idf.py flash
			idf.py monitor

			rm -rf build
		)

	git add README.txt project
	git commit -m 'idf.py create empty project, build, flash, monitor'
