% close all 
% clear
% clc

%% Parametros de las graficas  - CAMBIA DE VALOR

% datos
num_datos_ghs     =  24; % 16 datos 8 check rate - 24 datos 128 check rate; % Numero de datos. Es igual al numero de lineas del archivo nodo_1.txt
num_datos_ghs    =  23; % 59 datos 8 check rate - 23 datos 128 check rate % Numero de datos. Es igual al numero de lineas del archivo nodo_1.txt
% Network
num_nodos     = 9; % Numero de nodos en la simulacion de Cooja
intervalo     = 10; %Intervalo de powertrace. Contiki "powertrace_start(CLOCK_SECOND * 10)"

%% Parametros de las graficas  - CONSTANTES

% Valores para el CM5000
Voltage = 3; % Voltaje en Volts (V)
CPU_mA  = 0.33; % Consumo de corriente (mA) de la CPU
LPM_mA  = 0.0011; % Consumo de corriente (mA) cuando RF esta en Low Power Mode (LPM)
TX_mA   = 17.4; % Consumo de corriente (mA) del transceiver cuando TX
RX_mA   = 18.8; % Consumo de corriente (mA) del transceiver cuando RX

% Network
rtimer_second = 32768; % Es el numero de ticks per second, dado por la variable RTIMER_SECOND de Contiki 


%% Definiciones para que funcione el codigo de matlab
max_eje_x_ghs = intervalo * (num_datos_ghs - 1); % Resto -1 porque diff elimina 1 valor. 
eje_x_ghs     = [intervalo:intervalo:max_eje_x_ghs]; % Crea el eje x. De 10 en 10. 

max_eje_x_ghs = intervalo * (num_datos_ghs - 1); % Resto -1 porque diff elimina 1 valor. 
eje_x_ghs     = [intervalo:intervalo:max_eje_x_ghs]; % Crea el eje x. De 10 en 10. 

color0 = [0 0 0] ;
color1 = [0.2 0.2 0.6] ;
color2 = [0.2 0.6 0.2];
color3 = [0.6 0.2 0.2];
color4 = [0.4 0.3 0.3];


%% Leer archivos 

%archivos ghs
nodo_ghs_ATC_1_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_1.dat';
nodo_ghs_ATC_2_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_2.dat';
nodo_ghs_ATC_3_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_3.dat';
nodo_ghs_ATC_4_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_4.dat';
nodo_ghs_ATC_5_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_5.dat';
nodo_ghs_ATC_6_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_6.dat';
nodo_ghs_ATC_7_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_7.dat';
nodo_ghs_ATC_8_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_8.dat';
nodo_ghs_ATC_9_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_9.dat';
nodo_ghs_ATC_10_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_10.dat';
nodo_ghs_ATC_11_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_11.dat';
nodo_ghs_ATC_12_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_12.dat';
nodo_ghs_ATC_13_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_13.dat';
nodo_ghs_ATC_14_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_14.dat';
nodo_ghs_ATC_15_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_15.dat';
nodo_ghs_ATC_16_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_16.dat';
nodo_ghs_ATC_17_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_17.dat';
nodo_ghs_ATC_18_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_18.dat';
nodo_ghs_ATC_19_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_19.dat';
nodo_ghs_ATC_20_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_20.dat';
nodo_ghs_ATC_21_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_21.dat';
nodo_ghs_ATC_22_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_22.dat';
nodo_ghs_ATC_23_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_23.dat';
nodo_ghs_ATC_24_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_24.dat';
nodo_ghs_ATC_25_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_25.dat';
nodo_ghs_ATC_26_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_26.dat';
nodo_ghs_ATC_27_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_27.dat';
nodo_ghs_ATC_28_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_28.dat';
nodo_ghs_ATC_29_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_29.dat';
nodo_ghs_ATC_30_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_30.dat';
nodo_ghs_ATC_31_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_31.dat';
nodo_ghs_ATC_32_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_32.dat';
nodo_ghs_ATC_33_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_33.dat';
nodo_ghs_ATC_34_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_34.dat';
nodo_ghs_ATC_35_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_35.dat';
nodo_ghs_ATC_36_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_36.dat';
nodo_ghs_ATC_37_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_37.dat';
nodo_ghs_ATC_38_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_38.dat';
nodo_ghs_ATC_39_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_39.dat';
nodo_ghs_ATC_40_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_40.dat';

%archivos ghs
%nodo_ghs_ATC_1_file = '/home/sergiodiaz/Desktop/ContikiGHS_v14/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_ATC_1.dat';


