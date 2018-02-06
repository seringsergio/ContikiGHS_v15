% Tamano Ejecutable

%REFERENCIAS
% referencia text data bss: https://mcuoneclipse.com/2013/04/14/text-data-and-bss-code-and-data-size-explained/
% como calcular los valores: https://github.com/energia/Energia/wiki/FLASH-and-estimated-RAM-Usage


close all


%Definiciones
names = {'D-GHS'; 'LIBP';'' }
ojimetro0 = 0.85;
ojimetro1 = 0.75;
ojimetro2 = 0.86;
ojimetro3 = 1.14;
ojimetro4 = 1.86;
ojimetro5 = 2.14;

ojimetro6 = 0.015;
ojimetro7 = 0.045;
ojimetro8 = 0.105;
ojimetro9 = 0.055;
ojimetro10 = 0.070;
ojimetro11 = 0.055;
ojimetro12 = 0.045;
ojimetro13 = 0.065;
ojimetro14 = 0.145;


ojimetro15 = 1.3;
ojimetro16 = 5;
ojimetro17 = 0.75;
ojimetro18 = 1.2;

maxflash_cm5000      =   [ flash_cm5000 flash_cm5000  flash_cm5000 flash_cm5000] / 1000;
maxflash_cm5000_ejex =   [ 0 1 2 3];

maxRAM_cm5000      =   [ ram_cm5000 ram_cm5000 ram_cm5000 ram_cm5000] / 1000;
maxRAM_cm5000_ejex =   [ 0 1 2 3];

negro = [0 0 0];
blanco = [0.9 0.9 0.9];
%ojimetro2 = 200;
porcentaje_text = '%';
%message1 = sprintf('48KB \n available');
%message2 = sprintf('10KB \n available');



%valores CM5000 en Bytes
flash_cm5000 = 48000;
ram_cm5000  = 10000;

%Valores GHS en Bytes
text_ghs = 41334; % text is what ends up in FLASH memory. 
data_ghs = 450;   % data is used for initialized data. It will end up in RAM
bss_ghs  = 8652;  % bss contains all the uninitalized data. As bss ends up in RAM

%Valores LIBP en Bytes
text_libp = 30460; % text is what ends up in FLASH memory. 
data_libp = 208;   % data is used for initialized data. It will end up in RAM
bss_libp  = 7782;  % bss contains all the uninitalized data. As bss ends up in RAM


% Calculos segun: https://github.com/energia/Energia/wiki/FLASH-and-estimated-RAM-Usage

flash_ghs            = text_ghs  + data_ghs;   
flash_libp           = text_libp + data_libp;
ram_ghs              = bss_ghs   + data_ghs;    % Please note this is only an estimate 
ram_libp             = bss_libp  + data_libp;   % Please note this is only an estimate

% Calculo porcentajes

porcentaje_flash_ghs  = flash_ghs  / flash_cm5000 * 100;
porcentaje_flash_libp = flash_libp / flash_cm5000 * 100;
porcentaje_ram_ghs  = ram_ghs  / ram_cm5000 * 100;
porcentaje_ram_libp = ram_libp / ram_cm5000 * 100;

%conversion de porcentajes a TEXTO 
porcentaje_flash_ghs_text  = sprintf('%.1f',porcentaje_flash_ghs); % redondear a un solo decimal
porcentaje_flash_ghs_text  = strcat(porcentaje_flash_ghs_text,porcentaje_text); % Concatenar el caracter de porcentaje

porcentaje_flash_libp_text = sprintf('%.1f',porcentaje_flash_libp);% redondear a un solo decimal
porcentaje_flash_libp_text =strcat(porcentaje_flash_libp_text,porcentaje_text);% Concatenar el caracter de porcentaje

porcentaje_ram_ghs_text  = sprintf('%.1f',porcentaje_ram_ghs);% redondear a un solo decimal
porcentaje_ram_ghs_text  = strcat(porcentaje_ram_ghs_text,porcentaje_text);% Concatenar el caracter de porcentaje

porcentaje_ram_libp_text  = sprintf('%.1f',porcentaje_ram_libp);% redondear a un solo decimal
porcentaje_ram_libp_text  = strcat(porcentaje_ram_libp_text,porcentaje_text);% Concatenar el caracter de porcentaje

subplot(2,1,1);
y = [flash_ghs; flash_libp ] / 1000; % Divido en 1000 para que quede en kbytes. 
bar(y,'stacked', 'b')
hold on
plot(maxflash_cm5000_ejex, maxflash_cm5000, 'r-.','LineWidth',4);

%porcentaje = num2str(porcentaje_flash_ghs)  + '%';
text(ojimetro0, flash_cm5000/2/1000, porcentaje_flash_ghs_text,'Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro0+1, flash_cm5000/2/1000,porcentaje_flash_libp_text ,'Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro15, flash_cm5000/1000  + ojimetro16 ,  'Max' ,'Color','r', 'FontWeight', 'bold', 'FontSize', 12) % divido 1000 para que quede en KB y 2 para que quede en el centro

