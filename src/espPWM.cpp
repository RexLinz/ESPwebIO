#include <soc/ledc_struct.h>
#include "webIO.h"

const String espPWM::help()
{
    return
        "Help on PWM subsystem (using ESP's led controller)\r\n"
        "general setup (initialize first, all channels using same value)\r\n"
        "  frequency=Hz ... set pulse frequency (integer, default 50)\r\n"
        "  resolution=bits ... set resolution (default 16 bits)\r\n"
        "pin mapping and output\r\n"
        "  channels=number,... commands below work with channel list specified\r\n"
        "  map=pin,... map channels to GPIO pins\r\n"
        "  width=microseconds,... set pulse width in microseconds\r\n"
        "  duty=value,... duty (0.0 to 1.0)\r\n"
        "  value=int,... value in integer range 0...(2^resolution)-1\r\n"
        "  stop=channel,... stop and disable output of channel (all)";
}

espPWM::espPWM()
{
    for (int i=0; i<NUM_CHANNELS; i++)
        _pins[i] = -1; // not attached
}

uint32_t espPWM::maxVal(uint8_t channel)
{
    return (1UL<<(LEDC.timer_group[channel/8].timer[(channel/2)%4].conf.duty_resolution)) -1;
}

uint32_t espPWM::clipVal(uint8_t channel, uint32_t val)
{
    int32_t max = maxVal(channel);
    if (val > max)
        return max;
    else if (val<0)
        return 0;
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

String espPWM::setChannels(String channelList)
{
    String result = "";
    while (channelList.length()>0)
    {
        int channel = nextInt(channelList);
        if ((channel<0) || (channel >= NUM_CHANNELS))
            addResponse(result, "\"invalid\"", ",");
        else
            addResponse(result, String(channel), ",");
    }
    if (result.indexOf("invalid") == -1)
        _channelList = result;
    else
        _channelList = "";
    return result;
}

String espPWM::initChannel(uint8_t channel, uint8_t pin)
{
  if (LEDC.channel_group[channel/8].channel[channel%8].conf0.sig_out_en)
    return "\"channel in use\"";
  if (!digitalPinCanOutput(pin)) // output available on pin?
    return "\"invalid GPIO pin\"";
  ledcSetup(channel, _frequency, _bits);
  ledcAttachPin(pin, channel);
  ledcWrite(channel, 0);
  _pins[channel] = pin;
  return String(pin);
}

String espPWM::map(String channelList, String pinList)
{
    String result = "";
    while ((channelList.length()>0) && (pinList.length()>0))
    {
        uint8_t channel = nextInt(channelList);
        uint8_t pin = nextInt(pinList);
        String temp = initChannel(channel, pin);
        // assemble result string
        addResponse(result, temp, ",");
    }
    return result;
}

String espPWM::stop(String channelList, String userList)
{
    String result = "";
    if (userList.length() > 0)
        channelList = userList;
    while (channelList.length() > 0)
    {
        uint8_t channel = nextInt(channelList);
        String temp;
        LEDC.channel_group[channel/8].channel[channel%8].conf0.sig_out_en = 0; // stop output
        ledcDetachPin(_pins[channel]);
        _pins[channel] = -1;
        // assemble result string
        addResponse(result, String(channel), ",");
    }
    return result;
}

String espPWM::width(String channelList, String valueList)
{
    String result = "";
    if (valueList.indexOf(',') < 0) // list or just a single value?
    { // just single configuration for all channels
        float microseconds = nextFloat(valueList);
        while (channelList.length()>0)
        {
            uint8_t channel = nextInt(channelList);
            float clkMHz= (LEDC.timer_group[channel/8].timer[(channel/2)%4].conf.tick_sel) ? 80.0f : 1.0f;
            uint32_t clkDiv = LEDC.timer_group[channel/8].timer[(channel/2)%4].conf.clock_divider;
            float clkVal = clkMHz/clkDiv*microseconds*256; // has 8 fractional bits
            ledcWrite(channel, clipVal(channel, clkVal));
            addResponse(result, String(microseconds), ",");
        }
    }
    else     
    { // individual configurations
        while ((channelList.length()>0) && (valueList.length()>0))
        {
            uint8_t channel = nextInt(channelList);
            float clkMHz= (LEDC.timer_group[channel/8].timer[(channel/2)%4].conf.tick_sel) ? 80.0f : 1.0f;
            uint32_t clkDiv = LEDC.timer_group[channel/8].timer[(channel/2)%4].conf.clock_divider;
            float microseconds = nextFloat(valueList);
            float clkVal = clkMHz/clkDiv*microseconds*256; // has 8 fractional bits
            ledcWrite(channel, clipVal(channel, clkVal));
            addResponse(result, String(microseconds), ",");
        }
    }
    return result;
}

String espPWM::duty(String channelList, String valueList) // range 0.0 ... 1.0
{
    String result = "";
    // split numberList into numbers separated by comma
    // assemble returned strings to JSON like array
    if (valueList.indexOf(',') < 0) // list or just a single value?
    { // just single configuration for all channels
        float dutyVal = nextFloat(valueList);
        while (channelList.length()>0)
        {
            uint8_t channel = nextInt(channelList);
            uint32_t maxval = maxVal(channel);
            ledcWrite(channel, clipVal(channel, dutyVal*maxval));
            addResponse(result, String(dutyVal), ",");
        }
    }
    else     
    { // individual configurations
        while ((channelList.length()>0) && (valueList.length()>0))
        {
            uint8_t channel = nextInt(channelList);
            float dutyVal = nextFloat(valueList);
            uint32_t maxval = maxVal(channel);
            ledcWrite(channel, clipVal(channel, dutyVal*maxval));
            addResponse(result, String(dutyVal), ",");
        }
    }
    return result;
}

String espPWM::val(String channelList, String valueList) // integers
{
    String result = "";
    if (valueList.indexOf(',') < 0) // list or just a single value?
    { // just single configuration for all channels
        int32_t value = nextInt(valueList);
        while (channelList.length()>0)
        {
            uint8_t channel = nextInt(channelList);
            ledcWrite(channel, clipVal(channel, value));
            addResponse(result, String(value), ",");
        }
    }
    else
    { // individual configurations
        while ((channelList.length()>0) && (valueList.length()>0))
        {
            uint8_t channel = nextInt(channelList);
            int32_t value = nextInt(valueList);
            ledcWrite(channel, clipVal(channel, value));
            addResponse(result, String(value), ",");
        }
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
    else if (command == "channels")
        result = setChannels(value);
    else if (command == "map")
        result = map(_channelList, value); // scan list of channel/value pairs
    else if (command == "stop")
        result = stop(_channelList, value); // useing value if specified, else _channelList
    else if (command == "width")
        result = width(_channelList, value); // scan list of channel/value pairs
    else if (command == "duty")
        result = duty(_channelList, value); // scan list of channel/value pairs
    else if (command == "val")
        result = val(_channelList, value);
    else
        result = "\"invalid keyword\"";
    return result;
}
