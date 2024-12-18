#include "webIO.h"

String espADC::help()
{
    static const String helptext = 
        "Help on ADC subsystem\r\n"
        "  attenuation, oversampling, offset and scale apply to a list of pins to be configured before.\r\n"
        "  For these commands a list of values or a single value (to be applied to all) could be specified, e.g.\r\n"
        "    pins=36,39&attenuation=2.5dB,6dB ... set attenuation on input 36 to 2.5dB, input 39 to 6dB\r\n"
        "    pins=36,39&scale=2.5 ... set scale on inputs 36 and 39 to 2.0\r\n"
        "  pins=pinlist ... set list of pins, separated by commas\r\n"
        "\ngeneral settings\r\n"
        "  attenuation=0dB/2.5dB/6dB/11dB (FSR about 1.0/1.35/1.9/3.3 V)\r\n"
        "  oversampling=n ... sum n readings per call\r\n"
        "\nconfiguration for reading scaled output\r\n"
        "  offset=float ... set offset (default 0.0)\r\n"
        "  scale=float ... set scaling (default 1.0)\r\n"
        "\nreading input\r\n"
        "  raw=pins ... read as is (uint)\r\n"
        "  value=pins ... read scaled value=(raw-offset)*scale (float)\r\n";
    return helptext;
}

espADC::espADC(adc_attenuation_t att)
{
    analogSetAttenuation(att);
    for (int n=0; n<SOC_GPIO_PIN_COUNT; n++)
    {
        oversampling[n] = 1;
        offset[n] = 0.0f;
        scale[n] = 1.0f;
    }
}

/*
void espADC::setAttenuation(String a)
{
    analogSetAttenuation(attenuationValue(a));
}

void espADC::setAttenuation(uint8_t pin, String a)
{
    analogSetPinAttenuation(pin, attenuationValue(a));
}
*/

adc_attenuation_t espADC::attenuationValue(String a)
{
    adc_attenuation_t temp;
    if (a == "0dB")
        temp = ADC_0db;   // FSR about 1.05 V
    else if (a == "2.5dB")
        temp = ADC_2_5db; // FSR about 1.35 V
    else if (a == "6dB")
        temp = ADC_6db;   // FSR about 1.9 V, best S/N 
    else
        temp = ADC_11db;  // FSR about 3.3 V, nonlinear above 2.5 V
    return temp;
}

// scan the list of pins and set attenuation to requested value for all of them
void espADC::setAttenuation(String pinList, String valueList)
{ 
    if (valueList.indexOf(',') < 0) // list or just a single value?
    { // just single configuration for all channels
        adc_attenuation_t value = attenuationValue(valueList);
        while (pinList.length() > 0)
        { // process list of pins
            uint8_t pin = nextInt(pinList);
            analogSetPinAttenuation(pin, value);
        }
    }
    else 
    { // individual configurations
        while ((pinList.length() > 0) && (valueList.length() > 0))
        { // process as long as we have pin/attenuation pairs
            uint8_t pin = nextInt(pinList);
            adc_attenuation_t value = attenuationValue(nextString(valueList));
            analogSetPinAttenuation(pin, value);
        }
    }
}

void espADC::setOversampling(String pinList, String valueList)
{
    if (valueList.indexOf(',') < 0) // list or just a single value?
    { // just single configuration for all channels
        uint16_t value = valueList.toInt();
        while (pinList.length() > 0)
        { // process list of pins
            uint8_t pin = nextInt(pinList);
            oversampling[pin] = value;
        }
    }
    else 
    { // individual configurations
        while ((pinList.length() > 0) && (valueList.length() > 0))
        { // process as long as we have pin/attenuation pairs
            uint8_t pin = nextInt(pinList);
            float value = nextString(valueList).toFloat();
            oversampling[pin] = value;
        }
    }
}

void espADC::setOffset(String pinList, String valueList)
{
    if (valueList.indexOf(',') < 0) // list or just a single value?
    { // just single configuration for all channels
        float value = valueList.toFloat();
        while (pinList.length() > 0)
        { // process list of pins
            uint8_t pin = nextInt(pinList);
            offset[pin] = value;
        }
    }
    else 
    { // individual configurations
        while ((pinList.length() > 0) && (valueList.length() > 0))
        { // process as long as we have pin/attenuation pairs
            uint8_t pin = nextInt(pinList);
            float value = nextString(valueList).toFloat();
            offset[pin] = value;
        }
    }
}

void espADC::setScale(String pinList, String valueList)
{
    if (valueList.indexOf(',') < 0) // list or just a single value?
    { // just single configuration for all channels
        float value = valueList.toFloat();
        while (pinList.length() > 0)
        { // process list of pins
            uint8_t pin = nextInt(pinList);
            scale[pin] = value;
        }
    }
    else 
    { // individual configurations
        while ((pinList.length() > 0) && (valueList.length() > 0))
        { // process as long as we have pin/attenuation pairs
            uint8_t pin = nextInt(pinList);
            float value = nextString(valueList).toFloat();
            scale[pin] = value;
        }
    }
}


uint32_t espADC::getRaw(uint8_t pin)
{
//    analogSetAttenuation(ADC_6db);
    uint32_t temp = 0;
    int N = oversampling[pin];
    for (int n=N; n>0; n--)
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
    if (command == "pins")
        pins = value; // save for use by following commands
    else if (command == "attenuation")
        setAttenuation(pins, value);
    else if (command == "oversampling")
        setOversampling(pins, value);
    else if (command == "offset")
        setOffset(pins, value);
    else if (command == "scale")
        setScale(pins, value);
    else if (command == "raw")
        result = parseList(command, value); 
    else if (command == "value")
        result = parseList(command, value);
    else
        result = "invalid keyword " + command;
    return result;
}
