clear
close

figure
hold on
for(i = 1:10)
%     i
sample_f_hz = i*44800
buffer = 256;
d = 0.104;

all_xcor_idx = [-(buffer-1):1:buffer-1];

c_ms = 343*0.001; %using timing in ms, consistent with teensy code to avoid decimal issues

dt_ms = 1000/sample_f_hz;
all_tau = all_xcor_idx*dt_ms;
all_phi = asind((all_tau*c_ms)/d);

plot(all_xcor_idx, real(all_phi))
end
