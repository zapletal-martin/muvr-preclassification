%% Exercise set frequency
% Measures the period in a suspected set

%% Load data
% Load the CSV, filter out relevant rows
% To see the effect of non-exercise, remove the filtering on the .timestamp

M = readtable('/Users/janmachacek/Muvr/muvr-preclassification/training-data/jan-back-22-05/10.csv');
ads = table2array(M(:,[5,6,7]));
ads = ads(500:end,:);
time = 1:length(ads);
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
