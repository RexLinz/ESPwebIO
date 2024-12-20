#ifndef _WEBIO_H_
#define _WEBIO_H_
#include <Arduino.h>

// JSON online tools for testing
// https://emn178.github.io/online-tools/json/formatter/ 

#define WEBIO_VERSION "WebIO version 1.4.2\r\n"

// some utility functions which might be useful to several parsers 
class espUtil
{
protected:
    // get the next uint8 value from list, remove that from list
    static int nextInt(String &list, String delim=",");
    // get the next float value from separated list, remove that from list
    static float nextFloat(String &list, String delim=",");
    // get the next String value from separated list, remove that from list
    static String nextString(String &list, String delim=",");
public:
    static const String help; // root help
    static String status(); // ESP status without WiFi
};


class espGPIO : private espUtil
{
private:
    uint8_t savedPinMode[SOC_GPIO_PIN_COUNT]; // 0..39
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
public:
    espGPIO(); // initialize
    static const String help; // return help string
    String parse(String command, String value);
};

// ESP has one GPIO instance
extern espGPIO webGPIO;


class espSerial
{
private:
    HardwareSerial &_serial;
    String _endOfLine = "\r\n";
    String rxBuffer = ""; // receive buffer
    void begin(unsigned long baud) { _serial.begin(baud); };
    void setBaud(unsigned long baud) { _serial.updateBaudRate(baud); };
    void rxPin(uint8_t rxPin) { _serial.setPins(rxPin, -1); };
    void txPin(uint8_t txPin) { _serial.setPins(-1, txPin); };
    void setTerm(String t); // set termination 
    void write(String s) { _serial.print(s); }; // send string s
    void writeln(String s) { _serial.print(s+_endOfLine); }; // ... adding line termination
    String read();   // read all available data
    String readln(); // read single line removing configured line termination
public:
    espSerial(HardwareSerial &hwserial) : _serial(hwserial) {};
    static const String help;
    String parse(String command, String value);
};

extern espSerial webSerial0;
extern espSerial webSerial1;
extern espSerial webSerial2;

// ESP32 has two DAC's
class espDAC
{
private:
    uint8_t dacPin;
    // TODO set default scale and offset to get approximately voltage specified
    float _scale = 1.0f;
    float _offset = 0.0f;
    // raw output
    void setRaw(uint8_t value);
    // scaled output
    void setOffset(int o) { _offset = o; };
    void setScale(float s) { _scale = s; };
    void setValue(float val) { setRaw(round(val*_scale + _offset)); };
    void disable() { dacDisable(dacPin); };
public:
    espDAC(uint8_t pin) : dacPin(pin) {};
    static const String help;
    String parse(String command, String value);
};

extern espDAC webDAC1;
extern espDAC webDAC2;

// ESP32 has two ADC's, but ADC2 is blocked by WiFi!
class espADC : private espUtil
{
private:
    String pins=""; // list of pins for commands working on multiple inputs

//    adc_attenuation_t attenuation = ADC_6db; // best performance
//    void setAttenuation(String a); // all channels
//    void setAttenuation(uint8_t pin, String a); // specific channel
    adc_attenuation_t attenuationValue(String a); // convert string to enum
    void setAttenuation(String pinList, String valueList); // multiple channels

//    uint16_t oversampling=1; // oversampling (sum n values)
//    void setOversampling(uint16_t n) { oversampling = n; };
    uint16_t oversampling[SOC_GPIO_PIN_COUNT];
    void setOversampling(String pinList, String valueList);

//    float offset = 0.0f;
//    void setOffset(float o) { offset = o; };
    float offset[SOC_GPIO_PIN_COUNT];
    void setOffset(String pinList, String valueList);

//    float scale = 1.0f;
//    void setScale(float s) { scale = s; };
    float scale[SOC_GPIO_PIN_COUNT];
    void setScale(String pinList, String valueList);

    // read raw data from pin x (after summing n readings as configured with oversampling)
    uint32_t getRaw(uint8_t pin);
    // scaled input 
    float getValue(uint8_t pin) { return ((getRaw(pin)-offset[pin]) * scale[pin]); };
    String parseList(String command, String numberList);
public:
    espADC(adc_attenuation_t att=ADC_6db); // default, best performance of ADC
    static const String help;
    String parse(String command, String value); // parameters to be determined
};
extern espADC webADC;

/* TODO future classes to be implemented

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
