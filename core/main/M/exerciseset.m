%% Exercise set frequency
% Measures the period in a suspected set

%% 
% Load the CSV, filter out relevant rows
% To see the effect of non-exercise, remove the filtering on the .timestamp

M = readtable('all_4.csv');
rows = strcmp(M.location, 'wrist.0') & M.timestamp > 800 & M.timestamp < 2000;

time = table2array(M(rows,1));
ads  = table2array(M(rows,[5, 6, 7]));
subplot(2, 1, 1);
plot(time, ads);

%% Use the sigfft function
% Analyse the signal
[freqx, periodx, powerx] = sigfft(ads(:,1));
[freqy, periody, powery] = sigfft(ads(:,2));
[freqz, periodz, powerz] = sigfft(ads(:,3));

subplot(2, 1, 2);
hold on;
plot(periodx, powerx);
plot(periody, powery);
plot(periodz, powerz);
title(['Period = ', num2str([freqx, freqy, freqz])]);
hold off;
