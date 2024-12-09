% test WebIO's serial IO
% forwardong data from Serial2 to Octave
% e.g. data from GPS module, ...
% connect the device's TX data to GPIO pin 16 = Serial2 RX pin

% read version information
% webread("http://WebIO/")

% read status information
% webread("http://WebIO/status")

% server web address or host name
serial2 = "http://WebIO/Serial2"; % serial interface to host
webwrite(serial2, "begin", "9600"); % default baud rate of most GPS modules

switch 2
  case 0
    disp("receive all data available on each call");
    disp("press any key to stop");
    webread(serial2, "read", ""); % clear any input data in buffer
    do
      allData = webread(serial2, "read", "");
      if length(allData>0)
        disp(allData);
      end
      pause(0.1); % avoid useless traffic
    until length(kbhit(1)) > 0 % end on any keystroke
  case 1
    disp("receive full lines ending with CRLF pairs");
    disp("press any key to stop");
    webread(serial2, "read", ""); % clear any input data in buffer
    do
      lineData = webread(serial2, "readln", "");
      if length(lineData>0)
        disp(lineData);
      end
      % we have to read fast to catch up all data!
      % so we use no delay between web requests
    until length(kbhit(1)) > 0 % end on any keystroke
  case 2
    disp("receive full lines, but display line starting with $GNGGA, only");
    disp("  (NMEA message containing Time (UTC), Longitude and Latitude)");
    disp("press any key to stop");
    webread(serial2, "read", ""); % clear any input data in buffer
    do
      lineData = webread(serial2, "readln", "");
      if strncmp(lineData, "$GNGGA,", 7) > 0
        disp(lineData);
      end
      % we have to read fast to catch up all data!
      % so we use no delay between web requests
    until length(kbhit(1)) > 0 % end on any keystroke
end

