/* 
 * File:   dht11.h
 *  Libreria inspirada en código desarrollado por Giovanny Anglas
 *  Modificado por Estuardo Castillo para funcionar en xc8 y PIC16F887
 *  Hardware necesario: sensor de temperatura y humeadad DHT11
 * Created on August 19, 2023, 5:43 PM
 */

#ifndef DHT11_H
#define	DHT11_H

#define TRIS_DHT11 TRISAbits.TRISA2 //configuro el registro del bit donde esta conectado el DHT11                                                              
#define PORT_DHT11 PORTAbits.RA2                                                                                                                                                        


uint8_t check; //variable que revisa si sensor contesta

void StartSignal(void);                                                                                                                                                                                                                                           
void CheckResponse(void);
uint8_t ReadData(void);                                                                                                               
void Leer_DHT11(uint8_t *temperatura, uint8_t *humedad);

#endif	/* DHT11_H */

