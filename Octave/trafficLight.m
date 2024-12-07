% traffic light using WebIO

% server web address or host name
server = "http://WebIO/GPIO";

% pin definitions
red    = "23";
yellow = "22";
green  = "21";

% set pins as output
webwrite(server, "output", red, "output", yellow, "output", green);
% equivalent to
% webwrite(server, "output", "21,22,23");

% start with red ON
webwrite(server, "set", red);

disp("WebIO traffic light running");
disp("  press any key to stop next time on red");
do
  pause(3);
  webwrite(server, "set", yellow);
  pause(1);
  webwrite(server, "clear", red, "clear", yellow , "set", green);
  pause(3);
  for n=1:4
    webwrite(server, "clear", green);
    pause(0.5);
    webwrite(server, "set", green);
    pause(0.5);
  end
  webwrite(server, "clear", green, "set", yellow);
  pause(1);
  webwrite(server, "clear", yellow, "set", red);
until length(kbhit(1)) > 0 % end on any keystroke
disp("  stopped");

