%% Exercise set frequency
% Measures the period in a suspected set

%% Load data
% Load the CSV, filter out relevant rows
% To see the effect of non-exercise, remove the filtering on the .timestamp

M = readtable('all_4.csv');
rows = strcmp(M.location, 'wrist.0') & M.x ~= 0 & M.y ~= 0 & M.z ~= 0 & M.timestamp < 600 & M.timestamp > 200;
FM = M(rows,[5,6,7]);

ads  = table2array(FM);
time = 1:height(FM);
subplot(3, 1, 1);
plot(time, ads);

%% No exercise by no movement 
% Identify whether the window (with data in ``ads(:,n)``) is "no movement"

[nmx, datax] = nomovement(ads(:,1));
[nmy, datay] = nomovement(ads(:,2));
[nmz, dataz] = nomovement(ads(:,3));
subplot(3, 1, 2);
hold on;
plot(time,datax);
plot(time,datay);
plot(time,dataz);
title(['Movement = ', num2str([nmx, nmy, nmz])]);
hold off;

%% Use the sigfft function
% Analyse the signal
[freqx, periodx, powerx] = sigfft(ads(:,1));
[freqy, periody, powery] = sigfft(ads(:,2));
[freqz, periodz, powerz] = sigfft(ads(:,3));
freqs = [freqx, freqy, freqz];
exercises = abs(mean(freqs) - freqs) < 50;

subplot(3, 1, 3);
hold on;
plot(periodx, powerx);
plot(periody, powery);
plot(periodz, powerz);
title(['Exercise = ', num2str(exercises), ', periods = ', num2str(freqs)]);
hold off;
