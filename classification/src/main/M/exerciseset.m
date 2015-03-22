%% Exercise set frequency
% Measures the period in a suspected set

%% 
% Load the CSV, filter out relevant rows
% To see the effect of non-exercise, remove the filtering on the .timestamp

M = readtable('all_4.csv');
rows = strcmp(M.location, 'wrist.0') & M.timestamp > 800 & M.timestamp < 2000;
% ``xs`` are just the [timestamp, x] points from M
xs   = table2array(M(rows,5));
time = table2array(M(rows,1));
plot(time, xs);

%% Use the sigfft function
% Analyse the signal
[freq, period, power] = sigfft(xs);

plot(period, power);
title(['Period = ', num2str(freq)]);
