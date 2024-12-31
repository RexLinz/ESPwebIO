% testing of PWM functions

PWM = "http://WebIO/PWM";

PWMhelp = webread(PWM); % get help
if length(PWMhelp) == 0
  error("no connection to WebIO");
end
disp(PWMhelp);

webwrite(PWM,
  "frequency", "5",
  "channels", "0,1",
  "map", "21,23"
)

webwrite(PWM,
  "duty", "0.01,0.1"
);

webwrite(PWM,
  "frequency", "1",
  "channels", "2",
  "map", "22",
  "val", "500"
)

disp("press any key to stop");
while length(kbhit(1)) == 0
end
disp("  stopped");

webwrite(PWM,
  "stop", "0,1,2"
)

