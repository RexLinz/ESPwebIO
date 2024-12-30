#include "soc/ledc_struct.h"
#include "webIO.h"

const String espPWM::help()
{
    return
        "Help on PWM subsystem (using ESP's led controller)\r\n"
        "general setup (initialize first, all channels using same value)\r\n"
        "  frequency=freq ... set pulse frequency\r\n"
        "  resolution=bits ... set resolution\r\n"
        "pin mapping and output (accepting sequence of channel/value pairs)\r\n"
        "  map=channel,pin,... map channel (0..15) to GPIO pin\r\n"
        "  width=channel,microseconds,... set pulse width in microseconds\r\n"
        "  duty=channel,value,... duty (0.0 to 1.0)\r\n"
        "  stop=channel,... stop and disable output of channel";
}

espPWM::espPWM()
{
    for (int i=0; i<NUM_CHANNELS; i++)
        _pins[i] = -1; // not attached
}

uint32_t espPWM::maxDutyVal(uint8_t channel)
{
    return (16UL<<(LEDC.timer_group[channel/8].timer[(channel/2)%4].conf.duty_resolution)) -1;
}

uint32_t espPWM::clipDuty(uint8_t channel, uint32_t val)
{
    uint32_t maxval = maxDutyVal(channel);
    if (val>maxval)
        return maxval;
    else
        return val;
}

String espPWM::setFrequency(String freq)
{
    _frequency = freq.toInt();
    return freq; // TODO check range?
}

String espPWM::setResolution(String bits)
{
    _bits = bits.toInt();
    return bits; // TODO check range?
}

String espPWM::initChannel(uint8_t channel, uint8_t pin)
{
  if (channel >= NUM_CHANNELS)
    return "\"only supporting channels 0-15\"";
  if (LEDC.channel_group[channel/8].channel[channel%8].conf0.sig_out_en)
    return "\"channel in use\"";
  if (!digitalPinCanOutput(pin)) // output available on pin?
    return "\"invalid GPIO pin\"";
  double freq = ledcSetup(channel, _frequency, _bits); // actual frequency
  ledcAttachPin(pin, channel);
  ledcWrite(channel, 0);
  _pins[channel] = pin;
  return "\"Ok\"";
}

String espPWM::mapChannels(String args)
{
    String result = "";
    // split numberList into numbers separated by comma
    // assemble returned strings to JSON like array
    while (args.length() > 0)
    {
        uint8_t channel = nextInt(args);
        uint8_t pin = nextInt(args);
        String temp = initChannel(channel, pin);
        // assemble result string
        addResponse(result, temp, ",");
    }
    return result;
}

String espPWM::stopChannels(String args)
{
    String result = "";
    // split numberList into numbers separated by comma
    // for each number call function()
    // assemble returned strings to JSON like array
    while (args.length() > 0)
    {
        uint8_t channel = nextInt(args);
        String temp;
        if (channel >= NUM_CHANNELS)
            temp = "\"only supporting channels 0-15\"";
        else 
        {
            LEDC.channel_group[channel/8].channel[channel%8].conf0.sig_out_en = 0; // stop output
            ledcDetachPin(_pins[channel]);
            _pins[channel] = -1;
            temp = "\"off\"";
        }
        // assemble result string
        addResponse(result, temp, ",");
    }
    return result;
}

String espPWM::setMicroSeconds(uint8_t channel, uint32_t microSeconds)
{
    uint32_t duty = (((uint32_t)microSeconds) << _bits) / (1000000UL/(_frequency<<4));
    ledcWrite(channel, clipDuty(channel, duty));
    return String(microSeconds);
}

String espPWM::setWidth(String args)
{
    String result = "";
    // split numberList into numbers separated by comma
    // assemble returned strings to JSON like array
    while (args.length() > 0)
    {
        uint8_t channel = nextInt(args);
        uint32_t microseconds = nextInt(args);
        String temp;
        if (channel >= NUM_CHANNELS) // only valid for channels 0-15
            temp = "\"only supporting channels 0-15\"";
        else 
            temp = setMicroSeconds(channel, microseconds);
        addResponse(result, temp, ",");
    }
    return result;
}

String espPWM::setDuty(String args) // range 0.0 ... 1.0
{
    String result = "";
    // split numberList into numbers separated by comma
    // assemble returned strings to JSON like array
    while (args.length() > 0)
    {
        uint8_t channel = nextInt(args);
        float duty = nextFloat(args);
        String temp;
        if (channel >= NUM_CHANNELS) // only valid for channels 0-15
            temp = "\"only supporting channels 0-15\"";
        else 
        {
            uint32_t maxval = maxDutyVal(channel);
            ledcWrite(channel, clipDuty(channel, duty*maxval));
            temp = String(duty);
        }
        addResponse(result, temp, ",");
    }
    return result;
}

String espPWM::parse(String command, String value)
{
    // _serial.println(command + ":" + value);
    String result = "";
    if (command == "frequency")
        result = setFrequency(value); // save for use by following commands
    else if (command == "resolution")
        result = setResolution(value);
    else if (command == "map")
        result = mapChannels(value); // scan list of channel/value pairs
    else if (command == "stop")
        result = stopChannels(value); // scan list of channel/value pairs
    else if (command == "width")
        result = setWidth(value); // scan list of channel/value pairs
    else if (command == "duty")
        result = setDuty(value); // scan list of channel/value pairs
    else
        result = "\"invalid keyword\"";
    // complete result as JSON
    if (result.length()==0)
        return ""; // no output generated
    if (result.indexOf(",") > 0)
        result =  "[" + result + "]"; // output is array
    return "\"" + command + "\":" + result;
}

espPWM webPWM;
