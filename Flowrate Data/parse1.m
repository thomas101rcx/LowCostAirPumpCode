%# read the whole file to a temporary cell array
fid = fopen('HIGHFLOW.txt','rt');
tmp = textscan(fid,'%s','Delimiter','\n');
fclose(fid);

%# remove the lines starting with headerline
tmp = tmp{1};
idx = cellfun(@(x) strcmp(x(1:10),'HighFlowRate_0.6'), tmp);
tmp(idx) = [];

%# split and concatenate the rest
result = regexp(tmp,' ','split');
result = cat(1,result{:});

%# delete temporary array (if you want)
clear tmp