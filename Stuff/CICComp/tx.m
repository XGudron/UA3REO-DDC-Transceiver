%% ------------------------------------------------------------------------
%
% Title       : test_cic.m
% Author      : Alexander Kapitanov
% E-mail      : sallador@bk.ru 
% Version     : 1.0	 
%
% -------------------------------------------------------------------------
%
% Description : 
% 
%    FIR filter compensator to correct freq response after CIC filter.
%
% -------------------------------------------------------------------------
%
% Version     : 1.0 
% Date        : 2017.06.03 
%
%% ------------------------------------------------------------------------
%
%	GNU GENERAL PUBLIC LICENSE
% Version 3, 29 June 2007
%
%	Copyright (c) 2018 Kapitanov Alexander
%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% You should have received a copy of the GNU General Public License
% along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
% THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
% APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT 
% HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY 
% OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, 
% THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
% PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM 
% IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF 
%  ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 
%
%% ------------------------------------------------------------------------
  
set(0, 'DefaultAxesFontSize', 11, 'DefaultAxesFontName', 'Times New Roman');
set(0, 'DefaultTextFontSize', 11, 'DefaultTextFontName', 'Times New Roman'); 

close all;
clear all;

%% ------------------------------------------------------------------------
%  ---- CIC Filter Parameters
%  ------------------------------------------------------------------------

R = 3920;         % Decimation factor
N = 3;         % Number of stages
M = 1;         % Differential delay (only 1)

%% ------------------------------------------------------------------------
%  ---- FIR filter parameters
%  ------------------------------------------------------------------------

NFIR  = 256;     % Filter order, must be odd when Fo = 0.5 !!!
Bc    = 32;     % Coef. Bit-width
Fo    = 0.45;    % Normalized Cutoff: 0.2 < Fo < 0.5;
BETA  = 8;      % BETA parameter for Kaiser window (if IS_WIND = 'Y') 

%% ------------------------------------------------------------------------
%  ---- Save data parameters
%  ------------------------------------------------------------------------

IS_COE      = 'Y'; % create *.COE Xilinx file
IS_HDR      = 'N'; % create *.H file (header)
IS_WIND     = 'Y'; % use Kaiser Window for FIR corrector

IS_PLOT_IDL = 'Y'; % plot ideal response
IS_PLOT_FIR = 'Y'; % plot FIR filter IR
IS_PLOT_ERR = 'Y'; % plot total error response in passband

%% ------------------------------------------------------------------------
%  ---- CIC Compensator Design
%  ------------------------------------------------------------------------

NFFT  = 2^16;  % FFT points for Freq Response (spectrum)
STEP = 1/NFFT;   % Step size
w = -pi:2*pi/NFFT:pi-2*pi/NFFT;
ff = 0:1/NFFT:1-1/NFFT;
z = exp(1j * w);

Fc = 1/(2*R);
Fr = Fo/R;

% 1 way:
%HCIC = (1/R * (1-z.^(-R*M))./(1-z.^(-1))).^ N;
% 2 way:
HCIC = (R^-N*abs(1*M*sin(pi*M*R*ff) ./ sin(pi*ff)).^N);
HCICdb = 20 * log10(abs(HCIC));

fp = [0:STEP:Fo]; % Pass band frequency dots
fs = [(Fo+STEP):STEP:0.5]; % Stop band frequency dots
f = [fp fs]*2; % Normalized frequency dots
f(end) = 1;

% Calculate ideal response
Mp = ones(1, length(fp)); % Pass band response; Mp(1) = 1
Mp(2:end) = abs(M * R * sin(pi*fp(2:end)/R) ./ sin(pi*M*fp(2:end))).^(N);
Mf = [Mp zeros(1, length(fs))];

if (IS_PLOT_IDL == 'Y') 
  figure('name','FIR Ideal Response', 'Numbertitle', 'off')
    plot(f/2, Mf, '-.', 'LineWidth', 2, 'Color',[0 0 1]);

    title([{'FIR Ideal Response'};{sprintf('Fo = %i',Fo)}]);
    xlabel ('Freq (\pi x rad / samples)');
    ylabel ('Magnitude');  
    axis tight; 
    legend([{sprintf('Fo = %i',Fo)}]);  
    grid on;  
end

% Calculate FIR
hFIR = fir2(NFIR-1, f, Mf); % Filter length NFIR
hFIR = hFIR / max(hFIR); % Double coefficients
hCOE = round(hFIR*(2^(Bc-1)-1)); % Fixed point coefficients

% Windowed FIR (Kaiser with BETA) 
if (IS_WIND == 'Y')
  WIND = kaiser(NFIR, BETA); % KAISER WINDOW IS USED!
  hWIND = fir1(NFIR-1, Fo/R, 'low', WIND);
  hNEW = hCOE .* hWIND;% conv2(hCOE,Hwind);
  hCOE = hNEW;
end

if (IS_PLOT_FIR == 'Y')   
  figure('name','FIR Response', 'Numbertitle', 'off')
    plot(hFIR, '-', 'LineWidth', 2, 'Color',[1 0 0]);

    title([{'FIR Response'};{sprintf('Order = %i',NFIR)}]);
    xlabel ('Samples');
    ylabel ('Magnitude');  
    axis tight; 
    legend([{sprintf('Order = %i',NFIR)}]);    
    grid on;   
