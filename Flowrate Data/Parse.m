clear; format short e
fileID = fopen('LOWFLOW.txt');
C = textscan(fileID,'%s %s %f32');
fclose(fileID);
celldisp(C)