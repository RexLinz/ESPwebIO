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
    else // last one has been processed
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
    else // last one has been processed
        list = "";
    return val;
}

// get the next float value from comma separated list, 
// remove that from list
String espUtil::nextString(String &list)
{
    int n = list.indexOf(',');
    String val = "";
    if (n>=0)
    {
        val =  list.substring(0,n);
        list = list.substring(n+1);
    }
    else
    { // last one being processed
        val  = list;
        list = "";
    }
    return val;
}

// ESP32 has one set of GPIO
espGPIO webGPIO;

// ESP32 has 3 async serial interfaces 
espSerial webSerial0(Serial);  // pins TX=GPIO0, RX=GPIO1 - connected to USB
espSerial webSerial1(Serial1); // not available on Dev Kit C without remapping pins
espSerial webSerial2(Serial2); // pins TX=GPIO17, RX=GPIO16

// ESP32 has two DAC channels available at gpio pins 25 and 26
espDAC webDAC1(PIN_DAC1); // DAC1 at pin 25
espDAC webDAC2(PIN_DAC2); // DAC2 at pin 26
