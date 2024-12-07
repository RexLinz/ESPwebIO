% test of scaled DAC output

% server web address or host name
GPIO = "http://WebIO/GPIO";
ADC  = "http://WebIO/ADC";
DAC1 = "http://WebIO/DAC1"; % DAC1 on GPIO pin 25
DAC2 = "http://WebIO/DAC2"; % DAC1 on GPIO pin 26

attMode = {"0dB", "2.5dB", "6dB", "11dB"};

N = 256;
val1 = zeros(N,length(attMode));
val2 = zeros(N,length(attMode));

% webwrite(GPIO, "modeAnalog", "34,35");
webwrite(ADC, "oversampling", "16");

for a=1:length(attMode)
  webwrite(ADC, "attenuation", char(attMode(a)));

  disp(["attenuation " char(attMode(a)) ", scanning all DAC values"]);
  tic;
  for n=1:N
    webwrite(DAC1, "raw", num2str(n-1));
    webwrite(DAC2, "raw", num2str(n-1));
    r = webread(ADC, "raw", "34,35");
    p = strfind(r, "\"raw\":[") + length("\"raw\":[");
    v = sscanf(r(p:end), "%d,");
    val1(n, a) = v(1);
    val2(n, a) = v(2);
  end
  toc

end

webwrite(DAC1, "disable", "");
webwrite(DAC2, "disable", "");

N = 0:255;
plot(
  N, val1(:,1), "r",
  N, val2(:,1), "b",
  N, val1(:,2), "r",
  N, val2(:,2), "b",
  N, val1(:,3), "r",
  N, val2(:,3), "b",
  N, val1(:,4), "r",
  N, val2(:,4), "b"
);
grid on;
xlabel("DAC value");
ylabel("ADC reading");
legend("DAC1", "DAC2", "location", "southeast");

