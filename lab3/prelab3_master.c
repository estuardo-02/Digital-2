/*
 * File:   prelab3.c
 * Author: estua
 * Programa para pic maestro/cliente
 * Created on July 14, 2023, 5:11 PM
 *******************************************************************************
 */
/*******************************************************************************
 *  * PALABRA DE CONFIGURACION
 *******************************************************************************
*/
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection o

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
/*******************************************************************************
 *  * LIBRERIAS
 *******************************************************************************
*/
#include <xc.h>
#include <stdint.h>
#include "spi.h"
#include "adc_config.h"
#define _XTAL_FREQ 8000000
/*******************************************************************************
 *  * PROTOTIPOS DE FUNCION
 *******************************************************************************
*/
void setup(void);
/*******************************************************************************
 *  * VARIABLES GLOBALES
 *******************************************************************************
*/

/*******************************************************************************
 *  * ISR
 *******************************************************************************
*/
void __interrupt() isr(void){
}
/*******************************************************************************
 *  * fUNCION PRINCIPAL
 *******************************************************************************
*/
void main(){
   setup();
   while(1){
       RC7 = 0;       //Slave Select
       __delay_ms(1);
       
       spiWrite(0x12);
       PORTD = spiRead();
       
       __delay_ms(1);
       RC7 = 1;       //Slave Deselect 
       
   }
}
/*******************************************************************************
 *  * fUNCIONES
 *******************************************************************************
*/
   
void setup(void){
    OSCCON = 0b111;
    OSCCONbits.SCS = 1;
    //configuracion puertos
    TRISD = 0x00;                 //PORTD as output
    PORTD = 0x00;                 //All LEDs OFF
    PORTB = 0x00;
    TRISC7 = 0;
    RC7 = 1;
    ANSEL = 0;
    ANSELH = 0;
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    //perifericas y globales
    //INTCONbits.PEIE = 1;
    //INTCONbits.GIE = 1;
}
