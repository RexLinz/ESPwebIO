% testing of TCP stream access to WebIO

clear

pkg load instrument-control

% ESP32 WebIO
% host = resolvehost("WebIO");
% port = 1026;
webIO = tcpclient("WebIO", 1026, "Timeout", 1);
if webIO.Status ~= "open"
  error("could not connect to device via TCP");
end

switch 3
  case 1 % get status and root help
    disp(tcpQuery(webIO, "/status"));
    disp(tcpQuery(webIO, "/"));
  case 2 % query GPIO help
    disp(tcpQuery(webIO, "/GPIO"));
  case 3 % query GPIO pin values
    disp(tcpQuery(webIO, "/GPIO?state=21,22,23"))
  case 4 % query GPIO pin values
    disp("blinking LED at GPIO outputs 21,23");
    tcpQuery(webIO, "/GPIO?output=21,22,23&set=21,22");
    disp("press any key to stop");
    do
      disp(tcpQuery(webIO, "/GPIO?toggle=21,23"));
      pause(1.0);
    until length(kbhit(1)) > 0 % end on any keystroke
    disp("  stopped");
  case 5
    disp("reading raw values from ADC input 34,35");
    disp("press any key to stop");
    do
      disp(tcpQuery(webIO, "/ADC?raw=34,35"));
      pause(1.0);
    until length(kbhit(1)) > 0 % end on any keystroke
    disp("  stopped");
end

% clean up connection
clear webIO

