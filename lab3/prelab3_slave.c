/*
 * File:   prelab3.c
 * Author: estua
 * Programa para pic esclavo/servidor
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
#include "ioc.h"
#define _XTAL_FREQ 4000000
/*******************************************************************************
 *  * PROTOTIPOS DE FUNCION
 *******************************************************************************
*/
void setup(void);
/*******************************************************************************
 *  * VARIABLES GLOBALES
 *******************************************************************************
*/
int contador, adc_val;

/*******************************************************************************
 *  * ISR
 *******************************************************************************
*/
void __interrupt() isr(void){

    if(SSPIF == 1)
    {
            spiWrite(adc_val);
        SSPIF = 0;
    }
}
/*******************************************************************************
 *  * fUNCION PRINCIPAL
 *******************************************************************************
*/
void main(){
   setup();
   while(1)
   {
       adc_val= adc_read();
   }
}
/*******************************************************************************
 *  * fUNCIONES
 *******************************************************************************
*/
   
void setup(void){
    //reloj
    OSCCONbits.IRCF = 0b110;
    OSCCONbits.SCS = 1;
    //configuracion puertos
    TRISB = 0x00;
    TRISD = 0x00;                
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0x00;
    adc_init(2);
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    TRISAbits.TRISA5 = 1; //slave select
    //perifericas y globales
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    //interrupciones de SPI
    SSPIF = 0;
    SSPIE = 1;
    //valor inicial de puertos
    PORTB = 0;
    PORTD = 0;
}