end

hFFT = 20 * log10(abs(fft(hCOE, ceil(NFFT/R))));
hFFT = hFFT - max(hFFT);

H_amp = repmat(hFFT, 1, R);
H_amp = H_amp(1:length(ff));
H_amp = H_amp - max(H_amp);

H_comp = HCICdb + H_amp;
H_comp = H_comp - max(H_comp);

%% ------------------------------------------------------------------------
%  ---- Plot results 
%  ------------------------------------------------------------------------

% ---- Figure #1
figure('name','CIC/FIR Frequency Response', 'Numbertitle', 'off')
  plot(ff, HCICdb - max(HCICdb), '-.', 'LineWidth', 2, 'Color',[0 0 1]);
  hold on;

  plot(ff, H_amp, '--', 'LineWidth', 2, 'Color',[0 0.4 0]);
  hold on;
  
  plot(ff, H_comp, '-', 'LineWidth', 2, 'Color',[1 0 0]);
  hold on;  
  
  title([{'CIC, Comp. FIR and Result'};{sprintf('Filter Order = %i, Coef. width = %i',NFIR,Bc)}]);
  xlabel ('Freq (\pi x rad / samples)');
  ylabel ('Magnitude (dB)');  
  axis([0 ff(NFFT)/2 -100 5]); 
  line([Fr Fr], [-400 200], 'LineWidth', 1, 'linestyle', '--', 'Color', [0 0 0]);  
  line([Fc Fc], [-400 200], 'LineWidth', 1, 'linestyle', '--', 'Color', [0 0 0]);
  line([2*Fc 2*Fc], [-400 200], 'LineWidth', 1, 'linestyle', '--', 'Color', [0 0 0]);  
  legend('CIC filter','Comp. FIR','Sum Response','location','northeast');
  grid on;  
  
% ---- Figure #2
figure('name', 'CIC/FIR Frequency Response (Zoom)', 'Numbertitle', 'off')
  plot(ff, HCICdb - max(HCICdb), '-.', 'LineWidth', 2, 'Color',[0 0 1]); 
  hold on;

  plot(ff, H_amp, '--', 'LineWidth', 2, 'Color',[0 0.4 0]); 
  hold on;
  
  plot(ff, H_comp, '-', 'LineWidth', 2, 'Color',[1 0 0]);
  hold on; 

  title([{'CIC, Comp. FIR and Result'};{sprintf('Filter Order = %i, Coef. width = %i',NFIR,Bc)}]);
  xlabel ('Freq (\pi x rad / samples)');
  ylabel ('Magnitude (dB)'); 
  axis([0 ff(NFFT)/(2*R) -7 1]);    
  line([Fr Fr], [-400 200], 'LineWidth', 1, 'linestyle', '--', 'Color', [0 0 0]);  
  grid on;  
  
%% ------------------------------------------------------------------------
% ---- Passband irregularity
%  ------------------------------------------------------------------------  
if (IS_PLOT_ERR == 'Y')  
  pass = ceil(0.85*length(H_comp)*Fr);
  stp = 0:(0.9*Fr)/pass:(0.9*Fr)-(0.9*Fr)/pass;
  stp = stp(2:pass);
  Irr = H_comp(2:pass);
  Ism = mean(Irr);
  Iav = (max(Irr)-min(Irr))/2;
  
  figure('name', 'Passband irregularity', 'Numbertitle', 'off')
    plot(stp, Irr, '--', 'LineWidth', 2, 'Color',[0 0 1]); 
    line([0 (0.9*Fr)], [Ism Ism], 'LineWidth', 2, 'linestyle', '-', 'Color', [0 0 0]);   
    grid on;  
    title([{'Passband irregularity'};{sprintf('Mean value = %f',Ism)};{sprintf('Freq error = %f (dB)',Iav)}]);
    xlabel ('Freq (\pi x rad / samples)');
    ylabel ('Magnitude (dB)'); 
    axis tight;    
    %axis([0 ff(NFFT)/(2*R) -0.05 1]);
end

%% ------------------------------------------------------------------------
%  ---- Save coe data to files
%  ------------------------------------------------------------------------  
if (IS_COE == 'Y') 
  fid = fopen ('tx.coe', 'w');
  fprintf(fid, 'Radix = 10;\n');
  fprintf(fid, 'Coefficient_Width = %d;\n', Bc);
  fprintf(fid, 'Coefdata =\n');
  for i = 1:NFIR
    if (i == NFIR)
      fprintf(fid, '%d;\n', hCOE(1,i));
    else
      fprintf(fid, '%d,\n', hCOE(1,i));
    end
  end
  fclose(fid);
end

if (IS_HDR == 'Y') 
  fid = fopen ('fir_filter.h', 'w');
  fprintf(fid, 'const int BL = %d;\n',  NFIR);
  fprintf(fid, 'const int B[%d] = {\n', NFIR);
  for i = 1:NFIR
    if (i == NFIR)
      fprintf(fid, '%d}\n', hCOE(1,i));
    else
      fprintf(fid, '%d,\n', hCOE(1,i));
    end
  end
  fclose(fid);
end
  
