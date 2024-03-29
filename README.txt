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

		sudo dnf install git wget flex bison gperf python3 cmake ninja-build ccache dfu-util libusbx

	# esp-idf
	# https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#get-started-get-esp-idf

		# install esp-idf as a submodule under project
		git submodule add https://github.com/espressif/esp-idf.git

		# update now (and/or later)

			(
				cd esp-idf
				git fetch
				git checkout v4.4.6
				git submodule update --init --recursive
			)

	git add README.txt esp-idf
	git commit -m 'esp-idf v4.4.6'

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

	# the USB to serial converter on the TinyPICO NANO
	# collides with that on the esp-prog
	# the CP2104 chip and its auto reset transistors must be removed

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

	git add README.txt
	git commit -m 'esp-prog - motorbrainz 1.0'

# esp-prog - motorbrainz 2.0

	# the USB to serial converter on the TinyPICO NANO
	# (CP2104 chip and its auto reset transistors)
	# collided with similar functionality on the esp-prog board.
	# the PROG header on the motorbrainz 1.0 board was replaced
	# with a micro USB jack to resolve this.
	# power and programming use this USB interface.
	# the esp-prog/motorbrainz JTAG interfaces can be used for debugging.

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

# esp-idf-tools JTAG debugging

	https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/jtag-debugging/index.html

	# terminal for serial output monitor

		minicom -D /dev/ttyUSB1
		OR
		(cd project; idf.py monitor)

	# terminal for openocd

		# esp-prog uses an FTDI chip like the esp32-wrover-kit
		# ESP32-PICO-D4's integrated external SPI flash is 3.3v

		openocd -f board/esp32-wrover-kit-3.3v.cfg \
			-c 'program_esp32 project/build/bootloader/bootloader.bin 0x1000' \
			-c 'program_esp32 project/build/partition_table/partition-table.bin 0x8000' \
			-c 'program_esp32 project/build/project.bin 0x10000'

		(cd project; openocd -f board/esp32-wrover-kit-3.3v.cfg)
		OR
		(cd project; idf.py openocd)

			Open On-Chip Debugger  v0.10.0-esp32-20200709 (2020-07-09-08:54)
			Licensed under GNU GPL v2
			For bug reports, read
				http://openocd.org/doc/doxygen/bugs.html
			Info : Configured 2 cores
			Info : Listening on port 6666 for tcl connections
			Info : Listening on port 4444 for telnet connections
			Info : ftdi: if you experience problems at higher adapter clocks, try the command "ftdi_tdo_sample_edge falling"
			Info : clock speed 20000 kHz
			Info : JTAG tap: esp32.cpu0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
			Info : JTAG tap: esp32.cpu1 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
			Info : esp32: Debug controller 0 was reset.
			Info : esp32: Core 0 was reset.
			Info : esp32: Debug controller 1 was reset.
			Info : esp32: Core 1 was reset.
			Info : Listening on port 3333 for gdb connections

	# build

		(cd project; idf.py build)

	# terminal for gdb (project should have already been flashed)

		(cd project; xtensa-esp32-elf-gdb build/project.elf)

			# connect to openocd
			target remote :3333

			# reset processor and immediately halt it
			mon reset halt

			# flash from gdb
			# less the "mon",
			# these commands can be given directly to openocd as -c options
			mon program_esp32 build/bootloader/bootloader.bin 0x1000
			mon program_esp32 build/partition_table/partition-table.bin 0x8000
			mon program_esp32 build/project.bin 0x10000

			# set a temporary hardware breakpoint at app_main
			thb app_main

			# continue until breakpoint
			c

		OR
		(cd project; idf.py gdb)

			# will automatically
			# connect to openocd
			# reset processor and immediately halt it
			# set a temporary hardware breakpoint at app_main
			# continue until breakpoint

		OR
		(cd project; idf.py gdbtui)

			# will do the same with the gdb terminal user interface

		OR
		(cd project; idf.py gdbgui)

			# does not work.
			# should do the same (?) with the gdb graphical user interface (browser based)
			#	http://127.0.0.1:5000

	rm -rf project/build
	git add README.txt
	git commit -m 'esp-idf-tools JTAG debugging'