%Extraer la informacion del archivo - ghs
nodo_ghs_ATC_1  = csvread(nodo_ghs_ATC_1_file);
nodo_ghs_ATC_2  = csvread(nodo_ghs_ATC_2_file);
nodo_ghs_ATC_3  = csvread(nodo_ghs_ATC_3_file);
nodo_ghs_ATC_4  = csvread(nodo_ghs_ATC_4_file);
nodo_ghs_ATC_5  = csvread(nodo_ghs_ATC_5_file);
nodo_ghs_ATC_6  = csvread(nodo_ghs_ATC_6_file);
nodo_ghs_ATC_7  = csvread(nodo_ghs_ATC_7_file);
nodo_ghs_ATC_8  = csvread(nodo_ghs_ATC_8_file);
nodo_ghs_ATC_9  = csvread(nodo_ghs_ATC_9_file);
nodo_ghs_ATC_10  = csvread(nodo_ghs_ATC_10_file);
nodo_ghs_ATC_11  = csvread(nodo_ghs_ATC_11_file);
nodo_ghs_ATC_12  = csvread(nodo_ghs_ATC_12_file);
nodo_ghs_ATC_13  = csvread(nodo_ghs_ATC_13_file);
nodo_ghs_ATC_14  = csvread(nodo_ghs_ATC_14_file);
nodo_ghs_ATC_15  = csvread(nodo_ghs_ATC_15_file);
nodo_ghs_ATC_16  = csvread(nodo_ghs_ATC_16_file);
nodo_ghs_ATC_17  = csvread(nodo_ghs_ATC_17_file);
nodo_ghs_ATC_18  = csvread(nodo_ghs_ATC_18_file);
nodo_ghs_ATC_19  = csvread(nodo_ghs_ATC_19_file);
nodo_ghs_ATC_20  = csvread(nodo_ghs_ATC_20_file);
nodo_ghs_ATC_21  = csvread(nodo_ghs_ATC_21_file);
nodo_ghs_ATC_22  = csvread(nodo_ghs_ATC_22_file);
nodo_ghs_ATC_23  = csvread(nodo_ghs_ATC_23_file);
nodo_ghs_ATC_24  = csvread(nodo_ghs_ATC_24_file);
nodo_ghs_ATC_25  = csvread(nodo_ghs_ATC_25_file);
nodo_ghs_ATC_26  = csvread(nodo_ghs_ATC_26_file);
nodo_ghs_ATC_27  = csvread(nodo_ghs_ATC_27_file);
nodo_ghs_ATC_28  = csvread(nodo_ghs_ATC_28_file);
nodo_ghs_ATC_29  = csvread(nodo_ghs_ATC_29_file);
nodo_ghs_ATC_30  = csvread(nodo_ghs_ATC_30_file);
nodo_ghs_ATC_31  = csvread(nodo_ghs_ATC_31_file);
nodo_ghs_ATC_32  = csvread(nodo_ghs_ATC_32_file);
nodo_ghs_ATC_33  = csvread(nodo_ghs_ATC_33_file);
nodo_ghs_ATC_34  = csvread(nodo_ghs_ATC_34_file);
nodo_ghs_ATC_35  = csvread(nodo_ghs_ATC_35_file);
nodo_ghs_ATC_36  = csvread(nodo_ghs_ATC_36_file);
nodo_ghs_ATC_37  = csvread(nodo_ghs_ATC_37_file);
nodo_ghs_ATC_38  = csvread(nodo_ghs_ATC_38_file);
nodo_ghs_ATC_39  = csvread(nodo_ghs_ATC_39_file);
nodo_ghs_ATC_40  = csvread(nodo_ghs_ATC_40_file);

% %Extraer la informacion del archivo - ghs
% nodo_ghs_ATC_1  = csvread(nodo_ghs_ATC_1_file);

% Guardar valores individuales - ghs
CPU_ghs_val_1 = nodo_ghs_ATC_1(:,2); % Valores de la CPU
LPM_ghs_val_1 = nodo_ghs_ATC_1(:,3); % Valores de la CPU
TX_ghs_val_1  = nodo_ghs_ATC_1(:,4); % Valores de la CPU
RX_ghs_val_1 = nodo_ghs_ATC_1(:,5); % Valores de la CPU

CPU_ghs_val_2 = nodo_ghs_ATC_2(:,2); % Valores de la CPU
LPM_ghs_val_2 = nodo_ghs_ATC_2(:,3); % Valores de la CPU
TX_ghs_val_2  = nodo_ghs_ATC_2(:,4); % Valores de la CPU
RX_ghs_val_2  = nodo_ghs_ATC_2(:,5); % Valores de la CPU

CPU_ghs_val_3 = nodo_ghs_ATC_3(:,2); % Valores de la CPU
LPM_ghs_val_3 = nodo_ghs_ATC_3(:,3); % Valores de la CPU
TX_ghs_val_3  = nodo_ghs_ATC_3(:,4); % Valores de la CPU
RX_ghs_val_3  = nodo_ghs_ATC_3(:,5); % Valores de la CPU

CPU_ghs_val_4 = nodo_ghs_ATC_4(:,2); % Valores de la CPU
LPM_ghs_val_4 = nodo_ghs_ATC_4(:,3); % Valores de la CPU
TX_ghs_val_4  = nodo_ghs_ATC_4(:,4); % Valores de la CPU
RX_ghs_val_4  = nodo_ghs_ATC_4(:,5); % Valores de la CPU

CPU_ghs_val_5 = nodo_ghs_ATC_5(:,2); % Valores de la CPU
LPM_ghs_val_5 = nodo_ghs_ATC_5(:,3); % Valores de la CPU
TX_ghs_val_5  = nodo_ghs_ATC_5(:,4); % Valores de la CPU
RX_ghs_val_5  = nodo_ghs_ATC_5(:,5); % Valores de la CPU

CPU_ghs_val_6 = nodo_ghs_ATC_6(:,2); % Valores de la CPU
LPM_ghs_val_6 = nodo_ghs_ATC_6(:,3); % Valores de la CPU
TX_ghs_val_6  = nodo_ghs_ATC_6(:,4); % Valores de la CPU
RX_ghs_val_6  = nodo_ghs_ATC_6(:,5); % Valores de la CPU

CPU_ghs_val_7 = nodo_ghs_ATC_7(:,2); % Valores de la CPU
LPM_ghs_val_7 = nodo_ghs_ATC_7(:,3); % Valores de la CPU
TX_ghs_val_7  = nodo_ghs_ATC_7(:,4); % Valores de la CPU
RX_ghs_val_7 = nodo_ghs_ATC_7(:,5); % Valores de la CPU

CPU_ghs_val_8 = nodo_ghs_ATC_8(:,2); % Valores de la CPU
LPM_ghs_val_8 = nodo_ghs_ATC_8(:,3); % Valores de la CPU
TX_ghs_val_8  = nodo_ghs_ATC_8(:,4); % Valores de la CPU
RX_ghs_val_8  = nodo_ghs_ATC_8(:,5); % Valores de la CPU

CPU_ghs_val_9 = nodo_ghs_ATC_9(:,2); % Valores de la CPU
LPM_ghs_val_9 = nodo_ghs_ATC_9(:,3); % Valores de la CPU
TX_ghs_val_9  = nodo_ghs_ATC_9(:,4); % Valores de la CPU
RX_ghs_val_9  = nodo_ghs_ATC_9(:,5); % Valores de la CPU

