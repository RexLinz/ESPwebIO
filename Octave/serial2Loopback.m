% loopback test on /Serial2
% connect pins 16 and 17 on devkit to get a loopback on serial 2

% server web address or host name
serial2 = "http://WebIO/Serial2";

webwrite(serial2, "begin", "4800"); % start at low baud rate
webwrite(serial2, "write", "hello loopback test 1");
% first read might/will be incomplete, as not all data has been sent out
% note each keyword requires a value in webread
partialData = webread(serial2, "read", "")
% wait some time to complete the transmission
pause(1);
remainingData = webread(serial2, "read", "")

% using a line termination we should get the full string or nothing
webwrite(serial2, "writeln", "hello loopback test 2\r\nincomplete line");
fullLine = webread(serial2, "readln", "") % will read empty (not completed)
% wait some time to complete the transmission
pause(1);
fullLine = webread(serial2, "readln", "") % return first line
remainingLine = webread(serial2, "readln", "") % return remaining

