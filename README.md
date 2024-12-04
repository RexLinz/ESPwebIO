# ESPwebIO

## Program to access ESP IO via HTTP requests

The intention with this program is to allow simple access to IO's on ESP devices via HTTP requests either from the web browser or other programs which allow to send http GET or POST requests, e.g.

- <http://WebIO/GPIO?state=1,22> to read state of GPIO pins 1 and 11
- <http://WebIO/GPIO?set=23> to set GPIO pin 23 active

My usecase is using outputs on an ESP32 to be controlled via WiFi from GNU Octave (similar to Matlab) for automated test and measurement. In GNU Octave and Matlab the first example above will read as below.

> `webread("http://WebIO/GPIB?state=1,22")`
>
> `webread("http://WebIO/GPIB", "state", "1,22")`

**Note** in GNU Octave and Matlab you can code the arguments within the URL or specify as key / value pairs, so both are equivalent.

ESP's hardware blocks will be implemented as individual **subsystems**. Each subsystem will print additional (plain) help text if called without arguments.

If a query is returning value(s) this response will be JSON formatted for easy processing in the client's application. Web browsers will show this in a nice way as well.

### Subsystems available right now

- `/status` print version, memory and WiFi information
- [/GPIO](#gpio) access to GPIO (digital pin configuration, input and output)

### Planned in future

- `/Serial` sending and receiving from serial lines (UART)
- `/ADC` reading analog input
- `/DAC` set analog output
- `/I2C` IO via I2C
- `/SPI` IO via SPI
- `/PWM` PWM output
- ...

## Subsystem description

### GPIO

#### digital IO

- set/clear/toggle=pins ... new digital pin state
- state=pins ... query digital pin state

#### configuration

- input/inputPullUp/inputPullDown=pins ... digital input pin modes
- output/outputOpenDrain=pins ... digital output pin modes
- analog=pins ... analog input mode
- mode=pins ... query pin mode

#### specifying pin(s)

With string **pins** is a list of comma separated **GPIO** numbers (not ESP chip or development kit pin numbers) e.g. <http://webIO/GPIO/state=22,23> reading state of GPIO pins 22 and 23

#### return value

Commands writing data return 1 as result if successful, 0 on error

## Revision history

- V1.0 access to GPIO