# eclipse

	sudo dnf install eclipse eclipse-cdt eclipse-mpc

	# fedora 34 packaged eclipse does not work with espressif/idf-eclipse-plugin
	# https://github.com/espressif/idf-eclipse-plugin/issues/260
	# download from eclipse.org

		curl https://www.eclipse.org/downloads/download.php?file=/technology/epp/downloads/release/2021-03/R/eclipse-cpp-2021-03-R-linux-gtk-x86_64.tar.gz | tar xzf -
		mv eclipse ~/eclipse-2021-03

	# get rid of old eclipse artifacts

		rm -rf ~/.{eclipse,p2}

# https://docs.espressif.com/projects/esp-idf/en/latest/get-started/eclipse-setup.html

	echo .metadata >> .gitignore
	git rm -r project

	(. esp-idf-export.sh; ~/eclipse-2021-03/eclipse >/dev/null 2>&1 &)

		#! set workspace to here

		Help: Install New Software...: Add...: 

			# espressif/idf-eclipse-plugin

				Name:		espressif/idf-eclipse-plugin latest
				Location:	https://dl.espressif.com/dl/idf-eclipse-plugin/updates/latest/

				Add

				X Espressif IDF

				Install anyway
				Restart Now

		File: New: Project...

			Wizard: Espressif: Espressif IDF Project
			Project name: project

		Project Explorer: <project>

			toolbar: Launch Target
				Name:		esp32
				IDF Target:	esp32
				Serial Port:	/dev/ttyUSB1

			toolbar: Build

			toolbar: Open a Terminal
				Choose terminal:	ESP-IDF Serial Monitor
					this doesn't work
				Choose terminal:	Serial Terminal

			toolbar: Launch in 'Run' mode

		<project>: Run As: Run Configurations...
			ESP-IDF Application
				project
					Common
						Display in favorites menu
							0 Debug
							X Run
					Apply
					Close

		<project>: Debug As: Debug Configurations...
			ESP-IDF GDB OpenOCD Debugging
				Name:	project debugger
				New Configuration
					Debugger
						OpenOCD Setup
							Executable path:	openocd
							Config options:		-f board/esp32-wrover-kit-3.3v.cfg
						GDB Client Setup
							Actual executable:	xtensa-esp32-elf-gdb
					Startup
						Initialization Commands
							0 Enable ARM semihosting
					Common
						Display in favorites menu
							X Debug
							0 Run
					Apply
					Close

		Project Explorer: <project>

			toolbar: Launch Configuration; project debugger
			toolbar: Launch Mode: Debug
			toolbar: Launch in 'Debug' mode

				# auto change to debug perspective
				# find code stopped at app_main entry point

		exit eclipse

	rm -rf project/build
	rm project/sdkconfig*

	echo build	>> project/.gitignore
	echo sdkconfig	>> project/.gitignore
	echo *.old	>> project/.gitignore
	echo *.swp	>> project/.gitignore

	git add README.txt .gitignore project
	git commit -m 'idf-eclipse-plugin'

eclipse preferences

	General: Editors: Text Editors
		Displayed tab width:	8
		X Show print margin
		X Show line numbers
		X Show whitespace characters

	C/C++: Code Style: Formatter
		Active profile:	K&R [built-in]: Edit...
			Profile Name: K&R [built-in] sw=4 ts=8
				Tab policy:		Mixed
				Indentation size:	4
				Tab size:		8

	exit eclipse

	git add README.txt
	git commit -m 'eclipse preferences'

configuring build type and
flashing from command line
	
	# reconfigure for build type
	idf.py reconfigure -DCMAKE_BUILD_TYPE=Release
	idf.py reconfigure -DCMAKE_BUILD_TYPE=Debug

	idf.py build
	idf.py flash

flashing from eclipse

	# flash the build from the current configuration

		toolbar: Launch configuration: project
		toolbar: Launch in 'Run' mode
		
	# we should be able to flash on debugger startup but
	# this depends on openocd and gdb and
	<project>: Debug As: Debug Configurations...
		ESP-IDF GDB OpenOCD Debugging: project debugger
			Startup
				Load Symbols and Executable
					X Load executable
					X Use file:			${workspace_loc:/project/build/project.bin}
					Executable offset (hex):	10000
			Apply
			Close
	
