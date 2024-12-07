#include "webIO.h"

String espDAC::help()
{
    static const String helptext = 
        "Help on DAC subsystem\r\n"
        "\nconfiguration for scaled output using raw==round(value*scale+offset)\r\n"
        "  scale=float ... set scaling (default 1.0)\r\n"
        "  offset=float ... set offset (default 0.0)\r\n"
        "  disable ... disable DAC function on this channel (pin)\r\n"
        "\nwriting output\r\n"
        "  raw=uint_8 ... write out as is\r\n"
        "  value=float ... write out scaled round(float*scale+offset)\r\n";
    return helptext;
}

void espDAC::raw(uint8_t value)
{
    dacWrite(dacPin, value);
}

String espDAC::parse(String command, String value)
{
    // _serial.println(command + ":" + value);
    String result = "";
    if (command == "raw")
        raw(value.toInt());
    else if (command == "offset")
        setOffset(value.toInt());
    else if (command == "scale")
        setScale(value.toFloat());
    else if (command == "disable")
        disable();
    else if (command == "value")
        setValue(value.toFloat());
    else
        result = "invalid keyword " + command;
    return result;
}
