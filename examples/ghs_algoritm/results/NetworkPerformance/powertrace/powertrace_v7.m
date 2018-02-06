close all 
clear
clc

%% Parametros de las graficas  - CAMBIA DE VALOR

% datos
num_datos     =  6; % Numero de datos. Es igual al numero de lineas del archivo nodo_1.txt

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
max_eje_x = intervalo * (num_datos - 1); % Resto -1 porque diff elimina 1 valor. 
eje_x     = [intervalo:intervalo:max_eje_x]; % Crea el eje x. De 10 en 10. 
color0 = [0 0 0] ;
color1 = [0.2 0.2 0.6] ;
color2 = [0.2 0.6 0.2];
color3 = [0.6 0.2 0.2];
color4 = [0.4 0.3 0.3];


%% Leer archivos 

%archivos ghs
nodo_ghs_1_file = '/home/sergiodiaz/Desktop/ContikiGHS/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_ghs_1.dat';

%archivos libp
nodo_libp_1_file = '/home/sergiodiaz/Desktop/ContikiGHS/examples/ghs_algoritm/results/NetworkPerformance/powertrace/nodo_libp_1.dat';


%Extraer la informacion del archivo - ghs
nodo_ghs_1  = csvread(nodo_ghs_1_file);

%Extraer la informacion del archivo - libp
nodo_libp_1  = csvread(nodo_libp_1_file);

% Guardar valores individuales - ghs
CPU_ghs_val = nodo_ghs_1(:,2); % Valores de la CPU
LPM_ghs_val = nodo_ghs_1(:,3); % Valores de la CPU
TX_ghs_val  = nodo_ghs_1(:,4); % Valores de la CPU
RX_ghs_val  = nodo_ghs_1(:,5); % Valores de la CPU

% Guardar valores individuales - libp
CPU_libp_val = nodo_libp_1(:,2); % Valores de la CPU
LPM_libp_val = nodo_libp_1(:,3); % Valores de la CPU
TX_libp_val  = nodo_libp_1(:,4); % Valores de la CPU
RX_libp_val  = nodo_libp_1(:,5); % Valores de la CPU

%% Calculo de la potencia en (mW)

% Calculo de la potencia - ghs
CPU_ghs_mW  =  diff(CPU_ghs_val) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_ghs_mW =  diff(LPM_ghs_val) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_ghs_mW  =  diff(TX_ghs_val)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_ghs_mW  =  diff(RX_ghs_val)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_ghs = CPU_ghs_mW + LPM_ghs_mW + TX_ghs_mW + RX_ghs_mW;

% Calculo de la potencia - libp
CPU_libp_mW  =  diff(CPU_libp_val) *  CPU_mA * Voltage / rtimer_second / intervalo ;
LPM_libp_mW =  diff(LPM_libp_val) *  LPM_mA * Voltage / rtimer_second / intervalo ;
TX_libp_mW  =  diff(TX_libp_val)  *  TX_mA * Voltage / rtimer_second / intervalo ;
RX_libp_mW  =  diff(RX_libp_val)  *  RX_mA * Voltage / rtimer_second / intervalo ;
total_libp = CPU_libp_mW + LPM_libp_mW + TX_libp_mW + RX_libp_mW;

%% Graficar la potencia


% Grafica ghs
plot(eje_x, total_ghs,  '-.s' , 'color', color0  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, CPU_ghs_mW, '-.*' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, LPM_ghs_mW, '--+','color', color2  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, TX_ghs_mW,  ':d' ,'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, RX_ghs_mW,  '-o' ,'color', color4  ,'LineWidth',2, 'MarkerSize', 6);
set(gca,'XLim',[intervalo max_eje_x])
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
title('Power consumption GHS', 'FontSize', 20, 'fontweight','bold');
xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
legenda1 = 'Total';
legenda2 = 'CPU';
legenda3 = 'LPM';
legenda4 = 'TX';
legenda5 = 'RX';
legenda = legend(legenda1,legenda2,legenda3,legenda4,legenda5,'Location','NorthEastOutside');
set(legenda,'FontSize',12);


figure

% Grafica libp
plot(eje_x, total_libp,  '-.s' , 'color', color0  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, CPU_libp_mW, '-.*' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, LPM_libp_mW, '--+','color', color2  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, TX_libp_mW,  ':d' ,'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, RX_libp_mW,  '-o' ,'color', color4  ,'LineWidth',2, 'MarkerSize', 6);
set(gca,'XLim',[intervalo max_eje_x])
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
title('Power consumption LIBP', 'FontSize', 20, 'fontweight','bold');
xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
legenda1 = 'Total';
legenda2 = 'CPU';
legenda3 = 'LPM';
legenda4 = 'TX';
legenda5 = 'RX';
legenda = legend(legenda1,legenda2,legenda3,legenda4,legenda5,'Location','NorthEastOutside');
set(legenda,'FontSize',12);

figure

%% Grafica GHS y LIBP unidas

% total
plot(eje_x, total_ghs,  '-.s' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, total_libp,  '-.s' , 'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
set(gca,'XLim',[intervalo max_eje_x])
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
title('Total power consumption', 'FontSize', 20, 'fontweight','bold');
xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
legenda1 = 'ghs';
legenda2 = 'libp';
legenda = legend(legenda1,legenda2,'Location','NorthEastOutside');
set(legenda,'FontSize',12);


figure 

% CPU
plot(eje_x, CPU_ghs_mW,  '-.s' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, CPU_libp_mW,  '-.s' , 'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
set(gca,'XLim',[intervalo max_eje_x])
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
title('CPU power consumption', 'FontSize', 20, 'fontweight','bold');
xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
legenda1 = 'ghs';
legenda2 = 'libp';
legenda = legend(legenda1,legenda2,'Location','NorthEastOutside');
set(legenda,'FontSize',12);


figure 

% LPM
plot(eje_x, LPM_ghs_mW,  '-.s' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, LPM_libp_mW,  '-.s' , 'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
set(gca,'XLim',[intervalo max_eje_x])
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
title('LPM power consumption', 'FontSize', 20, 'fontweight','bold');
xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
legenda1 = 'ghs';
legenda2 = 'libp';
legenda = legend(legenda1,legenda2,'Location','NorthEastOutside');
set(legenda,'FontSize',12);


figure 

% TX
plot(eje_x, TX_ghs_mW,  '-.s' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, TX_libp_mW,  '-.s' , 'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
set(gca,'XLim',[intervalo max_eje_x])
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
title('TX power consumption', 'FontSize', 20, 'fontweight','bold');
xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
legenda1 = 'ghs';
legenda2 = 'libp';
legenda = legend(legenda1,legenda2,'Location','NorthEastOutside');
set(legenda,'FontSize',12);


figure 

% RX
plot(eje_x, RX_ghs_mW,  '-.s' , 'color', color1  ,'LineWidth',2, 'MarkerSize', 6);
hold on
plot(eje_x, RX_libp_mW,  '-.s' , 'color', color3  ,'LineWidth',2, 'MarkerSize', 6);
set(gca,'XLim',[intervalo max_eje_x])
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
title('RX power consumption', 'FontSize', 20, 'fontweight','bold');
xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Power (mW)','FontSize', 18,'fontweight','bold');
legenda1 = 'ghs';
legenda2 = 'libp';
legenda = legend(legenda1,legenda2,'Location','NorthEastOutside');
set(legenda,'FontSize',12);
