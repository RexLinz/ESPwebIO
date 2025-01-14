#include "webIO.h"

// helping functions which might be used by several parsers

// get the next int value from comma separated list, 
// remove that from list
int espRoot::nextInt(String &list, const String delim)
{
    int val = list.toInt();
    int n = list.indexOf(delim);
    if (n>=0)
        list = list.substring(n+1);
    else // last one has been processed
        list = "";
    return val;
}

// get the next int value from comma separated list of hexadecimal values, 
// remove that from list
int espRoot::nextHex(String &list, const String delim)
{
    int val = strtol(list.c_str(), 0, 16);
    int n = list.indexOf(delim);
    if (n>=0)
        list = list.substring(n+1);
    else // last one has been processed
        list = "";
    return val;
}

// get the next float value from comma separated list, 
// remove that from list
float espRoot::nextFloat(String &list, const String delim)
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
String espRoot::nextString(String &list, const String delim)
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

void espRoot::addResponse(String &message, const String &response, const String &separator)
{
    if (response.length() > 0)
    {
        if (message.length() > 0) // there is already text
            message += separator; // add separator before
        message += response;
    }
}

String espRoot::JSONline(const String &command, const String &result)
{
    // complete result as JSON line
    String temp;
    if (result.length()==0)
        temp = "\"no output\""; // no output generated
    else if (result.indexOf("\",\"") > 0)
        temp = "[" + result + "]"; // output is array of strings
    else if (!result.startsWith("\"") && (result.indexOf(",") > 0))
        temp =  "[" + result + "]"; // output is array of numbers (or booleans)
    else
        temp = result;
    return "\"" + command + "\":" + temp;
}

const String espRoot::help() 
{
    return 
        WEBIO_VERSION
        "Available subsystems\r\n"
        "  /status ... returning system status\r\n"
        "  /GPIO\r\n"
        "  /Serial = /Serial0, /Serial1, /Serial2\r\n"
        "  /DAC1, /DAC2\r\n"
        "  /ADC\r\n"
        "  /PWM\r\n"
        "  /I2C = /I2C0, /I2C1\r\n"
        "  /SPI = /HSPI, /VSPI\r\n"
        "Requesting any subsystem without further parameters will return subsystem's help.\r\n"
        "More documentation available at https://github.com/RexLinz/ESPwebIO";
}

const String espRoot::status()
{
    String message = 
        WEBIO_VERSION
        "Free heap " + String(ESP.getFreeHeap()) + " bytes, largest block " +String(ESP.getMaxAllocHeap()) + " bytes\r\n";
    return message;
}

espRoot webRoot;

// ESP32 has one set of GPIO
espGPIO webGPIO;

// ESP32 has 3 async serial interfaces 
espSerial webSerial0(Serial);  // pins TX=GPIO0, RX=GPIO1 - connected to USB
espSerial webSerial1(Serial1); // not available on Dev Kit C without remapping pins
espSerial webSerial2(Serial2); // pins TX=GPIO17, RX=GPIO16

// ESP32 has two DAC channels available at gpio pins 25 and 26
espDAC webDAC1(PIN_DAC1); // DAC1 at pin 25
espDAC webDAC2(PIN_DAC2); // DAC2 at pin 26

// Arduino like interface does not distinguish ADC1 and ADC2
espADC webADC;

// ESP32 has two I2C controllers
espI2C webI2C0(Wire);
espI2C webI2C1(Wire1);

// ESP32 has one LEDPWM controller (total of 16 outputs)
espPWM webPWM;

// ESP32 has three SPI interfaces
// espSPI webFSPI(FSPI); // SPI1 attached to flash memory, do not use
espSPI webHSPI(HSPI); // SPI2 attached to clk=14, miso=12, mosi=13, ss=15
espSPI webVSPI(VSPI); // SPI3 attached to clk=18, miso=19, mosi=23, ss=5
