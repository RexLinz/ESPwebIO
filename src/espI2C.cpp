#include "webIO.h"

const String espI2C::help()
{
    return 
        "Help on I2C subsystem /I2C0 and /I2C1\r\n"
        "\ngeneral settings\r\n"
        "  pins=scl,sda ... set pins to use\r\n"
        "  freqency=Hz ... set frequency (default 100 kHz, max 400 kHz)\r\n"
        "  begin ... open interface\r\n"
        "  scan ... scan bus and return address(es) of device(s) found\r\n"
        "           last one found will be set as default address\r\n"
        "  end ... end interface\r\n"
        "\nsending and receiving data\r\n"
        "  address=hex ... set read/write address\r\n"
        "  write=hex,... write out to slave, return number of bytes done\r\n"
        "  read=num ... read num bytes, return as hex values";
}

String espI2C::setPins(String args)
{
    if (args.length()==0)
        sdaPin = -1;
    else 
        sdaPin = nextInt(args);
    if (args.length()==0)
        sclPin = -1;
    else
        sclPin = nextInt(args);
    return String(sdaPin) + "," + String(sclPin);
}

String espI2C::setFrequency(String args)
{
    if (args.length()==0)
        frequency = 0;
    else 
        frequency = nextInt(args);
    return String(frequency);
}

String espI2C::begin()
{
    if (bus.begin(sdaPin, sclPin, frequency))
        return "\"Ok\"";
    else
        return "\"Error\""; 
}

String espI2C::end()
{
    bus.end();
    return "\"done\"";
}

String espI2C::setAddress(String hexAddress)
{
    if (hexAddress.length()==0)
        return "\"missing\"";
    else 
        address = nextHex(hexAddress);
    return "\"" + String(address, HEX) + "\"";
}

String espI2C::write(String hexArgs)
{
    bus.beginTransmission(address); // set read or write?
    int bytesDone = 0;
    while (hexArgs.length() > 0)
    {
        uint8_t n = nextHex(hexArgs);
        if (bus.write(n))
            bytesDone++;
        else
            break;
    }
    bus.endTransmission();
    return String(bytesDone);
}

String espI2C::read(String numBytes)
{
    String response = "";
    int bytes = numBytes.toInt(); // 
    bus.requestFrom(address, bytes);
    while (bus.available())
    {
        int b = bus.read();
        addResponse(response, String(b,HEX), ",");
    }
    return "\""+response+"\""; // as string, values are hexadecimal!
}

String espI2C::scan()
{
    String response = "";
    for (int a = 0; a<128; a++)
    {
        bus.beginTransmission(a);
        if (bus.endTransmission() == 0)
        {
            addResponse(response, String(a, HEX), ",");
            address = a; // remember last one found
        }
    }
    if (response.length() == 0)
        response = "\"no devices found\"";
    return "\"" + response + "\""; // as string, values are hexadecimal
}

String espI2C::parse(String command, String value)
{
    String result = "";
    if (command == "pins")
        result = setPins(value); // save for use by following commands
    else if (command == "frequency")
        result = setFrequency(value);
    else if (command == "begin")
        result = begin();
    else if (command == "end")
        result = end();
    else if (command == "address")
        result = setAddress(value);
    else if (command == "write")
        result = write(value);
    else if (command == "read")
        result = read(value); 
    else if (command == "scan")
        result = scan(); 
    else
        result = "\"invalid keyword\"";
    // complete result as JSON
    if (result.length()==0)
        return ""; // no output generated
    if (result.indexOf(",") > 0)
        result =  "[" + result + "]"; // output is array
    return "\"" + command + "\":" + result;
}

espI2C webI2C0(Wire);
espI2C webI2C1(Wire1);
