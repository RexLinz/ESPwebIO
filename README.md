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

Multiple requests to a single subsystem could be done in one request and will be processed in order specified, e.g.
<http://WebIO/GPIO?output=23&toggle=23&state=23> will set GPIO pin 23 to OUTPUT, toggle the state and read the state resulting.

If a query is returning value(s) this response will be JSON formatted for easy processing in the client's application. Web browsers will show this in a nice way as well.

### Subsystems available right now

- `/status` print version, memory and WiFi information
- [/GPIO](#gpio) access to GPIO (digital pin configuration, input and output)
- [/Serial](#serial) sending and receiving from serial lines (UART)

### Planned in future

- `/ADC` reading analog input
- `/DAC` set analog output
- `/I2C` IO via I2C
- `/SPI` IO via SPI
- `/PWM` PWM output
- ...

## Subsystem description

### GPIO

Access to GPIO (digital pin configuration, input and output)

#### GPIO configuration

- input/inputPullUp/inputPullDown=pins ... digital input pin modes
- output/outputOpenDrain=pins ... digital output pin modes
- analog=pins ... analog input mode
- mode=pins ... query pin mode

#### digital IO

- set/clear/toggle=pins ... new digital pin state
- state=pins ... query digital pin state

#### specifying pin(s)

With string **pins** is a list of comma separated **GPIO** numbers (not ESP chip or development kit pin numbers) e.g. <http://webIO/GPIO?state=22,23> reading state of GPIO pins 22 and 23

#### return value

Commands writing data return 1 as result if successful, 0 on error

### Serial

Sending and receiving from serial lines (UART)

- **/Serial0** pins TX=GPIO0, RX=GPIO1 - connected to USB. Also available as **/Serial**
- **/Serial1** not available on Dev Kit C without remapping pins
- **/Serial2** pins TX=GPIO17, RX=GPIO16

#### Serial configuration

- begin=baud   ... initialize. Equal to Serial.begin(baudrate) on Arduino. **NOTE** at the moment serial settings are fixed to 8N1
- setBaud=baud ... change baud rate. Equal to Serial.setBaud(baudrate) on Arduino
- txPin=pin
- rxPin=pin
- setTerm=CR|LF|CRLF (default)

**NOTE** if accessing a serial port on different pins than default set txPin and rxPin before calling begin, e.g. <http://WebIO/Serial1?txPin=23&rxPin=22&begin(115200)> to configure Serial1 using txPin 23, rxPin 22 at 115200 baud, 8N1.

#### reading and writing to Serial

- write=string ... send string out to client
- writeln=string ... send string adding configured line termination
- read ... read all data available on serial port
- readln ... read single line removing configured line termination. If a full line is not available return empty data.

## Revision history

- V1.1 added access to Serial (=Serial0), Serial1 and Serial2
- V1.0 access to GPIO
