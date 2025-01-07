% testing SPI loop
%
% connect miso to mosi to receive sent data
%
% default pins
%   HSPI: sck=14, miso=12, mosi=13, ss=15
%   VSPI: sck=18, miso=19, mosi=23, ss=5
% remapped pins
%   sck=18, miso=16, mosi=17, ss=5

clear

switch 2

  % default pins
  case 1
    SPI = "http://WebIO/SPI"
  case 2
    SPI = "http://WebIO/HSPI"
  case 3
    SPI = "http://WebIO/VSPI"

  % remapped pins
  case 11
    SPI = "http://WebIO/SPI"
    webread(SPI, "pins", "18,16,17,5") % remap miso,mosi to Serial2 RX/TX
  case 12
    SPI = "http://WebIO/HSPI"
    webread(SPI, "pins", "18,16,17,5") % remap miso,mosi to Serial2 RX/TX
  case 13
    SPI = "http://WebIO/HSPI"
    webread(SPI, "pins", "18,16,17,5") % remap miso,mosi to Serial2 RX/TX
end

webread(SPI, "begin", ""); % start with default parameters

disp("SPI loop test started, press any key to stop");
n = uint8(0);
do
  % read "changed" status
  txData = [num2hex(n) "," num2hex(n+1)]
  s = webread(SPI, "writeread", txData);
  rxData = JSONextract(s, "writeread", true);
  if rxData(2) ~= (n+1)
    disp(rxData')
    error("invalid data received");
  end
  n = n+1;
  pause(1.0);
until kbhit(1)
disp("  stopped");

webread(SPI, "end", ""); % stop SPI bus

