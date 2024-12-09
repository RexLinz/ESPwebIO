% test WebIO's serial IO
% sending to / receiving from console

% server web address or host name
serial0 = "http://WebIO/Serial0"; % serial interface to host

webwrite(serial0, "write", "hello host"); % no newline at the end
pause(1);
webwrite(serial0, "writeln", " ... ending this line");

% input from USB host interface
disp("any data available on serial0 will be displayed in 10 seconds");
pause(10);
% read all data available on interface
% note each keyword requires a value in webread
allData = webread(serial0, "read", "")

disp("write a line and end with CR");
do
  lineData = webread(serial0, "readln", "");
  pause(0.5); % avoid useless traffic
until length(lineData)>0
disp(lineData);
disp("other data available after this line");
webread(serial0, "read", "")

