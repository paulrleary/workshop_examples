clear
close all

fs = 44800;
lf = 700;
hf = 1300;

n = 34;

block = 256;

b = fir1(n-1, [lf/fs, hf/fs]);

fileID = fopen('firCoeffs.h','w');
fprintf(fileID, '#define NUM_TAPS %d\n', n);
fprintf(fileID, '#define BLOCK_SIZE %d\n', block);
fprintf(fileID, 'static float32_t bp_firState[BLOCK_SIZE + NUM_TAPS - 1];\n');
fprintf(fileID, 'const float32_t firCoeffs[NUM_TAPS] = {');
fprintf(fileID,'%12.8ff,',b);
fprintf(fileID,'};\n');
fclose(fileID);