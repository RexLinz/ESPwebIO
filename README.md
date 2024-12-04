# ESPwebIO

## Program to access ESP IO via HTTP requests

The intention with this program is to allow simple access to IO's on ESP devices via HTTP requests either from the web browser or other programs which allow to send http GET or POST requests, e.g.

- <http://WebIO/GPIO?state=1,22> to read state of GPIO pins 1 and 11
- <http://WebIO/GPIO?set=23> to set GPIO pin 23 active

My usecase is remote outputs to be controlled from GNU Octave (similar to Matlab) for automated test and measurement.

ESP's hardware blocks will be implemented as individual **subsystems**

### Subsystems available right now

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

## Revision history

- V1.0 access to GPIO
