% test of raw DAC output

% server web address or host name
DAC1 = "http://WebIO/DAC1"; % DAC1 on GPIO pin 25
DAC2 = "http://WebIO/DAC2"; % DAC2 on GPIO pin 26

% output full scale range square wave on DAC1
disp("raw DAC output running");
disp("  press any key to stop");
do
  webwrite(DAC1, "raw", "0");
  webwrite(DAC2, "raw", "255");
  pause(1);
  webwrite(DAC1, "raw", "255");
  webwrite(DAC2, "raw", "0");
  pause(1);
until kbhit(1) % end on any keystroke
disp("  stopped");

webwrite(DAC1, "disable", "");
webwrite(DAC2, "disable", "");

