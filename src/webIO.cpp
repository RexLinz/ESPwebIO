#include "webIO.h"

// helping functions which might be used by several parsers

// get the next int value from comma separated list, 
// remove that from list
int espUtil::nextInt(String &list)
{
    int val = list.toInt();
    int n = list.indexOf(',');
    if (n>=0)
        list = list.substring(n+1);
    else
        list = "";
    return val;
}

// get the next float value from comma separated list, 
// remove that from list
float espUtil::nextFloat(String &list)
{
    float val = list.toFloat();
    int n = list.indexOf(',');
    if (n>=0)
        list = list.substring(n+1);
    else
        list = "";
    return val;
}


// ESP32 has one set of GPIO
espGPIO webGPIO;

// ESP32 has 3 async serial interfaces 
espSerial webSerial0(Serial);  // pins TX=GPIO0, RX=GPIO1 - connected to USB
espSerial webSerial1(Serial1); // not available on Dev Kit C without remapping pins
espSerial webSerial2(Serial2); // pins TX=GPIO17, RX=GPIO16

// ESP32 has two DAC channels available at gpio pins 25 and 26
espDAC webDAC1(25);
espDAC webDAC2(26);

