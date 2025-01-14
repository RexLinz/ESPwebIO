#ifndef _WEBIO_H_
#define _WEBIO_H_
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// JSON online tools for testing
// https://emn178.github.io/online-tools/json/formatter/ 

#define WEBIO_VERSION "WebIO version 3.1.2\r\n"

// some utility functions which might be useful to several parsers 
class espRoot
{
protected:
    // get the next int value from list, remove that from list
    static int nextInt(String &list, const String delim=",");
    // get the next int value from list of hexadecimals, remove that from list
    static int nextHex(String &list, const String delim=",");
    // get the next float value from separated list, remove that from list
    static float nextFloat(String &list, const String delim=",");
    // get the next String value from separated list, remove that from list
    static String nextString(String &list, const String delim=",");
public:
    // add response to message, if message is nonempty add separator before
    void addResponse(String &message, const String &response, const String &separator);
    // complete a JSON response line
    String JSONline(const String &command, const String &result);
    static const String status(); // ESP status without WiFi
    virtual const String help(); // root help
    virtual String parse(const String &command, String value) { return ""; };
};
extern espRoot webRoot;


// ESP32 has one GPIO instance
class espGPIO : public espRoot
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
    const String help() final; // return help string
    String parse(const String &command, String value) final;
};
extern espGPIO webGPIO;


// ESP32 has three serial interfaces (UART)
class espSerial : public espRoot
{
private:
    HardwareSerial &_serial;
    String _endOfLine = "\r\n";
    String rxBuffer = ""; // internal receive buffer to allow delivering full lines
    String txPin(uint8_t txPin) { _serial.setPins(-1, txPin); return String(txPin); };
    String rxPin(uint8_t rxPin) { _serial.setPins(rxPin, -1); return String(rxPin); };
    // Arduino side buffer sizes (default is 128)
    String txBufferSize(size_t size) {_serial.setTxBufferSize(size); return String(size); };
    String rxBufferSize(size_t size) {_serial.setRxBufferSize(size); return String(size); };
    String setTerm(String &t); // set termination 
    String setBaud(unsigned long baud) { _serial.updateBaudRate(baud); return String(baud); };
    String begin(unsigned long baud) { _serial.begin(baud); return String(baud); };
    String write(const String &s) { _serial.print(s); return "\"OK\""; }; // send string s
    String writeln(const String &s) { _serial.print(s+_endOfLine); return "\"OK\""; }; // ... adding line termination
    String read();   // read all available data
    String readln(); // read single line removing configured line termination
public:
    explicit espSerial(HardwareSerial &hwserial) : _serial(hwserial) {};
    const String help() final;
    String parse(const String &command, String value) final;
};
extern espSerial webSerial0;
extern espSerial webSerial1;
extern espSerial webSerial2;


// ESP32 has two DAC's
class espDAC : public espRoot
{
private:
    uint8_t dacPin;
    // TODO set default scale and offset to get approximately voltage specified
    float _scale = 1.0f;
    float _offset = 0.0f;
    // raw output
    String setRaw(int value); // returns actual value set (clipped to range 0..255)
    // scaled output
    String setOffset(int o) { _offset = o; return String(o); };
    String setScale(float s) { _scale = s; return String(s); };
    String setValue(float val); // return scaled value set (clipped to range 0..255)
    String disable() { dacDisable(dacPin); return "\"OK\""; };
public:
    explicit espDAC(uint8_t pin) : dacPin(pin) {};
    const String help() final;
    String parse(const String &command, String value) final;
};
extern espDAC webDAC1;
extern espDAC webDAC2;


// ESP32 has two ADC's, but ADC2 is blocked by WiFi!
class espADC : public espRoot
{
private:
    String pins=""; // list of pins for commands working on multiple inputs

    bool validADCpin(int pin);
    String setPins(String pinList);

    adc_attenuation_t attenuationValue(String a); // convert string to enum
    String setAttenuation(String pinList, String valueList); // multiple channels

    uint16_t oversampling[SOC_GPIO_PIN_COUNT];
    String setOversampling(String pinList, String valueList);

    float offset[SOC_GPIO_PIN_COUNT];
    String setOffset(String pinList, String valueList);

    float scale[SOC_GPIO_PIN_COUNT];
    String setScale(String pinList, String valueList);

    // read raw data from pin x (after summing n readings as configured with oversampling)
    uint32_t getRaw(uint8_t pin);
    // scaled input 
    float getValue(uint8_t pin) { return ((getRaw(pin)-offset[pin]) * scale[pin]); };
    String parseList(String command, String numberList);
public:
    explicit espADC(adc_attenuation_t att=ADC_6db); // default, best performance of ADC
    const String help() final;
    String parse(const String &command, String value) final;
};
extern espADC webADC;


// 16 channel (LED) PWM controller, also suitable to be used as RC servo output
class espPWM : public espRoot
{
private:
    unsigned long _frequency = 50UL;
    unsigned _bits = 16;
    static const int NUM_CHANNELS = 16;
    unsigned _pins[NUM_CHANNELS];
    String _channelList;
    uint32_t maxVal(uint8_t channel);
    uint32_t clipVal(uint8_t channel, uint32_t val);
    String setFrequency(const String &freq);
    String setResolution(const String &bits);
    String setChannels(String channelList);
    String initChannel(uint8_t channel, uint8_t pin);
    String map(String channelList, String pinList);
    String stop(String channelList, String arg);
    String width(String channelList, String args);
    String duty(String channelList, String args); // range 0.0 ... 1.0
    String val(String channelList, String args); // range 0 to (2^bits)-1
public:
    espPWM();
    const String help() final;
    String parse(const String &command, String value) final;

};
extern espPWM webPWM;


// ESP32 has two I2C (TWI) controllers
class espI2C : public espRoot
{
private:
    TwoWire &bus;
    int sdaPin = -1; // default pin
    int sclPin = -1; // default pin
    uint32_t frequency = 0; // default = 100000
    String setPins(String args);
    String setFrequency(const String &Hz);
    String begin();
    String end();
    int address = -1;
    String setAddress(const String &address);
    String write(String hexArgs);
    String read(const String &numBytes);
    String scan();
public:
    explicit espI2C(TwoWire &twi) : bus(twi) {};
    const String help() override;
    String parse(const String &command, String value) override;
};
extern espI2C webI2C0;
extern espI2C webI2C1;


// ESP32 has 3 SPI interfaces, one reserved for Flash memory
class espSPI : public espRoot
{
private:
    SPIClass bus;
    SPISettings spiConfig; // frequency, bit order and SPI mode
    String setFrequency(String &Hz);
    String setBitOrder(const String &bitOrder);
    String setSPImode(const String &mode);
    int sckPin  = -1; // default pin
    int misoPin = -1; // default pin
    int mosiPin = -1; // default pin
    int ssPin   = -1; // default pin
    String setPins(String &args);
    String begin(const String &args);
    String end();
    String write(String &hexArgs); // write only (ignore input)
    String writeread(String &hexArgs); // write and return input 
    String read(const String &numBytes); // read only (writes 0)
public:
    explicit espSPI(uint8_t busNum) : bus(busNum), spiConfig(100000, MSBFIRST, SPI_MODE0) {};
    const String help() final;
    String parse(const String &command, String value) final;
};
// extern espSPI webFSPI; // SPI1 attached to Flash memory, do not use
extern espSPI webHSPI; // SPI2 default pins clk=14, miso=12, mosi=13, ss=15
extern espSPI webVSPI; // SPI3 default pins clk=18, miso=19, mosi=23, ss=5

#endif
