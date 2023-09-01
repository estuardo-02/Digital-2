/* 
 * File: ConfgUART.c  
 * Revision history: 
 */

#include "eusart.h"

void UART_RX_CONFIG(uint8_t baudrate){
    //Paso 1
    SPBRG = baudrate;     //Calculo para aprox. 9600 Baud
    SPBRGH = 0;            //% error= 0.16%
    BAUDCTLbits.BRG16 = 0; //Se utilizan 8 bits para Baud rate
    //Paso 2
    RCSTAbits.SPEN = 1;  //Habilita comunicacion (modulo UART)
    //Paso 4
    RCSTAbits.CREN = 1;  //Habilitar Receptor
    PIE1bits.RCIE = 1;   //Habilitar interrupcion de Recepcion
}

void UART_TX_CONFIG(uint8_t baudrate){
    //Paso 1
    SPBRG = baudrate;     //Calculo para aprox. 9600 Baud
    SPBRGH = 0;            //% error= 0.16%
    TXSTAbits.BRGH = 1;    //Baud rate de alta velocidad 
    //Paso 2
    TXSTAbits.SYNC = 0;  //Comunicacion asincrona (full-duplex
    //Paso 3: 9 bits
    TXSTAbits.TX9 = 0;   //Se utilizan solo 8 bits
    //Paso 4
    TXSTAbits.TXEN = 1;  //habilitado Transmisor
    TXIF = 0; //apagar bandera TX
}

void UART_READ(const char a)
{
  while (TXSTAbits.TRMT == 0);  // esperar a que el bit de transmision este en 0
  TXREG = a;       // actualizar EUSART registro de transmision
}

void UART_WRITE(unsigned char *cadena)
{
    uint8_t i = 0;
    while (cadena[i] != '\0')
    UART_READ (cadena[i++]);
}