CPU_ghs_val_10 = nodo_ghs_ATC_10(:,2); % Valores de la CPU
LPM_ghs_val_10 = nodo_ghs_ATC_10(:,3); % Valores de la CPU
TX_ghs_val_10  = nodo_ghs_ATC_10(:,4); % Valores de la CPU
RX_ghs_val_10  = nodo_ghs_ATC_10(:,5); % Valores de la CPU

CPU_ghs_val_11 = nodo_ghs_ATC_11(:,2); % Valores de la CPU
LPM_ghs_val_11 = nodo_ghs_ATC_11(:,3); % Valores de la CPU
TX_ghs_val_11  = nodo_ghs_ATC_11(:,4); % Valores de la CPU
RX_ghs_val_11  = nodo_ghs_ATC_11(:,5); % Valores de la CPU

CPU_ghs_val_12 = nodo_ghs_ATC_12(:,2); % Valores de la CPU
LPM_ghs_val_12 = nodo_ghs_ATC_12(:,3); % Valores de la CPU
TX_ghs_val_12  = nodo_ghs_ATC_12(:,4); % Valores de la CPU
RX_ghs_val_12  = nodo_ghs_ATC_12(:,5); % Valores de la CPU

CPU_ghs_val_13 = nodo_ghs_ATC_13(:,2); % Valores de la CPU
LPM_ghs_val_13 = nodo_ghs_ATC_13(:,3); % Valores de la CPU
TX_ghs_val_13  = nodo_ghs_ATC_13(:,4); % Valores de la CPU
RX_ghs_val_13  = nodo_ghs_ATC_13(:,5); % Valores de la CPU

CPU_ghs_val_14 = nodo_ghs_ATC_14(:,2); % Valores de la CPU
LPM_ghs_val_14 = nodo_ghs_ATC_14(:,3); % Valores de la CPU
TX_ghs_val_14  = nodo_ghs_ATC_14(:,4); % Valores de la CPU
RX_ghs_val_14  = nodo_ghs_ATC_14(:,5); % Valores de la CPU

CPU_ghs_val_15 = nodo_ghs_ATC_15(:,2); % Valores de la CPU
LPM_ghs_val_15 = nodo_ghs_ATC_15(:,3); % Valores de la CPU
TX_ghs_val_15  = nodo_ghs_ATC_15(:,4); % Valores de la CPU
RX_ghs_val_15  = nodo_ghs_ATC_15(:,5); % Valores de la CPU

CPU_ghs_val_16 = nodo_ghs_ATC_16(:,2); % Valores de la CPU
LPM_ghs_val_16 = nodo_ghs_ATC_16(:,3); % Valores de la CPU
TX_ghs_val_16  = nodo_ghs_ATC_16(:,4); % Valores de la CPU
RX_ghs_val_16  = nodo_ghs_ATC_16(:,5); % Valores de la CPU

CPU_ghs_val_17 = nodo_ghs_ATC_17(:,2); % Valores de la CPU
LPM_ghs_val_17 = nodo_ghs_ATC_17(:,3); % Valores de la CPU
TX_ghs_val_17  = nodo_ghs_ATC_17(:,4); % Valores de la CPU
RX_ghs_val_17  = nodo_ghs_ATC_17(:,5); % Valores de la CPU

CPU_ghs_val_18 = nodo_ghs_ATC_18(:,2); % Valores de la CPU
LPM_ghs_val_18 = nodo_ghs_ATC_18(:,3); % Valores de la CPU
TX_ghs_val_18  = nodo_ghs_ATC_18(:,4); % Valores de la CPU
RX_ghs_val_18  = nodo_ghs_ATC_18(:,5); % Valores de la CPU

CPU_ghs_val_19 = nodo_ghs_ATC_19(:,2); % Valores de la CPU
LPM_ghs_val_19 = nodo_ghs_ATC_19(:,3); % Valores de la CPU
TX_ghs_val_19  = nodo_ghs_ATC_19(:,4); % Valores de la CPU
RX_ghs_val_19  = nodo_ghs_ATC_19(:,5); % Valores de la CPU

CPU_ghs_val_20 = nodo_ghs_ATC_20(:,2); % Valores de la CPU
LPM_ghs_val_20 = nodo_ghs_ATC_20(:,3); % Valores de la CPU
TX_ghs_val_20  = nodo_ghs_ATC_20(:,4); % Valores de la CPU
RX_ghs_val_20  = nodo_ghs_ATC_20(:,5); % Valores de la CPU

CPU_ghs_val_21 = nodo_ghs_ATC_21(:,2); % Valores de la CPU
LPM_ghs_val_21 = nodo_ghs_ATC_21(:,3); % Valores de la CPU
TX_ghs_val_21  = nodo_ghs_ATC_21(:,4); % Valores de la CPU
RX_ghs_val_21  = nodo_ghs_ATC_21(:,5); % Valores de la CPU

CPU_ghs_val_22 = nodo_ghs_ATC_22(:,2); % Valores de la CPU
LPM_ghs_val_22 = nodo_ghs_ATC_22(:,3); % Valores de la CPU
TX_ghs_val_22  = nodo_ghs_ATC_22(:,4); % Valores de la CPU
RX_ghs_val_22  = nodo_ghs_ATC_22(:,5); % Valores de la CPU

CPU_ghs_val_23 = nodo_ghs_ATC_23(:,2); % Valores de la CPU
LPM_ghs_val_23 = nodo_ghs_ATC_23(:,3); % Valores de la CPU
TX_ghs_val_23  = nodo_ghs_ATC_23(:,4); % Valores de la CPU
RX_ghs_val_23  = nodo_ghs_ATC_23(:,5); % Valores de la CPU

CPU_ghs_val_24 = nodo_ghs_ATC_24(:,2); % Valores de la CPU
LPM_ghs_val_24 = nodo_ghs_ATC_24(:,3); % Valores de la CPU
TX_ghs_val_24  = nodo_ghs_ATC_24(:,4); % Valores de la CPU
RX_ghs_val_24  = nodo_ghs_ATC_24(:,5); % Valores de la CPU

