% testing of PWM functions

PWM = "http://WebIO/PWM";

webread("http://WebIO") % version information

%PWMhelp = webread(PWM); % get help
%if length(PWMhelp) == 0
%  error("no connection to WebIO");
%end
%disp(PWMhelp);

for f = [50 1]
  webwrite(PWM, "frequency", num2str(f))
  for c = [0 8]
    webwrite(PWM,
      "channels", num2str(c),
      "map", "21"
    )
    for val = 2^16*[0.1 0.5 0.9]
      webwrite(PWM, "val", num2str(val))
      pause
    end
    for duty = [0.1 0.5 0.9]
      webwrite(PWM, "duty", num2str(duty, "%.3f"))
      pause
    end
    for w = [0.1 0.5 0.9]
      webwrite(PWM, "width", num2str(1E6*w/f))
      pause
    end
    webwrite(PWM, "stop", "")
  end
end

