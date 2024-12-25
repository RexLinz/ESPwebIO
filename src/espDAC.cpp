#include "webIO.h"

// arduino like interface does not distinguish ADC1 and ADC2
espADC webADC;

const String espDAC::help()
{
    return
        "Help on DAC subsystem\r\n"
        "\nconfiguration for scaled output using raw=round(value*scale+offset)\r\n"
        "  scale=float ... set scaling (default 1.0)\r\n"
        "  offset=float ... set offset (default 0.0)\r\n"
        "  disable ... disable DAC function on this channel (pin)\r\n"
        "\nwriting output\r\n"
        "  raw=uint_8 ... write out as is\r\n"
        "  value=float ... write out scaled round(float*scale+offset)\r\n";
}

String espDAC::setRaw(int value)
{
    if (value<0)
        value=0;
    else if (value>255)
        value=255;
    dacWrite(dacPin, value);
    return String(value);
}

String espDAC::setValue(float value)
{
    int intVal = round(value*_scale + _offset);
    return setRaw(intVal);
}

String espDAC::parse(String command, String value)
{
    String result = "";
    if (command == "raw")
        result = setRaw(value.toInt());
    else if (command == "offset")
        result = setOffset(value.toInt());
    else if (command == "scale")
        result = setScale(value.toFloat());
    else if (command == "disable")
        result = disable();
    else if (command == "value")
        result = setValue(value.toFloat());
    else
        result = "invalid keyword";
    // complete result as JSON
    if (result.length()==0)
        return ""; // no output generated
    if (result.indexOf(",") > 0)
        result =  "[" + result + "]"; // output is array
    return "\"" + command + "\":" + result;
}
