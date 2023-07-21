/*
 * File:   postlab1.c
 * Author: Estuardo Castillo
 * Codigo para contador hexadecimal con display de 7 segmentos en puerto C
 * Multiplexeado con transsitores en puerto E, entrada analoga RA2
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
#include <stdint.h>
#include "oscilador.h"
#include "ioc.h"
#include "adc_config.h"
#include "seven_segment.h"
/*******************************************************************************
 *  * PROTOTIPOS DE FUNCION
 *******************************************************************************
*/
void setup(void);
/*******************************************************************************
 *  * VARIABLES GLOBALES
 *******************************************************************************
*/
#define _XTAL_FREQ 1000000
#define _tmr0_value 220
int display = 0b00;
int selector;
int adc_val, conversion;
/*******************************************************************************
 *  * ISR
 *******************************************************************************
*/
void __interrupt() isr(void){
    if(T0IF == 1){
        switch (display){ //seleccion de display
        PORTE = 0;
            case 0b00:
                display = 0b01;
                PORTC = hex_to_7seg(adc_read(), 0);
                //asigna a puerto valor de lectura y la posicion que se requiere
                PORTE = 0b01;
                break;
            case 0b01:
                display = 0b00;
                PORTC = hex_to_7seg(adc_read(), 1);
                PORTE = 0b10;
                break;
            default:
                break;
        }
    TMR0 = _tmr0_value; //asignar valor de precarga
    INTCONbits.TMR0IF = 0; //limpiar bandera 
    }
}
/*******************************************************************************
 *  * fUNCION PRINCIPAL
 *******************************************************************************
*/
void main(void) {
    setup();
    //Loop infinito 
    int referencia = 0xF0;
    while(1){
        adc_val = adc_read();
        if(adc_val>referencia){
            PORTCbits.RC3 = 1;
        }else{
            PORTCbits.RC3 = 0;
        }
    }
    return;
}
/*******************************************************************************
 *  * FUNCIONES
 *******************************************************************************
*/
void setup(void){
    setOscFrequency(4);
    configIntOsc();
    //configuracion puertos para despliegue de datos (salida)
    TRISC = 0;
    TRISE = 0;
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
    adc_init(2); //configuracion para canal 2, entra como parametro para RA#
    //valor inicial de puertos
    PORTB = 0;
    PORTA = 0;
    PORTC = 0;
    PORTE = 0;
    //configuración del tmr0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.T0SE = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b111; //256 scaler
    TMR0 = _tmr0_value;
    //configuracion para interrupción
    INTCON = 0;
    INTCONbits.TMR0IE = 1;
    //interrupciones en cambio de estado en B
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    //perifericas y globales
    //INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    INTCONbits.T0IF = 0; //bandera en 0
    
    return;
}
