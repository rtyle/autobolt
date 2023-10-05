# autobolt
Automatically bolt (lock) your manually operated garage door when it closes.

## Parts
[motorbrainz board](https://github.com/rtyle/motorbrainz)

[Liftmaster 841LM Automatic Garage Door Lock](https://www.amazon.com/Liftmaster-841LM-Automatic-Garage-Door/dp/B074L9JQQR)

Two [magnetic reed switch](https://www.digikey.com/en/products/detail/standex-meder-electronics/MK04-1A66B-500W/388233)es paired with appropriate (neodymium?) magnets

[10 (2x5) pin, 1.27mm pitch socket-socket ribbon cable assembly](https://www.digikey.com/en/products/detail/adafruit-industries-llc/1675/6827142)

[AC/DC wall mount adapter, +12V (on center pin), 2.1A](https://www.digikey.com/en/products/detail/cui-inc/SWI25-12-N-P5/7070093)

[2.1mm x 5.5mm barrel jack](https://www.digikey.com/en/products/detail/tensility-international-corp/54-00063/6206244)

[3mm LEDs](https://www.amazon.com/gp/product/B077XFLX13)

[3mm LED holders](https://www.amazon.com/gp/product/B07D9HCNDX)

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

Identify serial port and baud rate to connected device.
For example,

    port=/dev/ttyUSB0
    baud=115200

Prepare connected device and deploy application.

    (cd project; idf.py -b $baud -p $port erase_flash)
    (cd project; idf.py -b $baud -p $port flash)

Monitor serial output from device.

    (cd project; idf.py -b $baud -p $port monitor)

## Installation

Pull motorbrainz control inputs 1 and 2 high (+3V3) with 10k resistors and bridge their other sides to ground.
A closed reed switch across an input pair will take the input from high to low.
These inputs will be controlled by the door (1) and bolt (2) reed switches.
Harvest one 2x5 connector with a pigtail from the cable assembly.
From the right, connect the door and bolt reed switches across the first two wire pairs.

From the left, connect the bolt and door LEDs (with series current limiting resistors)
across the last two (control 5 and 4) wire pairs (observing proper LED polarity).
Mount LEDs to case using holders.

Open the 841LM case and slide the motorbrainz board into the corner below/behind the motor.
Secure and insulate, as necessary, with kapton tape.
Install the power jack and wire it to the +12V motorbrainz input.
Install jumper to select the +5V coming from the LDO powered by +12V.
Wire either motorbrainz output (or both in parallel, with same polarity) to the motor.
The green motor wire goes to the terminal with the circular pad(s); the yellow to the square pad(s).
Reed switches will be installed outside the case.
Make a way to route their wires to the motorbrainz control connector.

Reassemble 841LM and install on garage track per its instructions.
Locate reed switches and magnets so that
the door switch closes when the garage door closes and
the bolt switch closes when the bolt is in the locked position.
Plug it in to the +12V power adapter.

## Usage
The bolt/door LED will light when the bolt/door is open.
The bolt should close a few seconds after the garage door does.
