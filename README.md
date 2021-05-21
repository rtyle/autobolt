# autobolt
Automatically bolt (lock) your manually operated garage door when it closes.

## Parts
[motorbrainz board](https://github.com/rtyle/motorbrainz)

[Liftmaster 841LM Automatic Garage Door Lock](https://www.amazon.com/Liftmaster-841LM-Automatic-Garage-Door/dp/B074L9JQQR)

Two [magnetic reed switch](https://www.digikey.com/en/products/detail/standex-meder-electronics/MK04-1A66B-500W/388233)es paired with appropriate (neodymium?) magnets

[10 (2x5) pin, 1.27mm pitch socket-socket ribbon cable assembly](https://www.digikey.com/en/products/detail/adafruit-industries-llc/1675/6827142)

[AC/DC wall mount adapter, +12V (on center pin), 2.1A](https://www.digikey.com/en/products/detail/cui-inc/SWI25-12-N-P5/7070093)

[2.1mm x 5.5mm barrel jack](https://www.digikey.com/en/products/detail/tensility-international-corp/54-00063/6206244)

## Software Build, Deploy and Monitor

This project was built on a Fedora Linux platform.
Your mileage may vary elsewhere.

Get autobolt source code with its dependencies

    git clone http://github.com/rtyle/autobolt.git
    cd autobolt
    git submodule update --init --recursive

Install tools required by esp-idf in esp-idf-tools.

    IDF_TOOLS_PATH=esp-idf-tools esp-idf/install.sh

(Re)establish build environment.

    . esp-idf-export.sh

Build application.

    (cd project; idf.py fullclean reconfigure build)

Grant user access to USB serial devices then **relogin** to get access.

    sudo usermod -a -G dialout $USER

Identify serial port to connected device.
For example,

    port=/dev/ttyUSB0

Prepare connected device and deploy application.

    (cd project; idf.py -p $port erase_flash)
    (cd project; idf.py -p $port flash)

Monitor serial output from device.

    (cd project; idf.py -p $port monitor)

## Installation

Pull motorbrainz control inputs 0 and 1 high (+3V3) with 10k resistors and bridge their other sides to ground.
A closed reed switch across an input pair will take the input from high to low.
These inputs will be controlled by the door (0) and bolt (1) reed switches.
Harvest one 2x5 connector with a pigtail from the cable assembly.
From the right, connect the door and bolt reed switches across the first two wire pairs.

Open the 841LM case and slide the motorbrainz board into the corner below/behind the motor.
Secure and insulate, as necessary, with kapton tape.
Install the power jack and wire it to the +12V motorbrainz input.
Wire either motorbrainz output (or both in parallel) to the motor.
TODO: describe polarity.
Reed switches will be installed outside the case.
Make a way to route their wires to the motorbrainz control connector.

Reassemble 841LM and install on garage track per its instructions.
Locate reed switches and magnets so that
the door switch closes when the garage door closes and
the bolt switch closes when the bolt is in the locked position.
Plug it in to the +12V power adapter.

## Usage
The bolt should close shortly after the garage door does.