CPU_ghs_val_25 = nodo_ghs_ATC_25(:,2); % Valores de la CPU
LPM_ghs_val_25 = nodo_ghs_ATC_25(:,3); % Valores de la CPU
TX_ghs_val_25  = nodo_ghs_ATC_25(:,4); % Valores de la CPU
RX_ghs_val_25  = nodo_ghs_ATC_25(:,5); % Valores de la CPU

CPU_ghs_val_26 = nodo_ghs_ATC_26(:,2); % Valores de la CPU
LPM_ghs_val_26 = nodo_ghs_ATC_26(:,3); % Valores de la CPU
TX_ghs_val_26  = nodo_ghs_ATC_26(:,4); % Valores de la CPU
RX_ghs_val_26  = nodo_ghs_ATC_26(:,5); % Valores de la CPU

CPU_ghs_val_27 = nodo_ghs_ATC_27(:,2); % Valores de la CPU
LPM_ghs_val_27 = nodo_ghs_ATC_27(:,3); % Valores de la CPU
TX_ghs_val_27  = nodo_ghs_ATC_27(:,4); % Valores de la CPU
RX_ghs_val_27  = nodo_ghs_ATC_27(:,5); % Valores de la CPU

CPU_ghs_val_28 = nodo_ghs_ATC_28(:,2); % Valores de la CPU
LPM_ghs_val_28 = nodo_ghs_ATC_28(:,3); % Valores de la CPU
TX_ghs_val_28  = nodo_ghs_ATC_28(:,4); % Valores de la CPU
RX_ghs_val_28  = nodo_ghs_ATC_28(:,5); % Valores de la CPU

CPU_ghs_val_29 = nodo_ghs_ATC_29(:,2); % Valores de la CPU
LPM_ghs_val_29 = nodo_ghs_ATC_29(:,3); % Valores de la CPU
TX_ghs_val_29  = nodo_ghs_ATC_29(:,4); % Valores de la CPU
RX_ghs_val_29  = nodo_ghs_ATC_29(:,5); % Valores de la CPU

CPU_ghs_val_30 = nodo_ghs_ATC_30(:,2); % Valores de la CPU
LPM_ghs_val_30 = nodo_ghs_ATC_30(:,3); % Valores de la CPU
TX_ghs_val_30  = nodo_ghs_ATC_30(:,4); % Valores de la CPU
RX_ghs_val_30  = nodo_ghs_ATC_30(:,5); % Valores de la CPU

CPU_ghs_val_31 = nodo_ghs_ATC_31(:,2); % Valores de la CPU
LPM_ghs_val_31 = nodo_ghs_ATC_31(:,3); % Valores de la CPU
TX_ghs_val_31  = nodo_ghs_ATC_31(:,4); % Valores de la CPU
RX_ghs_val_31  = nodo_ghs_ATC_31(:,5); % Valores de la CPU

CPU_ghs_val_32 = nodo_ghs_ATC_32(:,2); % Valores de la CPU
LPM_ghs_val_32 = nodo_ghs_ATC_32(:,3); % Valores de la CPU
TX_ghs_val_32  = nodo_ghs_ATC_32(:,4); % Valores de la CPU
RX_ghs_val_32  = nodo_ghs_ATC_32(:,5); % Valores de la CPU

CPU_ghs_val_33 = nodo_ghs_ATC_33(:,2); % Valores de la CPU
LPM_ghs_val_33 = nodo_ghs_ATC_33(:,3); % Valores de la CPU
TX_ghs_val_33  = nodo_ghs_ATC_33(:,4); % Valores de la CPU
RX_ghs_val_33  = nodo_ghs_ATC_33(:,5); % Valores de la CPU

CPU_ghs_val_34 = nodo_ghs_ATC_34(:,2); % Valores de la CPU
LPM_ghs_val_34 = nodo_ghs_ATC_34(:,3); % Valores de la CPU
TX_ghs_val_34  = nodo_ghs_ATC_34(:,4); % Valores de la CPU
RX_ghs_val_34  = nodo_ghs_ATC_34(:,5); % Valores de la CPU

CPU_ghs_val_35 = nodo_ghs_ATC_35(:,2); % Valores de la CPU
LPM_ghs_val_35 = nodo_ghs_ATC_35(:,3); % Valores de la CPU
TX_ghs_val_35  = nodo_ghs_ATC_35(:,4); % Valores de la CPU
RX_ghs_val_35  = nodo_ghs_ATC_35(:,5); % Valores de la CPU

CPU_ghs_val_36 = nodo_ghs_ATC_36(:,2); % Valores de la CPU
LPM_ghs_val_36 = nodo_ghs_ATC_36(:,3); % Valores de la CPU
TX_ghs_val_36  = nodo_ghs_ATC_36(:,4); % Valores de la CPU
RX_ghs_val_36  = nodo_ghs_ATC_36(:,5); % Valores de la CPU

CPU_ghs_val_37 = nodo_ghs_ATC_37(:,2); % Valores de la CPU
LPM_ghs_val_37 = nodo_ghs_ATC_37(:,3); % Valores de la CPU
TX_ghs_val_37  = nodo_ghs_ATC_37(:,4); % Valores de la CPU
RX_ghs_val_37  = nodo_ghs_ATC_37(:,5); % Valores de la CPU

CPU_ghs_val_38 = nodo_ghs_ATC_38(:,2); % Valores de la CPU
LPM_ghs_val_38 = nodo_ghs_ATC_38(:,3); % Valores de la CPU
TX_ghs_val_38  = nodo_ghs_ATC_38(:,4); % Valores de la CPU
RX_ghs_val_38  = nodo_ghs_ATC_38(:,5); % Valores de la CPU

