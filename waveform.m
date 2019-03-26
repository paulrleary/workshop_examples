clear
close all

tick = 1/4480;
tstop = 1;
tstart = 0;

t = tstart:tick:tstop-tick;

noiserange = [-1,1];
noisescale = 1;
noise = noisescale*((noiserange(2) - noiserange(1))*rand(size(t)) + noiserange(1));

composite = [sin(100*t);0.5*sin(86*t+.7346*pi); noise];
% composite = [ noise];



% y = sin(t);
% 

% figure
% y = sum(composite,1);
% plot(t,y)
% ylabel('Volts')
% xlabel('Time (s)')
% set(gca, 'FontSize', 24)
% print(gcf,'unfilteredwave', '-dsvg')

% figure
% y = sum(composite([1,2],:),1);
% plot(t,y, 'linewidth', 2)
% ylabel('Volts')
% xlabel('Time (s)')
% set(gca, 'FontSize', 24)
% print(gcf,'filteredwave', '-dsvg')


% figure
% hold on
% y = sum(composite([1,2],:),1);
% ix = 1:40:length(t);
% plot(t,y, 'linewidth', 2)
% plot(t(ix),y(ix), 'r.','markersize',20)
% ylabel('Volts')
% xlabel('Time (s)')
% set(gca, 'FontSize', 24)
% print(gcf,'sampledwave', '-dsvg')

% figure
% hold on
% y = sum(composite([1,2],:),1);
% [a,b] = bounds(y);
% % y = rescale(y); 
% y = (y)/(b - a);
% [a,b] = bounds(y);
% y = y-a;
% y = round(y*1024);
% ix = 1:40:length(t);
% % plot(t,y)
% % plot(t(ix),y(ix), 'r.')
% ylim([0 1024])
% stairs(t(ix),y(ix),'linewidth', 2)
% ylabel('Bits')
% xlabel('Time (s)')
% set(gca, 'FontSize', 24)
% print(gcf,'digitalwave', '-dsvg')

figure
y = sum(composite([1,2],:),1);
[up, lo] = envelope(y);
% y = abs(y);
% bin = y<1;
% car = 500+bin*1000;
% carrier = sin(car.*t);
% 
car = 100*up;
carrier = sin(car.*t);
% fm = fmmod(bin,10000,20000, 10);
% plot(t,y,t,up,t,lo, 'linewidth', 2)
plot(t,carrier, 'linewidth', 2)
ylabel('EM Field')
xlabel('Time (s)')
set(gca, 'FontSize', 24)
print(gcf,'FMwave', '-dsvg')