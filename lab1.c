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
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
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
#include "oscilador.h"
#include "ioc.h"
#include "adc_config.h"
#define _XTAL_FREQ 1000000
/*******************************************************************************
 *  * PROTOTIPOS DE FUNCION
 *******************************************************************************
*/
void setup(void);
/*******************************************************************************
 *  * VARIABLES GLOBALES
 *******************************************************************************
*/
int selector;
/*******************************************************************************
 *  * ISR
 *******************************************************************************
*/
void __interrupt() isr(void){
    if(INTCONbits.RBIF){ //Subrutina para interrupciones por cambio en B
            if(PORTBbits.RB0){
                selector = ~selector;
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
        if(selector){
        PORTC = adc_read();
        }else{
        PORTC = adc_get_channel();
        }
    }
    return;
}
/*******************************************************************************
 *  * fUNCIONES
 *******************************************************************************
*/
void setup(void){
    setOscFrequency(4);
    configIntOsc();
    //configuracion puertos para despliegue de datos (salida)
    TRISC = 0;
    TRISD = 0;
    //configuracion puerto B 
    //configuracion para interrupciones, WPUB y IOCB de puerto B
    TRISB = 0;
    WPUB = 0;
    IOCB = 0;
    ioc_init(0); //puerto RB0
    ioc_init(1); //puerto RB1
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0;
    //configuracion del ADC
    adc_init(2); //configuracion para canal 0
    //interrupciones en cambio de estado en B
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    //perifericas y globales
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    //valor inicial de puertos
    PORTB = 0;
    PORTA = 0;
    PORTC = 0;
    PORTD = 0;
}