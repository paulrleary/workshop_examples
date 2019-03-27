clear
close all

lw = 2;

d = [0, 0, 1,0,0,1,0,1,0,1,1,0,0,0,0,];
d = [d,d,d,d];
t = 1:length(d);
lab = sprintfc('%d',d);

align_v = 'bottom';
align_h = 'left';

fig = figure;
hold on
plot(t, zeros(size(t)), 'k--', 'LineWidth', lw/2)
sig = d;
stairs(t, sig, 'b','linewidth', lw)
text(t,sig, lab,'VerticalAlignment',align_v,'HorizontalAlignment',align_h)
ylabel('Binary Value')
ylim([-0.2, 1.2])
set(findall(fig,'-property','FontSize'),'FontSize',20)
set(gca, 'xticklabel', '')
print(fig, 'Binary_signal', '-dpng')

fig = figure;
hold on
plot(t, zeros(size(t)), 'k--', 'LineWidth', lw/2,'HandleVisibility','off')
sig = d*5;
stairs(t, sig, 'b', 'linewidth', lw)
stairs(t, d*3.3, 'r-.', 'linewidth', lw)
text(t,sig, lab,'VerticalAlignment',align_v,'HorizontalAlignment',align_h)
legend('TTL', 'CMOS')
ylabel('Voltage')
ylim([-0.2, 5.2])
set(findall(fig,'-property','FontSize'),'FontSize',20)
set(gca, 'xticklabel', '')
print(fig, 'DigVoltage_signal', '-dpng')

fig = figure;
hold on
plot(t, zeros(size(t)), 'k--', 'LineWidth', lw/2)
sig = (~d)*20- 10;
stairs(t, sig, 'b', 'linewidth', lw)
text(t,sig, lab,'VerticalAlignment',align_v,'HorizontalAlignment',align_h)
ylabel('Voltage')
ylim([-10.2, 10.2])
set(findall(fig,'-property','FontSize'),'FontSize',20)
set(gca, 'xticklabel', '')
print(fig, 'RS232Voltage_signal', '-dpng')


return
left_color = [1 0 0];
right_color = [0 0 1];
if 0
fig = figure;
set(fig,'defaultAxesColorOrder',[left_color; right_color]);
hold on
% yyaxis left
vcc = 5;
plot(t, sig*vcc, 'color', left_color, 'linewidth', lw)
ax1 = gca;
xlim([t(1) t(end)]);
ylim(ax1, [0 vcc]);
ylabel('Analog Voltage');
set(ax1, 'YColor', left_color)

% yyaxis right
% res = 2^4;
% plot(t, floor(sig*res), 'b',  'linewidth', lw)
% ax2 = gca;
% xlim([t(1), t(end)]);
% ylim(ax2, [0 res]);
% ylabel('Digital Value')

set(ax1, 'xticklabel', '')

set(findall(fig,'-property','FontSize'),'FontSize',20)
print(fig, 'Analog_only', '-dpng')

fig = figure;
set(fig,'defaultAxesColorOrder',[left_color; right_color]);
hold on
yyaxis left
vcc = 5;
plot(t, sig*vcc, 'color', left_color, 'linewidth', lw)
ax1 = gca;
xlim([t(1) t(end)]);
ylim(ax1, [0 vcc]);
ylabel('Analog Voltage');

yyaxis right
res = 2^4;
plot(t, floor(sig*res), 'b',  'linewidth', lw)
ax2 = gca;
xlim([t(1), t(end)]);
ylim(ax2, [0 res]);
ylabel('Digital Value')

set(ax1, 'xticklabel', '')

set(findall(fig,'-property','FontSize'),'FontSize',20)
print(fig, 'Analog_and_4bit', '-dpng')


fig = figure;
sig = 0.5*sin(4*t) + 0.5;

% set(fig,'defaultAxesColorOrder',[left_color; right_color]);
hold on
% yyaxis left
vcc = 5;
plot(t, sig*vcc, 'color', left_color, 'linewidth', lw)
ax1 = gca;
ylabel('Analog Voltage');

lw = 2;
% yyaxis right
res = 2^4;
plot(t, (vcc/res)*(floor(sig*res)), 'color',[0 0 1],  'linewidth', lw)

bits = 5;
res = 2^bits;
plot(t, (vcc/res)*(floor(sig*res)), 'color',[0.1*5 0.1*bits 1] ,  'linewidth', lw)

bits = 6;
res = 2^bits;
plot(t, (vcc/res)*(floor(sig*res)), 'color',[0.1*5 0.1*bits 1] ,  'linewidth', lw)

bits = 8;
res = 2^bits;
plot(t, (vcc/res)*(floor(sig*res)), 'color',[0.1*5 0.1*bits 1] ,  'linewidth', lw)

bits = 12;
res = 2^bits;
plot(t, (vcc/res)*(floor(sig*res)), 'color','g' ,  'linewidth', lw)

bits = 16;
res = 2^bits;
plot(t, (vcc/res)*(floor(sig*res)),'linestyle', '--', 'color','c' ,  'linewidth', lw)

legend('Analog', '4bit', '5bit', '6bit', '8bit', '12bit', '16bit')
% ax2 = gca;
% xlim([t(1), t(end)]);
% ylim(ax2, [0 res]);
% ylabel('Digital Value')
% get(ax1);
% res = 2^5;
% addaxis(t, floor(sig*res), [0 res], 'color', 'c', 'linewidth', lw)
% ax3 = gca;

set(ax1, 'xticklabel', '')
xlim([t(4100) t(4400)]);
ylim(ax1, [0 vcc/8]);


set(findall(fig,'-property','FontSize'),'FontSize',20)
print(fig, 'Multibit', '-dpng')
end

fig = figure;
hold on
lw =4;
ms = 16;

x=[0 0]; y = [0 5];
plot(x,y, 'r', 'linewidth',lw)

res = 4;
x = 0.1*ones(1, 2^res); y = [0:(2^res)-1].*(5/(2^res));
plot(x,y, '.', 'markersize', ms)

res = 6;
x = 0.2*ones(1, 2^res); y = [0:(2^res)-1].*(5/(2^res));
plot(x,y, '.', 'markersize', ms)

res = 8;
x = 0.3*ones(1, 2^res); y = [0:(2^res)-1].*(5/(2^res));
plot(x,y, '.', 'markersize', ms)

res = 12;
x = 0.4*ones(1, 2^res); y = [0:(2^res)-1].*(5/(2^res));
h12 = plot(x,y, '.', 'markersize', ms);

res = 16;
x = 0.5*ones(1, 2^res); y = [0:(2^res)-1].*(5/(2^res));
h16 = plot(x,y, '.', 'markersize', ms);

xlim(gca, [-0.1,1])
ylim(gca, [0, 5])

ylabel('Voltage')
set(gca, 'xticklabel', '')
legend('Analog', '4bit', '6bit', '8bit', '12bit', '16bit')

set(findall(fig,'-property','FontSize'),'FontSize',20)
print(fig, 'Resolution', '-dpng')

ylim(gca, [0 0.1]);
% set(h12, 'marker', '+');
% set(h16, 'marker', '+');
print(fig, 'ResolutionZoom', '-dpng')

return
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