************PROYECTO 1 ELECTRONICA DIGITAL 2***************                                        
                                             
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
                                             
archivo readme.txt
desarrollado por Estuardo Castillo y Carlos Valdés 
Códigos desarrollados en XC8 utilizan todos una frecuencia de reloj de 8MHz para su operación.
Condieraciones necesarias deben hacerse si se desea cambiar la frecuencia de reloj para archivo
slave_sistema_clima.c dado que es altamente dependiente de frecuencia de tmr_1 y tmr_0
control de servo via interrupciones que se generan cada 20ms en tmr 0 y t_on de 1 hasta 2 ms de tmr1

Seleccionar difrerntes direccione si se desean conectar múltiples dispositivos i2c, direcciones dedicadas para 
cada uno de los PICs:
0x30 dirección de PIC ultrasonico
0x40 dirección de PIC sistema de clima
0x50 dirección de PIC sistema de riego
