function myKalman2(duration,dt)
%Writen by Xavier Ang
%Kalman Filter Example 2 
%Falling body example from Kleeman's paper
%Argument(s):
%duration = duration of whole simulation (sec)
%dt = time step for each measurement (sec)
%This function simply uses the falling body dynamic model to perform
%Kalman filtering on the measured position. True, measured and estimated
%parameters are then plotted on the same axis.

close all;
%KF equations
F = [1 1 ; 0 1]; %transition matrix
G = [0.5 ; 1]; %input vector matrix
H = [1 0]; %measurement matrix
x = [100;0]; %initial state vector
xhat = [95 ;1]; %initial estimate state vector
g=1;

%Covariances
Q = 0; %Process noise covariance
P = [.1 0; 0 .1]; %Initial process noise covariance
R = 1; %Measurement noise covariance

%setup for residual vector
Res = zeros(size(R));

pos = [x(1)]; %true position
pos_hat = [xhat(1)]; %estimate position
vel_hat = [xhat(2)]; %estimate velocity
pos_measurement = [100 97.9 94.4 92.7 87.3]; %measured position

counter=0;
for i=1:dt:duration
    counter=counter+1;
    
    %State estimation process (kleeman's steps)
    x = F*x + G*(-g); %1) State prediction
    z = H*x  ; %2) Measurement prediction
    Res = pos_measurement(i) - z ;  %3) measurement residual
    
    %State covariance estimation process (kleeman's steps)
    P = F*P*F' + Q; %1) State prediction covariance
    S = H*P*H' + R; %2) Measurement prediction covariance/Residual covariance
    K = P*H'*inv(S); %3) K.Filter Gain
    P = P - K*S*K'; %4) Update state covariance
    
    xhat = x + K*Res; %4) Update state estimate
    
    pos = [pos; x(1)];
    pos_hat = [pos_hat; xhat(1)];
    vel_hat = [vel_hat; xhat(2)];
  
end

%plot after simulation is complete
t = 0:dt:duration;
t = t';
t_m = 1:dt:duration;
t_m = t_m';
hold on
plot(t,pos,'b');
plot(t,pos_hat,'g');
plot(t_m,pos_measurement,'r');
grid;
xlabel('Time (sec)');
ylabel('Position');
title('Kalman Filter - Falling body example');
legend('True position','Estimated position','Measured position');
hold off
    


