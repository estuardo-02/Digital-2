/*
 * File:   eusart.c
 * Author: estua
 *
 * Created on July 27, 2023, 6:51 PM
 */


#include "eusart.h"
#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 1000000

void UART_RX_config(uint16_t baudrate, long xtal_freq){
    uint16_t SPBRG_temp = ((xtal_freq/baudrate)/4) - 1;
    BAUDCTLbits.BRG16 = 1;
    SPBRG = SPBRG_temp;
    SPBRGH = 0;
    //SPBRG = 25;
    //SPBRGH = 0;
    UART_TX_config(baudrate, xtal_freq);
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;
    
}
void UART_TX_config(uint16_t baudrate, long xtal_freq){
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    //TXSTAbits.TXEN = 1;
    uint16_t SPBRG_temp = ((xtal_freq/baudrate)/64) - 1;
}

void UART_write_char(char c){
    while(PIR1bits.TXIF){
        TXREG = c;
    }
}
char UART_read_char(){
    //RCSTAbits.CREN = 1; //habilitar recepcion de datos
    return RCREG;
}
void cadena(char mensaje[]) {
    int done = 0;
    while(done ==0){
    while(PIR1bits.TXIF){
    int i;
        if (mensaje[i] != '\0'){
            TXREG = mensaje[i];
        }else {
            TXREG = 13;
            i = -1;
            done = 1;
        }
        i++;
    }
  }
}