CPU_ghs_val_39 = nodo_ghs_ATC_39(:,2); % Valores de la CPU
LPM_ghs_val_39 = nodo_ghs_ATC_39(:,3); % Valores de la CPU
TX_ghs_val_39  = nodo_ghs_ATC_39(:,4); % Valores de la CPU
RX_ghs_val_39  = nodo_ghs_ATC_39(:,5); % Valores de la CPU

CPU_ghs_val_40 = nodo_ghs_ATC_40(:,2); % Valores de la CPU
LPM_ghs_val_40 = nodo_ghs_ATC_40(:,3); % Valores de la CPU
TX_ghs_val_40  = nodo_ghs_ATC_40(:,4); % Valores de la CPU
RX_ghs_val_40  = nodo_ghs_ATC_40(:,5); % Valores de la CPU

% % Guardar valores individuales - ghs
% CPU_ghs_val = nodo_ghs_ATC_1(:,2); % Valores de la CPU
% LPM_ghs_val = nodo_ghs_ATC_1(:,3); % Valores de la CPU
% TX_ghs_val  = nodo_ghs_ATC_1(:,4); % Valores de la CPU
% RX_ghs_val  = nodo_ghs_ATC_1(:,5); % Valores de la CPU

%% Calculo de la potencia en (mW)

% Calculo de la potencia - ghs
CPU_ghs_mW_1  =  diff(CPU_ghs_val_1) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_1 =  diff(LPM_ghs_val_1) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_1  =  diff(TX_ghs_val_1)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_1  =  diff(RX_ghs_val_1)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_1 = CPU_ghs_mW_1 + LPM_ghs_mW_1 + TX_ghs_mW_1 + RX_ghs_mW_1;

CPU_ghs_mW_2  =  diff(CPU_ghs_val_2) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_2 =  diff(LPM_ghs_val_2) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_2  =  diff(TX_ghs_val_2)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_2  =  diff(RX_ghs_val_2)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_2 = CPU_ghs_mW_2 + LPM_ghs_mW_2 + TX_ghs_mW_2 + RX_ghs_mW_2;

CPU_ghs_mW_3  =  diff(CPU_ghs_val_3) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_3 =  diff(LPM_ghs_val_3) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_3  =  diff(TX_ghs_val_3)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_3  =  diff(RX_ghs_val_3)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_3 = CPU_ghs_mW_3 + LPM_ghs_mW_3 + TX_ghs_mW_3 + RX_ghs_mW_3;

CPU_ghs_mW_4  =  diff(CPU_ghs_val_4) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_4 =  diff(LPM_ghs_val_4) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_4  =  diff(TX_ghs_val_4)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_4  =  diff(RX_ghs_val_4)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_4 = CPU_ghs_mW_4 + LPM_ghs_mW_4 + TX_ghs_mW_4 + RX_ghs_mW_4;

CPU_ghs_mW_5  =  diff(CPU_ghs_val_5) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_5 =  diff(LPM_ghs_val_5) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_5  =  diff(TX_ghs_val_5)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_5  =  diff(RX_ghs_val_5)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_5 = CPU_ghs_mW_5 + LPM_ghs_mW_5 + TX_ghs_mW_5 + RX_ghs_mW_5;

CPU_ghs_mW_6  =  diff(CPU_ghs_val_6) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_6 =  diff(LPM_ghs_val_6) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_6  =  diff(TX_ghs_val_6)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_6  =  diff(RX_ghs_val_6)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_6 = CPU_ghs_mW_6 + LPM_ghs_mW_6 + TX_ghs_mW_6 + RX_ghs_mW_6;

CPU_ghs_mW_7  =  diff(CPU_ghs_val_7) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_7 =  diff(LPM_ghs_val_7) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_7  =  diff(TX_ghs_val_7)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_7  =  diff(RX_ghs_val_7)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_7 = CPU_ghs_mW_7 + LPM_ghs_mW_7 + TX_ghs_mW_7 + RX_ghs_mW_7;

CPU_ghs_mW_8  =  diff(CPU_ghs_val_8) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_8 =  diff(LPM_ghs_val_8) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_8  =  diff(TX_ghs_val_8)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_8  =  diff(RX_ghs_val_8)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_8 = CPU_ghs_mW_8 + LPM_ghs_mW_8 + TX_ghs_mW_8 + RX_ghs_mW_8;

CPU_ghs_mW_9  =  diff(CPU_ghs_val_9) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_9 =  diff(LPM_ghs_val_9) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_9  =  diff(TX_ghs_val_9)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_9  =  diff(RX_ghs_val_9)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_9 = CPU_ghs_mW_9 + LPM_ghs_mW_9 + TX_ghs_mW_9 + RX_ghs_mW_9;

CPU_ghs_mW_10  =  diff(CPU_ghs_val_10) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_10 =  diff(LPM_ghs_val_10) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_10  =  diff(TX_ghs_val_10)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_10  =  diff(RX_ghs_val_10)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_10 = CPU_ghs_mW_10 + LPM_ghs_mW_10 + TX_ghs_mW_10 + RX_ghs_mW_10;

CPU_ghs_mW_11  =  diff(CPU_ghs_val_11) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_11 =  diff(LPM_ghs_val_11) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_11  =  diff(TX_ghs_val_11)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_11  =  diff(RX_ghs_val_11)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_11 = CPU_ghs_mW_11 + LPM_ghs_mW_11 + TX_ghs_mW_11 + RX_ghs_mW_11;

CPU_ghs_mW_12  =  diff(CPU_ghs_val_12) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_12 =  diff(LPM_ghs_val_12) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_12  =  diff(TX_ghs_val_12)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_12  =  diff(RX_ghs_val_12)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_12 = CPU_ghs_mW_12 + LPM_ghs_mW_12 + TX_ghs_mW_12 + RX_ghs_mW_12;

CPU_ghs_mW_13  =  diff(CPU_ghs_val_13) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_13 =  diff(LPM_ghs_val_13) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_13  =  diff(TX_ghs_val_13)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_13  =  diff(RX_ghs_val_13)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_13 = CPU_ghs_mW_13 + LPM_ghs_mW_13 + TX_ghs_mW_13 + RX_ghs_mW_13;

