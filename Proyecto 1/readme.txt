***************************************PROYECTO 1 ELECTRONICA DIGITAL 2*********************************              archivo readme.txt
desarrollado por Estuardo Castillo y Carlos Valdés        
                           ,----..           
         ,--,       ,---. /   /   \          
       ,'_ /|      /__./||   :     :         
  .--. |  | : ,---.;  ; |.   |  ;. /         
,'_ /| :  . |/___/ \  | |.   ; /--`          
|  ' | |  . .\   ;  \ ' |;   | ;  __         
|  | ' |  | | \   \  \: ||   : |.' .'        
:  | | :  ' ;  ;   \  ' ..   | '_.' :        
|  ; ' |  | '   \   \   ''   ; : \  |        
:  | : ;  ; |    \   `  ;'   | '/  .'        
'  :  `--'   \    :   \ ||   :    /          
:  ,      .-./     '---"  \   \ .'           
 `--`----'                 `---`             
                                             
Se trabajó una red de sensores implementando distintos protocolos de comunicación entre controladores servidores y cliente con el fin de simular un proceso. Para el proyecto se emplearon microcontroladores PIC16F887 con comunicación i2c para simular un sistema de riego automatizado para un invernadero. El proyecto contó con sensores que miden la temperatura, humedad en tierra y capacidad de un tanque de agua. Adicionalmente se empleó un motor DC para el sistema de riego y un servo motor para el control de ventilación y de temperatura. 
Posteriormente se implementó una comunicación con un ESP32 a partir del PIC cliente con el protocolo serial para poder recolectar los datos a través de un controlador con acceso a internet. Utilizando la plataforma Adafruit se logró implementar un dashboard en la nube donde se tiene monitoreo de la red de sensores desde cualquier parte del mundo y abriendo las puertas para un sistema del internet de las cosas. 

**********************************************XC8******************************************************
Códigos desarrollados en XC8 utilizan todos una frecuencia de reloj de 8MHz para su operación.
Condieraciones necesarias deben hacerse si se desea cambiar la frecuencia de reloj para archivo
slave_sistema_clima.c dado que es altamente dependiente de frecuencia de tmr_1 y tmr_0
control de servo via interrupciones que se generan cada 20ms en tmr 0 y t_on de 1 hasta 2 ms de tmr1
**********************************************I2C CONFIG******************************************************
Seleccionar difrerntes direccione si se desean conectar múltiples dispositivos i2c, direcciones dedicadas para 
cada uno de los PICs:
0x30 dirección de PIC ultrasonico
0x40 dirección de PIC sistema de clima
0x50 dirección de PIC sistema de riego

**********************************************EUSART CONFIG******************************************************
Velocidad de transferencia entre PIC16F887 y ESP32 establecida a 9600 baudios, no es necesario modificar dada la naturaleza de la tranferencia de datos lenta por restricciones de Adafruit io

Velocidad de transferencia entre ESP32 y monitor serial conectado a PC establecido a 115200 baudios. 
