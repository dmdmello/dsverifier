function reproduce_overflow(path, varargin)
%
% Script developed to reproduce overflow property given a 'path' with all .out counterexamples.
%
% Function: reproduce_overflow(path)
%
% You need inform the 'path', that is a directory with all counterexamples stored in a .out files.
%
% The output is the feedback returned from simulation (successful or failed) and a .MAT file with counterexamples stored.
%
% Another usage form is adding other parameters (optional parameters) as follow:
%
% reproduce_overflow(path, ovmode, rmode, filename);
%
% Where:
%  ovmode is related to overflow mode and it could be: 'saturate' or 'wrap'. By default is 'wrap';
%  rmode is related to rounding mode and it could be: 'round' or 'floor'. By default is 'round';
%  filename is the name of counterexample .MAT file generated. By default is 'digital_system'.
%
%  Example of usage:
%
%  reproduce_overflow('/home/user/log/overflow/');
%
%  reproduce_overflow('/home/user/log/overflow/','saturate','floor','counterexample_file');
%
% Lennon Chaves
% November 04, 2016
% Manaus, Brazil

ovmode = '';
rmode = '';
filename = '';

nvar = nargin;

if nvar >= 2
if length(var{1}) > 0
 ovmode = var{1};
end
end

if nvar >= 3
if length(var{2}) > 0
 rmode = var{2};
end
end

if nvar >= 4
if length(var{3}) > 0
 filename = var{3};
end
end

property = 'o';

dsv_validation(path, property, ovmode, rmode, filename);

end
