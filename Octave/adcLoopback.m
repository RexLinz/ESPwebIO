% test of scaled DAC output

% server web address or host name
%GPIO = "http://WebIO/GPIO";
ADC  = "http://WebIO/ADC";
DAC1 = "http://WebIO/DAC1"; % DAC1 on GPIO pin 25
DAC2 = "http://WebIO/DAC2"; % DAC1 on GPIO pin 26

attMode = {"0dB", "2.5dB", "6dB", "11dB"};

N = 0:255;
val1 = NaN(length(N), length(attMode));
val2 = NaN(length(N),length(attMode));

% webwrite(GPIO, "modeAnalog", "34,35");
webwrite(ADC,
  "pins", "34,35",
  "oversampling", "16"
);

for a=1:length(attMode)
  webwrite(ADC, "attenuation", char(attMode(a)));

  disp(["attenuation " char(attMode(a)) ", scanning all DAC values"]);
  tic;
  i = 0;
  for n = N
    webwrite(DAC1, "raw", num2str(n));
    webwrite(DAC2, "raw", num2str(n));
%    r = webread(ADC, "raw", "34,35");
    r = webread(ADC, "raw", ""); % all configured pins
    v = JSONextract(r, "raw");
    i = i+1;
    val1(i, a) = v(1);
    val2(i, a) = v(2);
  end
  toc
end

webwrite(DAC1, "disable", "");
webwrite(DAC2, "disable", "");

plot(
  N, val1(:,1), "r", % DAC1 0dB
  N, val2(:,1), "b", % DAC2 0dB
  N, val1(:,2), "r", % DAC1 2.5dB
  N, val2(:,2), "b", % DAC2 2.5dB
  N, val1(:,3), "r", % DAC1 6dB
  N, val2(:,3), "b", % DAC2 6dB
  N, val1(:,4), "r", % DAC1 11dB
  N, val2(:,4), "b"  % DAC2 11dB
);
grid on;
xlabel("DAC value");
ylabel("ADC reading");
legend("DAC1", "DAC2", "location", "southeast");

