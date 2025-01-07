% testing of I2C IO
% using board with one PCF8574A port expander
% bits 0..3 connected to LED's
% bits 4..7 connected to buttons
%
% SDA = GPIO21
% SCL = GPIO22
% INT = GPIO23 (status change interrupt)

I2C = "http://WebIO/I2C";
INT = "http://WebIO/GPIO?state=23";

s = webread(I2C,
%  "sda", "21", % 21 is default
%  "scl", "22", % 22 is default
  "frequency", "400000", % default is 100 kHz
  "begin", "",
  "scan", ""
);

% extract address of device found
% we expect PCF8574A on address 0x38
% NOTE the scan will set this device address as default
address = JSONextract(s, "scan", true);
disp(["device found at address 0x" num2hex(address)]);

disp("\nscan LED's, press any key to stop");
direction = +1;
do
  % read "changed" status
  s = webread(INT);
  unchanged = JSONextract(s, "state", true);
  % read input(will reset changed status)
  s = webread(I2C, "read", "1");
  val = JSONextract(s, "read", true);
  disp(["inputs = " dec2bin(val,8)]);
  if unchanged==0
    if bitand(val, 128)==0 % input 7 = rightmost button
      direction = +1;
      disp("low to high");
    elseif bitand(val, 64)==0 % input 6
      val = bitor(val, 15);
      disp("restarted");
    elseif bitand(val, 32)==0 % input 5
      direction = 0;
      disp("stopped");
    elseif bitand(val, 16)==0 % input 4 = leftmost button
      direction = -1;
      disp("high to low");
    end
  end
  switch direction
    case +1 % LEDs running low to high
      val = bitand(bitshift(val,+1), 15);
      if val==0
        val=1;
      end
    case 0
      % stopped
    case -1 % LEDs running high to low
      val = bitshift(bitand(val,15), -1);
      if val==0
        val=8;
      end
    otherwise
      disp("invalid direction");
  end
  val = bitor(val, hex2dec("F0")); % always set button lines to 1
  webwrite(I2C, "write", num2hex(uint8(val)));
  pause(0.5)
until kbhit(1)
disp("  stopped");

webread(I2C, "end", ""); % stop I2C bus

