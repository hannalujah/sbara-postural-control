%% -----------------------
% Filtering Data

durations = diff(t);

for i = 1:length(durations)
   if i > length(durations)
       break;
   end
   if durations(i) > 500
      durations(i) = [];
   end
end

dt = mean(durations) * 10^(-3);       % Mean data sampling time
Fs = 1 / dt;                        % Mean sampling frequency

cutoff = 10;                        % Cutoff frequency in Hz (change as needed)
order = 3;                          % Filter order

[b, a] = butter(order, cutoff / (Fs / 2), 'low');   % Normalized cutoff

COPx_filtered = filtfilt(b, a, COPx);
COPy_filtered = filtfilt(b, a, COPy);

COPx_mean = mean(COPx_filtered);
COPy_mean = mean(COPy_filtered);

COPx_final = COPx_filtered - COPx_mean;
COPy_final = COPy_filtered - COPy_mean;

if flaG == 0
    figure;
    plot(COPx_filtered,COPy_filtered);
    figure;
    plot(COPx,COPy);
    figure;
    plot(COPx_final,COPy_final);
    flaG = 1;
end

%COP_measures = data_calculation(t, COPx_final, COPy_final);
%-------------------------
% Static Variables

COPx_MD = mean(abs(COPx_final));
COPy_MD = mean(abs(COPy_final));

radius = [];
for i = 1:length(COPx_final)
   radius(i) = sqrt((COPx_final(i))^2 + (COPy_final(i))^2); 
end
radius = radius';
R_M = mean(radius);
R_max = max(radius);
R_RMS = rms(radius);




COPx_RMS = rms(COPx_final);
COPy_RMS = rms(COPy_final);

COPx_MaxD = max(COPx_final);
COPy_MaxD = max(COPy_final);

N = length(COPx_final);
COV = 0;
for i = 1:N
   COV = COV + (COPx_final(i))*(COPy_final(i)); 
end
COV = COV / N;

%------------------------
% Dynamic Variables
T = N/Fs; %total signal duration

deltaX = abs(diff(COPx_final));
deltaY = abs(diff(COPy_final));

SwayLengthX = sum(deltaX);
SwayLengthY = sum(deltaY);
SwayLength  = 0;
for i = 1:N-1
    SwayLength = SwayLength + sqrt((deltaX(i))^2 + (deltaY(i))^2);    
end

COPx_MeanSPD = SwayLengthX/T;
COPy_MeanSPD = SwayLengthY/T;
MeanSPD      = SwayLength/T;

durations = diff(t);
for i = 1:N-1
    if i > length(durations)
        break;
    end
    
    if durations(i) > 50
       durations(i) = [];
       deltaX(i) = [];
       deltaY(i) = [];
    end
end

durations = durations * 10^(-3);
COPx_V = [];
COPy_V = [];

for i = 1:length(durations)
   COPx_V(i) = deltaX(i) / durations(i); 
   COPy_V(i) = deltaY(i) / durations(i);
end

COPx_VMean = mean(COPx_V);
COPy_VMean = mean(COPy_V);

COPx_VRMS = rms(COPx_V);
COPy_VRMS = rms(COPy_V);

COPx_VSTD = std(COPx_V);
COPy_VSTD = std(COPy_V);

%VRMSXt = 0;
%for i = 1:length(durations)
 %  VRMSXt = VRMSXt + (COPx_V(i) - COPx_VMean)^2;
%end
%VRMSX = sqrt((1/length(durations))*VRMSXt);
%----------------------------
% Frequency Domain
MFREQx = (1/(4*sqrt(2))) * COPx_MeanSPD/COPx_MD;
MFREQy = (1/(4*sqrt(2))) * COPy_MeanSPD/COPy_MD;
MFREQ =  (1/(4*sqrt(2))) * MeanSPD/R_M;

[Pxx,Fx] = pwelch(COPx_final,[],[],[],Fs);
[Pyy,Fy] = pwelch(COPy_final,[],[],[],Fs);
[Pxy,Fxy] = pwelch(radius,[],[],[],Fs);

dFx = Fx(2) - Fx(1);             % Frequency bin width
dFy = Fy(2) - Fy(1);
dFxy = Fxy(2) - Fxy(1);
total_powerX = sum(Pxx) * dFx; % Approximation of integral
total_powerY = sum(Pyy) * dFy;
total_powerXY = sum(Pxy) * dFxy;

cumulative_powerX = cumsum(Pxx) * dFx;
cumulative_powerY = cumsum(Pyy) * dFy;
cumulative_powerXY = cumsum(Pxy) * dFxy;

% Total power double-checked
total_powerx1 = cumulative_powerX(end);
total_powery1 = cumulative_powerY(end);
total_powerxy1 = cumulative_powerXY(end);

% Find frequency indices
f50x_idx = find(cumulative_powerX >= 0.50 * total_powerX, 1);
f50y_idx = find(cumulative_powerY >= 0.50 * total_powerY, 1);
f50xy_idx = find(cumulative_powerXY >= 0.50 * total_powerXY, 1);

f95x_idx = find(cumulative_powerX >= 0.95 * total_powerX, 1);
f95y_idx = find(cumulative_powerY >= 0.95 * total_powerY, 1);
f95xy_idx = find(cumulative_powerXY >= 0.95 * total_powerXY, 1);

% Extract frequencies
f50X = Fx(f50x_idx);   % Median frequency
f50Y = Fy(f50y_idx);   % Median frequency
f50XY = Fxy(f50xy_idx);   % Median frequency

f95X = Fx(f95x_idx);   % 95% power frequency
f95Y = Fy(f95y_idx);
f95XY = Fxy(f95xy_idx);

Area95 = 2*pi*((N-1)/(N-2))*f95XY*sqrt(COPx_RMS^2 * COPy_RMS^2 - COV^2);


