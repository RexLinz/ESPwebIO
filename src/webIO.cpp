#include "webIO.h"

// helping functions which might be used by several parsers

// get the next int value from comma separated list, 
// remove that from list
int espUtil::nextInt(String &list, String delim)
{
    int val = list.toInt();
    int n = list.indexOf(delim);
    if (n>=0)
        list = list.substring(n+1);
    else // last one has been processed
        list = "";
    return val;
}

// get the next float value from comma separated list, 
// remove that from list
float espUtil::nextFloat(String &list, String delim)
{
    float val = list.toFloat();
    int n = list.indexOf(delim);
    if (n>=0)
        list = list.substring(n+1);
    else // last one has been processed
        list = "";
    return val;
}

// get the next float value from comma separated list, 
// remove that from list
String espUtil::nextString(String &list, String delim)
{
    int n = list.indexOf(delim);
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

const String espUtil::help()
{
    String message = 
        WEBIO_VERSION
        "available subsystems\r\n"
        "  /status ... returning system status\r\n"
        "  /GPIO\r\n"
        "  /Serial = /Serial0, /Serial1, Serial2\r\n"
        "  /DAC1, /DAC2\r\n"
        "  /ADC\r\n"
        "requesting any subsystem without further parameters will return subsystem's help\r\n";
    return message;
}

String espUtil::status()
{
    String message = 
        WEBIO_VERSION
        "Free heap " + String(ESP.getFreeHeap()) + " bytes, largest block " +String(ESP.getMaxAllocHeap()) + " bytes\r\n";
    return message;
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