CPU_ghs_mW_14  =  diff(CPU_ghs_val_14) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_14 =  diff(LPM_ghs_val_14) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_14  =  diff(TX_ghs_val_14)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_14  =  diff(RX_ghs_val_14)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_14 = CPU_ghs_mW_14 + LPM_ghs_mW_14 + TX_ghs_mW_14 + RX_ghs_mW_14;

CPU_ghs_mW_15  =  diff(CPU_ghs_val_15) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_15 =  diff(LPM_ghs_val_15) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_15  =  diff(TX_ghs_val_15)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_15  =  diff(RX_ghs_val_15)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_15 = CPU_ghs_mW_15 + LPM_ghs_mW_15 + TX_ghs_mW_15 + RX_ghs_mW_15;

CPU_ghs_mW_16  =  diff(CPU_ghs_val_16) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_16 =  diff(LPM_ghs_val_16) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_16  =  diff(TX_ghs_val_16)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_16  =  diff(RX_ghs_val_16)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_16 = CPU_ghs_mW_16 + LPM_ghs_mW_16 + TX_ghs_mW_16 + RX_ghs_mW_16;

CPU_ghs_mW_17  =  diff(CPU_ghs_val_17) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_17 =  diff(LPM_ghs_val_17) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_17  =  diff(TX_ghs_val_17)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_17  =  diff(RX_ghs_val_17)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_17 = CPU_ghs_mW_17 + LPM_ghs_mW_17 + TX_ghs_mW_17 + RX_ghs_mW_17;

CPU_ghs_mW_18  =  diff(CPU_ghs_val_18) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_18 =  diff(LPM_ghs_val_18) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_18  =  diff(TX_ghs_val_18)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_18  =  diff(RX_ghs_val_18)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_18 = CPU_ghs_mW_18 + LPM_ghs_mW_18 + TX_ghs_mW_18 + RX_ghs_mW_18;

CPU_ghs_mW_19  =  diff(CPU_ghs_val_19) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_19 =  diff(LPM_ghs_val_19) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_19  =  diff(TX_ghs_val_19)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_19  =  diff(RX_ghs_val_19)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_19 = CPU_ghs_mW_19 + LPM_ghs_mW_19 + TX_ghs_mW_19 + RX_ghs_mW_19;

CPU_ghs_mW_20  =  diff(CPU_ghs_val_20) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_20 =  diff(LPM_ghs_val_20) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_20  =  diff(TX_ghs_val_20)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_20  =  diff(RX_ghs_val_20)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_20 = CPU_ghs_mW_20 + LPM_ghs_mW_20 + TX_ghs_mW_20 + RX_ghs_mW_20;

CPU_ghs_mW_21  =  diff(CPU_ghs_val_21) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_21 =  diff(LPM_ghs_val_21) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_21  =  diff(TX_ghs_val_21)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_21  =  diff(RX_ghs_val_21)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_21 = CPU_ghs_mW_21 + LPM_ghs_mW_21 + TX_ghs_mW_21 + RX_ghs_mW_21;

CPU_ghs_mW_22  =  diff(CPU_ghs_val_22) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_22 =  diff(LPM_ghs_val_22) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_22  =  diff(TX_ghs_val_22)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_22  =  diff(RX_ghs_val_22)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_22 = CPU_ghs_mW_22 + LPM_ghs_mW_22 + TX_ghs_mW_22 + RX_ghs_mW_22;

CPU_ghs_mW_23  =  diff(CPU_ghs_val_23) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_23 =  diff(LPM_ghs_val_23) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_23  =  diff(TX_ghs_val_23)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_23  =  diff(RX_ghs_val_23)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_23 = CPU_ghs_mW_23 + LPM_ghs_mW_23 + TX_ghs_mW_23 + RX_ghs_mW_23;

CPU_ghs_mW_24  =  diff(CPU_ghs_val_24) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_24 =  diff(LPM_ghs_val_24) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_24  =  diff(TX_ghs_val_24)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_24  =  diff(RX_ghs_val_24)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_24 = CPU_ghs_mW_24 + LPM_ghs_mW_24 + TX_ghs_mW_24 + RX_ghs_mW_24;

CPU_ghs_mW_25  =  diff(CPU_ghs_val_25) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_25 =  diff(LPM_ghs_val_25) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_25  =  diff(TX_ghs_val_25)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_25  =  diff(RX_ghs_val_25)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_25 = CPU_ghs_mW_25 + LPM_ghs_mW_25 + TX_ghs_mW_25 + RX_ghs_mW_25;

CPU_ghs_mW_26  =  diff(CPU_ghs_val_26) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_26 =  diff(LPM_ghs_val_26) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_26  =  diff(TX_ghs_val_26)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_26  =  diff(RX_ghs_val_26)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_26 = CPU_ghs_mW_26 + LPM_ghs_mW_26 + TX_ghs_mW_26 + RX_ghs_mW_26;

CPU_ghs_mW_27  =  diff(CPU_ghs_val_27) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_27 =  diff(LPM_ghs_val_27) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_27  =  diff(TX_ghs_val_27)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_27  =  diff(RX_ghs_val_27)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_27 = CPU_ghs_mW_27 + LPM_ghs_mW_27 + TX_ghs_mW_27 + RX_ghs_mW_27;

CPU_ghs_mW_28  =  diff(CPU_ghs_val_28) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_28 =  diff(LPM_ghs_val_28) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_28  =  diff(TX_ghs_val_28)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_28  =  diff(RX_ghs_val_28)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_28 = CPU_ghs_mW_28 + LPM_ghs_mW_28 + TX_ghs_mW_28 + RX_ghs_mW_28;

CPU_ghs_mW_29  =  diff(CPU_ghs_val_29) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_29 =  diff(LPM_ghs_val_29) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_29  =  diff(TX_ghs_val_29)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_29  =  diff(RX_ghs_val_29)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_29 = CPU_ghs_mW_29 + LPM_ghs_mW_29 + TX_ghs_mW_29 + RX_ghs_mW_29;

