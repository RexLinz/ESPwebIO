% function data = tcpQuery(dev, url, terminator = "\004")
%
% send request by url and return response excluding terminator
% default terminator is set to 004 = End Of Transmission (EOT)

function data = tcpQuery(dev, url, terminator = "\004")

write(dev, [url terminator]);

timeout = get(dev, "Timeout");
data = "";
tStart = tic;
while toc(tStart) < timeout
  data = [data char(read(dev, dev.NumBytesAvailable))];
  if (term = index(data, terminator)) > 0 % terminator found
    data = data(1:term-1); % return data excluding terminator
    return;
  end
end
warning ("timeout reading device response");

