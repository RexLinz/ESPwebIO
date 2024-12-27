% testing of TCP stream access to WebIO

clear

pkg load instrument-control

% ESP32 WebIO
% host = resolvehost("WebIO");
% port = 1026;
webIO = tcpclient("WebIO", 1026, "Timeout", 2);
if webIO.Status ~= "open"
  error("could not connect to device via TCP");
end
configureTerminator(webIO, 4); % 004 = End Of Text (EOT)
flush(webIO);

switch 6
  case 1 % get status and root help
    disp(writeread(webIO, "/status"));
    disp(writeread(webIO, "/"));
  case 2 % query GPIO help
    disp(writeread(webIO, "/GPIO"));
  case 3 % query GPIO pin values
    disp(writeread(webIO, "/GPIO?state=21,22,23"))
  case 4 % query GPIO pin values
    disp("blinking LED at GPIO outputs 21,23");
    writeread(webIO, "/GPIO?output=21,22,23&set=21,22");
    disp("press any key to stop");
    do
      disp(writeread(webIO, "/GPIO?toggle=21,23"));
      pause(1.0);
    until length(kbhit(1)) > 0 % end on any keystroke
    disp("  stopped");
  case 5
    disp("set DAC output values");
    disp(writeread(webIO, "/DAC1?raw=50"));
    disp(writeread(webIO, "/DAC2?raw=100"));
  case 6
    disp("reading raw values from ADC input 34,35");
    disp("press any key to stop");
    do
      disp(writeread(webIO, "/ADC?raw=34,35"));
      pause(1.0);
    until length(kbhit(1)) > 0 % end on any keystroke
    disp("  stopped");
end

% clean up connection
clear webIO

