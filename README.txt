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
