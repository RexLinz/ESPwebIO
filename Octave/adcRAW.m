% test of raw ADC input

% server web address or host name
ADC  = "http://WebIO/ADC";
DAC1 = "http://WebIO/DAC1"; % DAC1 on GPIO pin 25
DAC2 = "http://WebIO/DAC2"; % DAC2 on GPIO pin 26

webwrite(DAC1, "raw", "200");
webwrite(DAC2, "raw", "100");

disp("raw ADC input running");
disp("  press any key to stop");
do
  webread(ADC, "raw", "34,35")
  pause(1);
until length(kbhit(1)) > 0 % end on any keystroke
disp("  stopped");

webwrite(DAC1, "disable", "");
webwrite(DAC2, "disable", "");

