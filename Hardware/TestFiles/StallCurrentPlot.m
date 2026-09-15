%% driveWire Motor Test Plot
clear;
clc;
close all;

%% Load telemetry log
filename = "StallCurrentFullTest.txt";

text = fileread(filename);

%% extract telemetry values

% battery voltage
voltageTokens = regexp(text, ...
    'Battery voltage:\s*([\d.]+)\s*V', ...
    'tokens');

% battery current
currentTokens = regexp(text, ...
    'Battery current:\s*([\d.]+)\s*mA', ...
    'tokens');

% distance
distanceTokens = regexp(text, ...
    'Distance in front:\s*([\d.]+)\s*mm', ...
    'tokens');

% convert extracted strings to numeric arrays
batteryVoltage = cellfun(@(x) str2double(x{1}), voltageTokens);
batteryCurrent = cellfun(@(x) str2double(x{1}), currentTokens);
distance = cellfun(@(x) str2double(x{1}), distanceTokens);


%% time axis

% sensorTask runs every 500 ms
samplePeriod = 0.5;      % seconds

numSamples = length(batteryVoltage);

time = (0:numSamples-1) * samplePeriod;


%% plot voltage and current

figure;

yyaxis left

plot(time, batteryVoltage, 'LineWidth', 1.8);

ylabel('Battery Voltage (V)');
ylim([6.2 7.9]);


yyaxis right

plot(time, batteryCurrent, '--', 'LineWidth', 1.8);

ylabel('Battery Current (mA)');
ylim([0 1400]);


%% labels and format

xlabel('Elapsed Time (s)');

title('DriveWire Motor Test: Battery Voltage and Current vs Time');

grid on;

legend('Battery Voltage', ...
    'Battery Current', ...
    'Location', 'best');

set(gca, 'FontSize', 11);