CPU_ghs_mW_30  =  diff(CPU_ghs_val_30) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_30 =  diff(LPM_ghs_val_30) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_30  =  diff(TX_ghs_val_30)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_30  =  diff(RX_ghs_val_30)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_30 = CPU_ghs_mW_30 + LPM_ghs_mW_30 + TX_ghs_mW_30 + RX_ghs_mW_30;

CPU_ghs_mW_31  =  diff(CPU_ghs_val_31) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_31 =  diff(LPM_ghs_val_31) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_31  =  diff(TX_ghs_val_31)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_31  =  diff(RX_ghs_val_31)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_31 = CPU_ghs_mW_31 + LPM_ghs_mW_31 + TX_ghs_mW_31 + RX_ghs_mW_31;

CPU_ghs_mW_32  =  diff(CPU_ghs_val_32) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_32 =  diff(LPM_ghs_val_32) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_32  =  diff(TX_ghs_val_32)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_32  =  diff(RX_ghs_val_32)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_32 = CPU_ghs_mW_32 + LPM_ghs_mW_32 + TX_ghs_mW_32 + RX_ghs_mW_32;

CPU_ghs_mW_33  =  diff(CPU_ghs_val_33) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_33 =  diff(LPM_ghs_val_33) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_33  =  diff(TX_ghs_val_33)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_33  =  diff(RX_ghs_val_33)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_33 = CPU_ghs_mW_33 + LPM_ghs_mW_33 + TX_ghs_mW_33 + RX_ghs_mW_33;

CPU_ghs_mW_34  =  diff(CPU_ghs_val_34) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_34 =  diff(LPM_ghs_val_34) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_34  =  diff(TX_ghs_val_34)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_34  =  diff(RX_ghs_val_34)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_34 = CPU_ghs_mW_34 + LPM_ghs_mW_34 + TX_ghs_mW_34 + RX_ghs_mW_34;

CPU_ghs_mW_35  =  diff(CPU_ghs_val_35) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_35 =  diff(LPM_ghs_val_35) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_35  =  diff(TX_ghs_val_35)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_35  =  diff(RX_ghs_val_35)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_35 = CPU_ghs_mW_35 + LPM_ghs_mW_35 + TX_ghs_mW_35 + RX_ghs_mW_35;

CPU_ghs_mW_36  =  diff(CPU_ghs_val_36) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_36 =  diff(LPM_ghs_val_36) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_36  =  diff(TX_ghs_val_36)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_36  =  diff(RX_ghs_val_36)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_36 = CPU_ghs_mW_36 + LPM_ghs_mW_36 + TX_ghs_mW_36 + RX_ghs_mW_36;

CPU_ghs_mW_37  =  diff(CPU_ghs_val_37) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_37 =  diff(LPM_ghs_val_37) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_37  =  diff(TX_ghs_val_37)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_37  =  diff(RX_ghs_val_37)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_37 = CPU_ghs_mW_37 + LPM_ghs_mW_37 + TX_ghs_mW_37 + RX_ghs_mW_37;

CPU_ghs_mW_38  =  diff(CPU_ghs_val_38) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_38 =  diff(LPM_ghs_val_38) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_38  =  diff(TX_ghs_val_38)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_38  =  diff(RX_ghs_val_38)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_38 = CPU_ghs_mW_38 + LPM_ghs_mW_38 + TX_ghs_mW_38 + RX_ghs_mW_38;

CPU_ghs_mW_39  =  diff(CPU_ghs_val_39) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_39 =  diff(LPM_ghs_val_39) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_39  =  diff(TX_ghs_val_39)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_39  =  diff(RX_ghs_val_39)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_39 = CPU_ghs_mW_39 + LPM_ghs_mW_39 + TX_ghs_mW_39 + RX_ghs_mW_39;

CPU_ghs_mW_40  =  diff(CPU_ghs_val_40) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW_40 =  diff(LPM_ghs_val_40) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW_40  =  diff(TX_ghs_val_40)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW_40  =  diff(RX_ghs_val_40)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs_40 = CPU_ghs_mW_40 + LPM_ghs_mW_40 + TX_ghs_mW_40 + RX_ghs_mW_40;


VectorEnergiaPromedio = [        mean(total_ghs_1) 
                                 mean(total_ghs_2)
                                 mean(total_ghs_3)
                                 mean(total_ghs_4)
                                 mean(total_ghs_5)
                                 mean(total_ghs_6)
                                 mean(total_ghs_7)
                                 mean(total_ghs_8)
                                 mean(total_ghs_9)
                                 mean(total_ghs_10)
                                 mean(total_ghs_11)
                                 mean(total_ghs_12)
                                 mean(total_ghs_13)
                                 mean(total_ghs_14)
                                 mean(total_ghs_15)
                                 mean(total_ghs_16)
                                 mean(total_ghs_17)
                                 mean(total_ghs_18)
                                 mean(total_ghs_19)
                                 mean(total_ghs_20)
                                 mean(total_ghs_21)
                                 mean(total_ghs_22)
                                 mean(total_ghs_23)
                                 mean(total_ghs_24)
                                 mean(total_ghs_25)
                                 mean(total_ghs_26)
                                 mean(total_ghs_27)
                                 mean(total_ghs_28)
                                 mean(total_ghs_29)
                                 mean(total_ghs_30)
                                 mean(total_ghs_31)
                                 mean(total_ghs_32)
                                 mean(total_ghs_33)
                                 mean(total_ghs_34)
                                 mean(total_ghs_35)
                                 mean(total_ghs_36)
                                 mean(total_ghs_37)
                                 mean(total_ghs_38)
                                 mean(total_ghs_39)
                                 mean(total_ghs_40)];

Energia_Promedio_GHS_AfterTreeConstruction =  mean (VectorEnergiaPromedio)
Energia_std_GHS_AfterTreeConstruction      =  std  (VectorEnergiaPromedio)

