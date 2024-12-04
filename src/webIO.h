#ifndef _WEBIO_H_
#define _WEBIO_H_
#include <Arduino.h>

// JSON online tools for testing
// https://emn178.github.io/online-tools/json/formatter/ 

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

extern espGPIO webGPIO;

// TODO future classes to be implemented
/*
class espSerial
{
public:
};

class espADC
{

};

class espDAC
{
    
};

class espI2C
{

};

class espSPI
{

};

class espPWM
{

};
*/

#endif