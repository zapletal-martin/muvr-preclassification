%% XXX frequency
% Measures the period in a suspected set

%% 
% Load the CSV, filter out relevant rows
% To see the effect of non-exercise, remove the filtering on the .timestamp

M = readtable('all_4.csv');
rows = strcmp(M.location, 'wrist.0') & M.timestamp > 800 & M.timestamp < 2000;
% ``xs`` are just the [timestamp, x] points from M
xs = M(rows,[1,5]);

%%
% Exercise activity is cyclical, and there is typically a major component
% that falls in the range of 1 to 5 seconds or 100 - 500 cycles /
% timestamp.

time = table2array(xs(:,1));
relNums = table2array(xs(:,2));
plot(time,relNums)
title('X acceleration data from wrist')

%%
% Take FFT of the xs. The first component of Y, Y(1), is simply the sum of 
% the data, and can be removed.

Y = fft(relNums);
Y(1) = [];

%%
% A graph of the distribution of the Fourier coefficients (given by Y) in the
% complex plane is pretty, but difficult to interpret.  We need a more useful way
% of examining the data in Y.

plot(Y,'ro')
title('Fourier Coefficients in the Complex Plane');
xlabel('Real Axis');
ylabel('Imaginary Axis');

%%
% The complex magnitude squared of Y is called the power, and a plot of power
% versus frequency is a "periodogram".

n = length(Y);
power = abs(Y(1:floor(n/2))).^2;
nyquist = 1/2;
freq = (1:n/2)/(n/2)*nyquist;
plot(freq,power)
xlabel('cycles/timestamp')
title('Periodogram')

%%
% The scale in cycles/year is somewhat inconvenient.  We can plot in years/cycle
% and estimate the length of one cycle.

plot(freq(1:40),power(1:40))
xlabel('cycles/sample')

%%
% Now we plot power versus period for convenience (where period = 1./freq).  
% As expected, there is a very prominent cycle that falls in the 1 - 5
% seconds: 100 - 500 samples

period = 1./freq;
plot(period,power);
axis([0 length(Y) 0 max(power) * 1.2]);
ylabel('Power');
xlabel('Period [sample]');

%%
% Finally, we can fix the cycle length a little more precisely by picking out
% the strongest frequency.  The red dot locates this point.

hold on;
index = find(power == max(power));
mainPeriodStr = num2str(period(index));
plot(period(index),power(index),'r.', 'MarkerSize',25);
text(period(index)+2,power(index),['Period = ',mainPeriodStr]);
hold off;


displayEndOfDemoMessage(mfilename)