% % Calculo de la potencia - ghs
% CPU_ghs_mW  =  diff(CPU_ghs_val) *  CPU_mA * Voltage / rtimer_second / intervalo ;
% LPM_ghs_mW =  diff(LPM_ghs_val) *  LPM_mA * Voltage / rtimer_second / intervalo ;
% TX_ghs_mW  =  diff(TX_ghs_val)  *  TX_mA * Voltage / rtimer_second / intervalo ;
% RX_ghs_mW  =  diff(RX_ghs_val)  *  RX_mA * Voltage / rtimer_second / intervalo ;
% total_ghs = CPU_ghs_mW + LPM_ghs_mW + TX_ghs_mW + RX_ghs_mW;

% Energia_Consumida_Promedio_LIBP =  mean(total_ghs)

%% Graficar la potencia



% % Grafica ghs
% plot(eje_x_ghs, total_ghs,  '-.s' , 'color', color0  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x_ghs, CPU_ghs_mW, '-.*' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x_ghs, LPM_ghs_mW, '--+','color', color2  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x_ghs, TX_ghs_mW,  ':d' ,'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x_ghs, RX_ghs_mW,  '-o' ,'color', color4  ,'LineWidth',2, 'MarkerSize', 6);
% set(gca,'XLim',[intervalo max_eje_x_ghs])
% set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
% title('Power consumption GHS', 'FontSize', 20, 'fontweight','bold');
% xlabel('Time (s)','FontSize', 18,'fontweight','bold');
% ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
% legenda1 = 'Total';
% legenda2 = 'CPU';
% legenda3 = 'LPM';
% legenda4 = 'TX';
% legenda5 = 'RX';
% legenda = legend(legenda1,legenda2,legenda3,legenda4,legenda5,'Location','NorthEastOutside');
% set(legenda,'FontSize',12);
% 
% 
% figure
% 
% % Grafica ghs
% plot(eje_x_ghs, total_ghs,  '-.s' , 'color', color0  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x_ghs, CPU_ghs_mW, '-.*' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x_ghs, LPM_ghs_mW, '--+','color', color2  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x_ghs, TX_ghs_mW,  ':d' ,'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x_ghs, RX_ghs_mW,  '-o' ,'color', color4  ,'LineWidth',2, 'MarkerSize', 6);
% set(gca,'XLim',[intervalo max_eje_x_ghs])
% set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
% title('Power consumption LIBP', 'FontSize', 20, 'fontweight','bold');
% xlabel('Time (s)','FontSize', 18,'fontweight','bold');
% ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
% legenda1 = 'Total';
% legenda2 = 'CPU';
% legenda3 = 'LPM';
% legenda4 = 'TX';
% legenda5 = 'RX';
% legenda = legend(legenda1,legenda2,legenda3,legenda4,legenda5,'Location','NorthEastOutside');
% set(legenda,'FontSize',12);
% 
% figure
% 
% %% Grafica GHS y LIBP unidas
% 
% % total
% plot(eje_x, total_ghs,  '-.s' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x, total_ghs,  '-.s' , 'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
% set(gca,'XLim',[intervalo max_eje_x])
% set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
% title('Total power consumption', 'FontSize', 20, 'fontweight','bold');
% xlabel('Time (s)','FontSize', 18,'fontweight','bold');
% ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
% legenda1 = 'ghs';
% legenda2 = 'ghs';
% legenda = legend(legenda1,legenda2,'Location','NorthEastOutside');
% set(legenda,'FontSize',12);
% 
% 
% figure 
% 
% % CPU
% plot(eje_x, CPU_ghs_mW,  '-.s' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x, CPU_ghs_mW,  '-.s' , 'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
% set(gca,'XLim',[intervalo max_eje_x])
% set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
% title('CPU power consumption', 'FontSize', 20, 'fontweight','bold');
% xlabel('Time (s)','FontSize', 18,'fontweight','bold');
% ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
% legenda1 = 'ghs';
% legenda2 = 'ghs';
% legenda = legend(legenda1,legenda2,'Location','NorthEastOutside');
% set(legenda,'FontSize',12);
% 
% 
% figure 
% 
% % LPM
% plot(eje_x, LPM_ghs_mW,  '-.s' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x, LPM_ghs_mW,  '-.s' , 'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
% set(gca,'XLim',[intervalo max_eje_x])
% set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
% title('LPM power consumption', 'FontSize', 20, 'fontweight','bold');
% xlabel('Time (s)','FontSize', 18,'fontweight','bold');
% ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
% legenda1 = 'ghs';
% legenda2 = 'ghs';
% legenda = legend(legenda1,legenda2,'Location','NorthEastOutside');
% set(legenda,'FontSize',12);
% 
% 
% figure 
% 
% % TX
% plot(eje_x, TX_ghs_mW,  '-.s' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x, TX_ghs_mW,  '-.s' , 'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
% set(gca,'XLim',[intervalo max_eje_x])
% set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
% title('TX power consumption', 'FontSize', 20, 'fontweight','bold');
% xlabel('Time (s)','FontSize', 18,'fontweight','bold');
% ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
% legenda1 = 'ghs';
% legenda2 = 'ghs';
% legenda = legend(legenda1,legenda2,'Location','NorthEastOutside');
% set(legenda,'FontSize',12);
% 
% 
% figure 
% 
% % RX
% plot(eje_x, RX_ghs_mW,  '-.s' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
% hold on
% plot(eje_x, RX_ghs_mW,  '-.s' , 'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
% set(gca,'XLim',[intervalo max_eje_x])
% set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
% title('RX power consumption', 'FontSize', 20, 'fontweight','bold');
% xlabel('Time (s)','FontSize', 18,'fontweight','bold');
% ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
% legenda1 = 'ghs';
% legenda2 = 'ghs';
% legenda = legend(legenda1,legenda2,'Location','NorthEastOutside');
% set(legenda,'FontSize',12);
