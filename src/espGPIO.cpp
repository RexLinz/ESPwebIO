#include "webIO.h"

espGPIO::espGPIO()
{
    for (int i=0; i<NUM_OUPUT_PINS; i++)
        savedPinMode[i] = INPUT; // after reset we assume it is input
}

String espGPIO::help()
{
    static const String helptext = 
        "Help on GPIO subsystem\r\n"
        "\ndigital IO\r\n"
        "  set/clear/toggle=pins ... new digital pin state\r\n"
        "  state=pins ... query digital pin state\r\n"
        "\nconfiguration\r\n"
        "  input/inputPullUp/inputPullDown=pins ... digital input pin modes\r\n"
        "  output/outputOpenDrain=pins ... digital output pin modes\r\n"
        "  analog=pins ... analog input mode\r\n"
        "  mode=pins ... query pin mode\r\n"
        "\npins is a list of comma separated GPIO numbers, e.g.\r\n"
        "  http://webIO/GPIO/state=22,23 reading state of GPIO pins 22 and 23\r\n"
        "\ncommands writing data return 1 as result if successful, 0 on error";
    return helptext;
}

String espGPIO::set(uint8_t pin)
{
    if (digitalPinCanOutput(pin))
    {
        digitalWrite(pin, 1);
        return "1"; // OK
    }
    return "0"; // not executed
}

String espGPIO::clear(uint8_t pin)
{
    if (digitalPinCanOutput(pin))
    {
        digitalWrite(pin, 0);
        return "1"; // OK
    }
    return "0"; // not executed
}

String espGPIO::toggle(uint8_t pin)
{
    if (digitalPinCanOutput(pin))    
    {
        if (digitalRead(pin))
            digitalWrite(pin, 0);
        else
            digitalWrite(pin, 1);
        return "1"; // OK
    }
    return "0"; // not executed
}

String espGPIO::state(uint8_t pin)
{
    if (digitalPinIsValid(pin))
        return digitalRead(pin) ? "1" : "0";
    else
        return "?"; // will force json error
}

String espGPIO::modeOutput(uint8_t pin)
{
    if (digitalPinCanOutput(pin))    
    {
        pinMode(pin, OUTPUT);
        savedPinMode[pin] = OUTPUT;
        return "1"; // OK
    }
    return "0"; // not executed
}

String espGPIO::modeOutputOpenDrain(uint8_t pin)
{
    if (digitalPinCanOutput(pin))
    {
        pinMode(pin, OUTPUT_OPEN_DRAIN);
        savedPinMode[pin] = OUTPUT_OPEN_DRAIN;
        return "1"; // OK
    }
    return "0"; // not executed
}

String espGPIO::modeInput(uint8_t pin)
{
    if (digitalPinIsValid(pin))
    {
        pinMode(pin, INPUT);
        savedPinMode[pin] = INPUT;
        return "1"; // OK
    }
    return "0"; // not executed
}

String espGPIO::modeInputPullUp(uint8_t pin)
{
    if (digitalPinIsValid(pin))
    {
        pinMode(pin, INPUT_PULLUP);
        savedPinMode[pin] = INPUT_PULLUP;
        return "1"; // OK
    }
    return "0"; // not executed
}

String espGPIO::modeInputPullDown(uint8_t pin)
{
    if (digitalPinIsValid(pin))
    {
        pinMode(pin, INPUT_PULLDOWN);
        savedPinMode[pin] = INPUT_PULLDOWN;
        return "1"; // OK
    }
    return "0"; // not executed
}

String espGPIO::modeAnalog(uint8_t pin)
{
    if (digitalPinIsValid(pin))
    {
        pinMode(pin, ANALOG);
        savedPinMode[pin] = ANALOG;
        return "1"; // OK
    }
    return "0"; // not executed
}

String espGPIO::mode(uint8_t pin) // read pin mode
{
    String result;
    if (digitalPinIsValid(pin)) 
    {
        switch (savedPinMode[pin])
        {
            case INPUT: result = "input"; break;
            case INPUT_PULLUP: result = "inputPullUp"; break;
            case INPUT_PULLDOWN: result = "inputPullDown"; break;
            case OUTPUT: result = "output"; break;
            case OUTPUT_OPEN_DRAIN: result = "outputOpenDrain"; break;
            case ANALOG: result = "analog"; break;
            default: result = "unkown";
        }
    }
    else
        result = "invalidPin";
    return "\"" + result + "\"";
}

String espGPIO::parse(String command, String numberList)
{
    String result = "";
    // split pinList into numbers separated by comma
    // for each number call function()
    // assemble returned strings to JSON like array
    // TODO add remaining pin modes
    while (numberList.length() > 0)
    {
        uint8_t pin = nextInt(numberList);
        String temp;
        if (command == "set")
            temp = set(pin);
        else if (command == "clear")
            temp = clear(pin);
        else if (command == "toggle")
            temp = toggle(pin);
        else if (command == "mode")
            temp = mode(pin);
        else if (command == "state")
            temp = state(pin);
        else if (command == "output")
            temp = modeOutput(pin);
        else if (command == "outputOpenDrain")
            temp = modeOutputOpenDrain(pin);
        else if (command == "input")
            temp = modeInput(pin);
        else if (command == "inputPullUp")
            temp = modeInputPullUp(pin);
        else if (command == "inputPullDown")
            temp = modeInputPullDown(pin);
        else if (command == "analog")
            temp = modeAnalog(pin);
        else
            temp = "invalid keyword " + command;
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