%text(ojimetro0+2, flash_cm5000/2/1000,'48KB' ,'Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro

set(gca,'YLim',[0 flash_cm5000/1000 + 10]) % Divido en 1000 para que quede en kbytes. 
set(gca,'xtick',[1:3],'xticklabel',names) % para colocar los nombres GHS, LIBP, available
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
%title('Memory consumption', 'FontSize', 20, 'fontweight','bold');
%xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Flash (KB)','FontSize', 18,'fontweight','bold');


subplot(2,1,2);
y = [ ram_ghs;  ram_libp  ] / 1000; % Divido en 1000 para que quede en kbytes.
map = [0, 0, 0.3
    0, 0, 0.4
    0, 0, 0.5
    0, 0, 0.6
    0, 0, 0.8
    0, 0, 1.0];
colormap(map)
bar(y,'stacked','b')
hold on
plot(maxRAM_cm5000_ejex, maxRAM_cm5000, 'r-.','LineWidth',4);

text(ojimetro0, ram_cm5000/2/1000, porcentaje_ram_ghs_text,'Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro0+1, ram_cm5000/2/1000,porcentaje_ram_libp_text ,'Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
%text(ojimetro0+2, ram_cm5000/2/1000,'10KB' ,'Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro15, ram_cm5000/1000  + ojimetro18 ,  'Max' ,'Color','r', 'FontWeight', 'bold', 'FontSize', 12) % divido 1000 para que quede en KB y 2 para que quede en el centro


set(gca,'YLim',[0 ram_cm5000/1000 + 2]) % Divido en 1000 para que quede en kbytes. 
set(gca,'xtick',[1:3],'xticklabel',names)
ylabel('RAM (KB)','FontSize', 18,'fontweight','bold');
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes

figure

% Tiempo de convergencia
TiempoConv_ghs  = [164 174 167 151 163 168 165 166 162 161];
TiempoConv_libp = [157 143 165 153 145 153 150 150 153 149];

TiempoConv_ghs_promedio  = mean(TiempoConv_ghs);
TiempoConv_libp_promedio = mean(TiempoConv_libp);

tiempos_promedios = [TiempoConv_ghs_promedio TiempoConv_libp_promedio];

TiempoConv_ghs_standard  = std(TiempoConv_ghs);
TiempoConv_libp_standard = std(TiempoConv_libp);

tiempos_standard = [TiempoConv_ghs_standard TiempoConv_libp_standard];

% porcentaje_text = 'TiempoConv_ghs_promedio +- ';
map = [0, 0, 0.3
    0, 0, 0.4
    0, 0, 0.5
    0, 0, 0.6
    0, 0, 0.8
    0, 0, 1.0];
colormap(map)
bar(tiempos_promedios,'stacked','b')
%set(gca,'YLim',[0 ojimetro2]) % Divido en 1000 para que quede en kbytes. 
%text(ojimetro1, ojimetro2/2, porcentaje_ram_ghs_text,'Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
%text(ojimetro1+1, ojimetro2/2,porcentaje_ram_libp_text ,'Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro

hold on
errorbar(1:2,tiempos_promedios,tiempos_standard,'k.','LineWidth', 2)


%set(gca,'YLim',[0 flash_cm5000/1000]) % Divido en 1000 para que quede en kbytes. 
set(gca,'xtick',[1:3],'xticklabel',names) % para colocar los nombres GHS, LIBP, available
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
% title('Convergence time', 'FontSize', 20, 'fontweight','bold');
%xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Convergence time (s)','FontSize', 18,'fontweight','bold');

% ENERGIA
% powertrace_v8_ghs; %script
powertrace_v8_libp_TreeConstruction; %script
powertrace_v8_libp_AfterTreeConstruction; %script

powertrace_v8_ghs_TreeConstruction; %script
powertrace_v8_ghs_AfterTreeConstruction; %script

energia_promedios = [Energia_Promedio_GHS_TreeConstruction   Energia_Promedio_GHS_AfterTreeConstruction  ;  Energia_Promedio_LIBP_TreeConstruction  Energia_Promedio_LIBP_AfterTreeConstruction];
energia_standard  = [Energia_std_GHS_TreeConstruction        Energia_std_GHS_AfterTreeConstruction  ;  Energia_std_LIBP_TreeConstruction      Energia_std_LIBP_AfterTreeConstruction ];

% porcentaje_text = 'TiempoConv_ghs_promedio +- ';
figure

map = [0, 0, 0.3
    0, 0, 0.4
    0, 0, 0.5
    0, 0, 0.6
    0, 0, 0.8
    0, 0, 1.0];
colormap(map)
%bar(energia_promedios,'stacked')
bar(energia_promedios)
%set(gca,'YLim',[0 ojimetro2]) % Divido en 1000 para que quede en kbytes. 
%text(ojimetro1, ojimetro2/2, porcentaje_ram_ghs_text,'Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
%text(ojimetro1+1, ojimetro2/2,porcentaje_ram_libp_text ,'Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro

legenda1 = 'Tree construction';
legenda2 = 'Data collection';
legenda =legend(legenda1,legenda2,'Location','NorthWest');
set(legenda,'FontSize',14);

hold on

x = [ojimetro2 ojimetro3 ; ojimetro4 ojimetro5]
errorbar(x, energia_promedios,energia_standard,'k.','LineWidth', 2)


%set(gca,'YLim',[0 flash_cm5000/1000]) % Divido en 1000 para que quede en kbytes. 
set(gca,'xtick',[1:3],'xticklabel',names) % para colocar los nombres GHS, LIBP, available
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
%title('Energy consumption', 'FontSize', 20, 'fontweight','bold');
%xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Energy consumption (mW)','FontSize', 18,'fontweight','bold');

%% Numero de mensajes de control enviados

figure

ControlPacketSent_libp; %script
ControlPacketSent_ghs; %script


map = [0, 0, 0.3
    0, 0, 0.4
    0, 0, 0.5
    0, 0, 0.6
    0, 0, 0.8
    0, 0, 1.0];
colormap(map)
TotalCtrlmessages_GHS = TotalConnect_ghs +TotalInitiate_ghs +TotalTest_ghs +TotalAccept_ghs +TotalReject_ghs +TotalReport_ghs +TotalChangeroot_ghs;
TotalCtrlmessages_LIBP = TotalBeaconSent_libp + TotalAckSent_libp;
PorcentajeMasLIBP = (TotalCtrlmessages_LIBP - TotalCtrlmessages_GHS) * 100 / TotalCtrlmessages_GHS;
y = [TotalConnect_ghs TotalInitiate_ghs TotalTest_ghs TotalAccept_ghs TotalReject_ghs TotalReport_ghs TotalChangeroot_ghs ;TotalBeaconSent_libp TotalAckSent_libp 0 0 0 0 0 ];
bar(y,'stacked',  'b'  )
%bar(y,'stacked')
text(ojimetro1 + ojimetro6, TotalConnect_ghs/2, 'Connect','Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro1 + ojimetro7, (TotalConnect_ghs)+(TotalInitiate_ghs/2), 'Initiate','Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro1 + ojimetro8, (TotalConnect_ghs)+(TotalInitiate_ghs) + (TotalTest_ghs/2), 'Test','Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro1 + ojimetro9, (TotalConnect_ghs)+(TotalInitiate_ghs) + (TotalTest_ghs) + (TotalAccept_ghs/2), 'Accept','Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro1 + ojimetro10, (TotalConnect_ghs)+(TotalInitiate_ghs) + (TotalTest_ghs) + (TotalAccept_ghs) + (TotalReject_ghs/2), 'Reject','Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro1 + ojimetro11, (TotalConnect_ghs)+(TotalInitiate_ghs) + (TotalTest_ghs) + (TotalAccept_ghs) + (TotalReject_ghs) +(TotalReport_ghs/2), 'Report','Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro1 + ojimetro12, (TotalConnect_ghs)+(TotalInitiate_ghs) + (TotalTest_ghs) + (TotalAccept_ghs) + (TotalReject_ghs) +(TotalReport_ghs) + (TotalChangeroot_ghs/2) , 'ChangeRoot','Color','white', 'FontWeight', 'bold', 'FontSize', 9) % divido 1000 para que quede en KB y 2 para que quede en el centro

text(ojimetro1+1 + ojimetro13, TotalBeaconSent_libp/2, 'Beacon','Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro
text(ojimetro1+1 + ojimetro14, TotalBeaconSent_libp + (TotalAckSent_libp/2) , 'Ack','Color','white', 'FontWeight', 'bold', 'FontSize', 14) % divido 1000 para que quede en KB y 2 para que quede en el centro

set(gca,'xtick',[1:3],'xticklabel',names) % para colocar los nombres GHS, LIBP, available
set(gca,'fontsize',14) % Colocar mas grandes los numeros de los ejes
%title('Total number of control packets', 'FontSize', 20, 'fontweight','bold');
%xlabel('Time (s)','FontSize', 18,'fontweight','bold');
ylabel('Number of packets','FontSize', 18,'fontweight','bold');


% legenda1 = 'Scenario 2: Int close to TX';
% legenda2 = 'Threshold';
% legenda3 = 'Threshold';
% legenda =legend(legenda1,legenda2,legenda3,'Location','NorthWest');
% set(legenda,'FontSize',14);
