/* 
 * File:   eusart.h
 * Author: estua
 *
 * Created on July 27, 2023, 6:48 PM
 */

#ifndef EUSART_H
#define	EUSART_H
#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

void UART_RX_config(uint16_t baudrate, long xtal_freq);
void UART_TX_config(uint16_t baudrate, long xtal_freq);
void UART_write_char(char c);
void cadena(char mensaje[]);
char UART_read_char();

#endif	/* EUSART_H */

