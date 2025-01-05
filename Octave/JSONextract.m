% function val = JSONextract(json, key, isHex=false)
%
% very simple implementation to extract a value from JSON string
% accepting numbers, array of numbers and strings.
% Strings might hold hexadecimal uint8 values.
% If isHex is true these are converted to numeric / numeric array
%
% restrictions of this implementation
%   key must be immediately followed by ":"
%   booleans are not accepted
%   some invalid JSON strings might get parsed without errors
%
% sample of valid JSON string
% {
% "num": 15.2,
% "numArray": [12.5,13,14],
% "string": "hello",
% "hex": "4C",
% "hexArray": "4C,4F,0d,0A"
% }

function val = JSONextract(json, key, isHex=false)

% used for testing only
%if nargin<1
%  json = [
%    "{\r\n\"num\": 15.2,\r\n" ...
%    "\"numArray\": [12,3.1415,2.7183],\r\n" ...
%    "\"string\": \"hello\",\r\n" ...
%    "\"hex\": \"4C\",\r\n" ...
%    "\"hexArray\": \"4C,4F,0d,0A\"" ...
%    "}"];
%end
%if nargin<2
%  switch 3
%    case 1
%      key = "num";
%    case 2
%      key = "numArray";
%    case 3
%      key = "string";
%    case 4
%      key = "hex";
%      isHex=true;
%    case 5
%      key = "hexArray";
%      isHex=true;
%  end
%end

key = ["\"" key "\":"]; % key with delimiter
p = strfind(json, key) + length(key);
if isempty(p)
  val = NA;
  return
end

json = strtrim(json(p:end)); % remove whitespace

if json(1)=="\"" % value is string
  p = strfind(json(2:end), "\""); % find end of string
  json = json(2:p(1)); % first " found
  if isHex % string is list of hexadecimal values
    val = uint8(sscanf(json, "%x,"));
  else
    val = json;
  end
elseif json(1) == "[" % numeric array
  p = strfind(json(2:end), "]");
  json = json(2:p);
  val = sscanf(json, "%f,");
else % expect single numbers
  val = sscanf(json, "%f,");
end

