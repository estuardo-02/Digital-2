/*
 * File:   main.c
 * Author: estua
 *
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
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
/*******************************************************************************
 *  * LIBRERIAS
 *******************************************************************************
*/
#include <xc.h>
#include "oscilador.h"
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
    if(INTCONbits.RBIF){ //Subrutina para interrupciones por cambio en B
            if(PORTBbits.RB0){
                PORTA++;
            }if(PORTBbits.RB1){
                PORTA--;
            }
        INTCONbits.RBIF = 0; //apagar bandera luego de interrupcion 
    }
}
/*******************************************************************************
 *  * fUNCION PRINCIPAL
 *******************************************************************************
*/
void main(void) {
    setup();
    //Loop infinito 
    while(1){
    }
    return;
}
/*******************************************************************************
 *  * fUNCIONES
 *******************************************************************************
*/
void setup(void){
    setOscFrequency(7);
    configIntOsc();
    //configuracion puerto A
    TRISA = 0x00;
    PORTA = 0x00;
    //configuracion puerto B 
    TRISB = 0b11;
    PORTB = 0;
    ANSEL = 0;
    ANSELH = 0;
        //configuracion para interrupciones, WPUB y IOCB de puerto B
    OPTION_REGbits.nRBPU = 0;
    WPUBbits.WPUB = 0b11;
    //interrupciones en cambio de estado en B
    INTCONbits.RBIE = 1;
    IOCBbits.IOCB0 = 1;
    IOCBbits.IOCB1 = 1;
    INTCONbits.RBIF = 0;
    //perifericas y globales
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    //valor inicial de puertos
    PORTB = 0;
}
