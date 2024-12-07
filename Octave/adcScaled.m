% test of scaled DAC output

% server web address or host name
ADC  = "http://WebIO/ADC";
DAC1 = "http://WebIO/DAC1"; % DAC1 on GPIO pin 25
DAC2 = "http://WebIO/DAC2"; % DAC2 on GPIO pin 26

webwrite(DAC1, "raw", "128");
webwrite(DAC2, "raw", "128");

webwrite(ADC, "scale", "0.5", "oversampling", "16");

disp("scaled ADC input running");
disp("  press any key to stop");
do
  webread(ADC, "value", "34,35")
  pause(1);
until length(kbhit(1)) > 0 % end on any keystroke
disp("  stopped");

webwrite(DAC1, "disable", "");
webwrite(DAC2, "disable", "");

