% test of scaled DAC output

% server web address or host name
ADC  = "http://WebIO/ADC";
DAC1 = "http://WebIO/DAC1"; % DAC1 on GPIO pin 25
DAC2 = "http://WebIO/DAC2"; % DAC2 on GPIO pin 26

webwrite(DAC1, "raw", "128");
webwrite(DAC2, "raw", "128");

webwrite(ADC,
  "pins", "34,35",
  "attenuation", "11dB", % both channels
  "scale", "0.5,0.25", % pin 35 lower scale
  "oversampling", "16,32" % but higher oversampling (should compensate)
);

disp("scaled ADC input running");
disp("  press any key to stop");
do
  webread(ADC, "value", "34,35")
  pause(1);
until length(kbhit(1)) > 0 % end on any keystroke
disp("  stopped");

webwrite(DAC1, "disable", "");
webwrite(DAC2, "disable", "");

