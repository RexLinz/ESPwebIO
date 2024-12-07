% test of scaled DAC output

% server web address or host name
DAC1 = "http://WebIO/DAC1"; % DAC1 on GPIO pin 25
DAC2 = "http://WebIO/DAC2"; % DAC1 on GPIO pin 26

% scale output of 0.0 to 3.0 approximately to volts on pin
webwrite(DAC1, "scale", num2str(255/3.15));
% scale output of -/+ 1 to full scale
webwrite(DAC2, "scale", num2str(127/1.0), "offset", "127.5");

% output ramp on DAC1, sinewave with offset on DAC2
ramp = 0.0; % ramp value for DAC1
phi = 0.0; % phasse for DAC2
do
  ramp += 0.1;
  if ramp>3.0, ramp=0.0; end
  webwrite(DAC1, "value", num2str(ramp));
  phi += 0.1;
  webwrite(DAC2, "value", num2str(sin(phi)));
  pause(0.5);
until length(kbhit(1)) > 0 % end on any keystroke

webwrite(DAC1, "disable", "")
webwrite(DAC2, "disable", "")

