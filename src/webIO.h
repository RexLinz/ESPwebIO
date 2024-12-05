#ifndef _WEBIO_H_
#define _WEBIO_H_
#include <Arduino.h>

// JSON online tools for testing
// https://emn178.github.io/online-tools/json/formatter/ 

// get the next uint8 value from list, 
// remove that from list
int nextInt(String &list);

// get the next float value from comma separated list, 
// remove that from list
float nextFloat(String &list);

class espGPIO
{
private:
    uint8_t savedPinMode[NUM_OUPUT_PINS];
public:
    espGPIO(); // initialize
    String help(); // return help string
    // use pin as ...
    String modeOutput(uint8_t pin);          // digital output
    String modeOutputOpenDrain(uint8_t pin); // digital open drain output
    String modeInput(uint8_t pin);           // digital input
    String modeInputPullUp(uint8_t pin);     // digital input with pull up
    String modeInputPullDown(uint8_t pin);   // digital input with pull down
    String modeAnalog(uint8_t pin);          // analog input
    String mode(uint8_t pin);                // read pin mode
    // digital output
    String set(uint8_t pin);    // set output
    String clear(uint8_t pin);  // clear output
    String toggle(uint8_t pin); // toggle output
    // digital input
    String state(uint8_t pin);  // read state
    String parse(String command, String value);
};

// ESP has one GPIO instance
extern espGPIO webGPIO;

// TODO future classes to be implemented
/*

class espSerial
{
private:
    HardwareSerial &_serial;
    String _endOfLine = "";
public:
    espSerial(HardwareSerial &interface) : _serial(interface) {};
    void begin(unsigned long baud, uint8_t rxPin=-1, uint8_t txPin=-1);
    void baudRate(unsigned long baud) { _serial.updateBaudRate(baud); };
    void setPins(uint8_t rxPin=-1, uint8_t txPin=-1) { _serial.setPins(rxPin, txPin); };
    void termChar(String t) { _endOfLine = t; }; // set termination 
    void print(String s) { _serial.print(s); }; // send string s
    void println(String s) { _serial.print(s+_endOfLine); }; // ... will add termination
    String read();   // read all available data
    String readln(); // read a line up to configured termination character(s)
    String parse();  // parameters to be determined
};

// ESP32 has 3 async serial interfaces 
espSerial webSerial0(Serial);  // pins TX=GPIO0, RX=GPIO1 - connected to USB
espSerial webSerial1(Serial1); // not available on Dev Kit C without remapping pins
espSerial webSerial2(Serial2); // pins TX=GPIO17, RX=GPIO16

// ESP32 has two ADC's, but ADC2 is blocked by WiFi!
class espADC
{
private:
    ADC &_adc;
    uint _n; // oversampling (sum n values)
    int _offset = 0;
    float _scale = 1.0f;
public:
    void oversampling(uint n);
    // read raw data from pin x (after summing n readings)
    int raw(uint8_t pin);
    // scaled input TODO manage per pin
    void offset(int o) { _offset = o; };
    void scale(float s) {_scale = s; };
    float value(uint8_t pin) { return ((raw(pin)-_offset) * _scale};
    String parse(); // parameters to be determined
};
espADC webADC1(ADC1);

// ESP32 has two DAC's
class espDAC
{
private:
    DAC &_dac;
    int _offset = 0;
    float _scale = 1.0f;
public:
    espDAC(DAC ?dac) : _dac(dac);
    void raw(int value);
    // scaled output
    void offset(int o) { _offset = o; };
    void scale(float s) {_scale = s; };
    void value(float val) { raw(round(val * _scale) + _offset); };
    String parse(); // parameters to be determined
};

espDAC webDAC1(DAC1);
espDAC webDAC2(DAC2);

class espI2C
{
private:
public:
};

class espSPI
{
private:
public:
};

class espPWM
{
private:
public:
};
*/

#endif