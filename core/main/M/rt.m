%% Connect mobile
connector on foobar;
m = mobiledev;


%% Plot in RT
figure;
hold on;

for i = 1:1000;
  clf;
  ad = accellog(m);
  x = ad(length(ad)-500:end,:);
  
  [freqx, periodx, powerx] = sigfft(x(:,1));
  [freqy, periody, powery] = sigfft(x(:,2));
  [freqz, periodz, powerz] = sigfft(x(:,3));
  freqs = [freqx, freqy, freqz];

  subplot(2, 1, 1);
  hold on;
  plot(periodx, powerx);
  plot(periody, powery);
  plot(periodz, powerz);
  hold off;
  subplot(2, 1, 2);
  plot(x);
  
  drawnow;
  pause(5);
end