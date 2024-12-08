#include "webIO.h"

String espADC::help()
{
    static const String helptext = 
        "Help on ADC subsystem\r\n"
        "\ngeneral settings\r\n"
        "  attenuation=0dB/2.5dB/6dB/11dB (FSR about 1.0/1.35/1.9/3.3 V)\r\n"
        "  oversampling=n ... sum n readings per call\r\n"
        "\nconfiguration for reading scaled output\r\n"
        "  scale=float ... set scaling (default 1.0)\r\n"
        "  offset=float ... set offset (default 0.0)\r\n"
        "\nreading input\r\n"
        "  raw=pins ... read as is (uint)\r\n"
        "  value=pins ... read scaled value=(raw-offset)*scale (float)\r\n";
    return helptext;
}

espADC::espADC()
{
    analogSetAttenuation(attenuation);
}

void espADC::setAttenuation(String a)
{
    if (a == "0dB")
        attenuation = ADC_0db;   // FSR about 1.05 V
    else if (a == "2.5dB")
        attenuation = ADC_2_5db; // FSR about 1.35 V
    else if (a == "6dB")
        attenuation = ADC_6db;   // FSR about 1.9 V, best S/N 
    else
        attenuation = ADC_11db;  // FSR about 3.3 V, nonlinear above 2.5 V
// TODO for setting per pin use
// analogSetPinAttenuation(pin, attenuation);
    analogSetAttenuation(attenuation);
}

uint32_t espADC::getRaw(uint8_t pin)
{
//    analogSetAttenuation(ADC_6db);
    uint32_t temp = 0;
    for (int n=oversampling; n>0; n--)
        temp += analogRead(pin);
    return temp;
}

String espADC::parseList(String command, String numberList)
{
    String result = "";
    // split pinList into numbers separated by comma
    // for each number call function()
    // assemble returned strings to JSON like array
    while (numberList.length() > 0)
    {
        uint8_t pin = nextInt(numberList);
        String temp;
        if (command == "raw")
            temp = String(getRaw(pin));
        else if (command == "value")
            temp = String(getValue(pin));
        // assemble result string
        if (result.length() > 0)
            result += ","; // item separator
        result += temp; // function result
    }
    if (result.length()==0)
        return ""; // no output generated
    if (result.indexOf(",") > 0)
        result =  "[" + result + "]"; // output is array
    return "\"" + command + "\":" + result;
}

String espADC::parse(String command, String value)
{
    // _serial.println(command + ":" + value);
    String result = "";
    if (command == "attenuation")
        setAttenuation(value);
    else if (command == "oversampling")
        setOversampling(value.toInt());
    else if (command == "offset")
        setOffset(value.toFloat());
    else if (command == "scale")
        setScale(value.toFloat());
    else if (command == "raw")
        result = parseList(command, value); 
    else if (command == "value")
        result = parseList(command, value);
    else
        result = "invalid keyword " + command;
    return result;
}
