# ESPwebIO

## Program to access ESP IO via HTTP requests

The intention with this program is to allow simple access to IO's on ESP devices via HTTP requests either from the web browser or other programs which allow to send http GET or POST requests, e.g.

- <http://WebIO/GPIO?state=1,22> to read state of GPIO pins 1 and 22
- <http://WebIO/GPIO?set=23> to set GPIO pin 23 active

My usecase is using outputs on an ESP32 to be controlled via WiFi from GNU Octave (similar to Matlab) for automated test and measurement. In GNU Octave and Matlab the first example above will read as below.

> `webread("http://WebIO/GPIB?state=1,22")`
or
> `webread("http://WebIO/GPIB", "state", "1,22")`

**Note** in GNU Octave and Matlab you can code the arguments within the URL or specify as key / value pairs, so both are equivalent.

ESP's hardware blocks will be implemented as individual **subsystems**. Each subsystem will print additional (plain) help text if called without arguments.

Multiple requests to a single subsystem could be done in one request and will be processed in order specified, e.g.
<http://WebIO/GPIO?output=23&toggle=23&state=23> will set GPIO pin 23 to OUTPUT, toggle the state and read the state resulting.

If a query is returning value(s) this response will be JSON formatted for easy processing in the client's application. Web browsers will show this in a nice way as well.

### TCP connections

Starting with version 2.1 communication is also accepted via TCP or other stream based connection if enabled in main.cpp (see streamIO). After connecting to the device requests are sent similar to http requests, terminated by \004 = End Of Text (EOT).  
The example above is equal to sending `/GPIB?state=1,22` via TCP. Response sent back is formatted equal to http requests and terminated with EOT to simplify parsing of response.

### Specifying pin(s)

Commands where **pins** have to be specified accept a list of comma separated **GPIO** numbers (not ESP chip or development kit pin numbers) e.g. <http://webIO/GPIO?state=22,23> reading state of GPIO pins 22 and 23

### Subsystems available right now

- `/` ... version and overview of implemented subsystems
- `/status` return system status: memory and WiFi information
- [/GPIO](#gpio) access to GPIO (digital pin configuration, input and output)
- [/Serial](#serial) sending and receiving from serial lines (UART)
- [/DAC](#dac) digital to analog converter output
- [/ADC](#adc) reading analog to digital converter input

### Planned in future

- `/I2C` IO via I2C
- `/SPI` IO via SPI
- `/PWM` PWM output
- ...

## Subsystem description

### GPIO

Access to GPIO (digital pin configuration, input and output)

#### GPIO configuration

Return OK or error message for each pin

- input/inputPullUp/inputPullDown=pins ... digital input pin modes
- output/outputOpenDrain=pins ... digital output pin modes
- analog=pins ... analog input mode

Query pin mode

- mode=pins ... return string

#### Digital IO

Commands returning (new) pin state or error message

- set/clear/toggle=pins ... define pin state
- state=pins ... query digital pin state

### Serial

Sending and receiving from serial lines (UART)

- **/Serial0** pins TX=GPIO0, RX=GPIO1 - connected to USB. Also available as **/Serial**
- **/Serial1** not available on Dev Kit C without remapping pins
- **/Serial2** pins TX=GPIO17, RX=GPIO16

#### Serial configuration

- txPin=pin
- rxPin=pin
- txBufferSize=size
- rxBufferSize=size
- setTerm=CR|LF|CRLF (default)
- setBaud=baud ... change baud rate. Equal to Serial.setBaud(baudrate) on Arduino
- begin=baud   ... initialize at given baud rate  
Equal to Serial.begin(baudrate) on Arduino.
**NOTE** at the moment serial settings are fixed to 8N1

**NOTE** if accessing a serial port on different pins than default set txPin and rxPin before calling begin, e.g. <http://WebIO/Serial1?txPin=23&rxPin=22&begin=115200> to configure Serial1 using txPin 23, rxPin 22 at 115200 baud, 8N1.

#### Reading and writing to Serial

- write=string ... send string out to client
- writeln=string ... send string adding configured line termination
- read ... read all data available on serial port
- readln ... read single line removing configured line termination. If a full line is not available return empty data.

### DAC

#### DAC configuration

- scale=float ... set scaling (default 1.0)
- offset=float ... set offset (default 0.0)
- disable ... disable DAC function on this channel (pin)

#### DAC output

- raw=uint_8 ... write out as is
- value=float ... write out scaled by round(float*scale+offset)

### ADC

#### ADC configuration

Attenuation, oversampling, offset and scale apply to a list of pins to be configured before.
For these commands a list of values or a single value (to be applied to all) could be specified, e.g.

- pins=36,39&attenuation=2.5dB,6dB ... set attenuation on input 36 to 2.5dB, input 39 to 6dB
- pins=36,39&scale=2.5 ... set scale on inputs 36 and 39 to 2.5
- pins=pinlist ... set list of pins, separated by commas

##### General settings

- attenuation=0dB|2.5dB|6dB|11dB ... set input attenuation, default is 6dB  
resulting of full scale range of about 1.0 / 1.35 / 1.9 / 3.3 V  
**NOTE** ADC show serious nonlinearity in 11dB configuration above 2.5 V
- oversampling=n ... return sum of n samples per read (noise reduction)

##### Configuration for reading scaled output

- scale=float ... set scaling (default 1.0)
- offset=float ... set offset (default 0.0)

#### Reading ADC input

- raw=pins ... read as is (uint)
- value=pins ... read scaled value=(raw-offset)*scale (float)\r\n";

## Known issues / ideas for further improvements

- At the moment some commands simply ignore invalid input instead of returning error codes
- There is no check of pin numbers or pin capabilities. For example some GPIO pins are input only and the ADC could not be used at all pins as well.

## Revision history

- v2.2 added PWM controller
- V2.1 added stream based connection, e.g. TCP or serial
- V2.0 reworked to get all but help and serial read/readln as JSON
- V1.4 ADC configuration per pin
- V1.3 added analog to digital converter /ADC
- V1.2 added digital to analog converters /DAC1, /DAC2
- V1.1 added access to /Serial (=/Serial0), /Serial1 and /Serial2
- V1.0 access to /GPIO
