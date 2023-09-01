/*
 * File:   main.c
 * Author: Carlos Valdes
 * Uso de I2C Slave
 * Created on 06 de agosto de 2023
 * Codigo basado de: https://electrosome.com/hc-sr04-ultrasonic-sensor-pic/
 */
//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
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
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <stdint.h>
#include <pic16f887.h>
#include "i2c.h"
#include "ioc.h"
#include <xc.h>
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t z;
uint8_t dato;
volatile int a;
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void){
   if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        /*if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            PORTD = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }else*/ if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = PORTD;             //cargar el valor del volumen para enviarlo
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
   
        if(INTCONbits.RBIF == 1)                       //INTERRUPCION PORTB On-Change
         {
         INTCONbits.RBIE = 0;                         //Deshabilitar On-change mientras estoy en la interrupcion
         if(PORTBbits.RB4 == 1)                      //Si ECHO es HIGH
            T1CONbits.TMR1ON = 1;                       //Empezar Timer1
         
            if(PORTBbits.RB4 == 0)                      //Si ECHO es LOW
            {
             T1CONbits.TMR1ON = 0;                     //Detener Timer1
             a = (TMR1L | (TMR1H<<8))/58.82;           //Calcular distancia
            }
         }
        INTCONbits.RBIF = 0;                           //Limpiar banderas
        INTCONbits.RBIE = 1;
   
}
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    
    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    T1CON = 0x10;  //Initialize Timer Module
    while(1){
        PORTD = a;
       
        TMR1H = 0;                        //Poner valores iniciales del timer
        TMR1L = 0;                        //Poner valores iniciales del timer
        
        RB0 = 1;                            //TRIGGER HIGH
         __delay_us(10);                   //10uS Delay
         RB0 = 0;                          //TRIGGER LOW 

         __delay_ms(100);                  //Esperar a ECHO
        a = (a + 1)*2;                        //Factor de correccion
        
        /*if (a>=2 && a<=400){
            a = a;
        }
        else{
            a=0;
        }*/
        
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 1;
    
    ANSEL= 0b00000000; //configurar los pines de los POT
    ANSELH = 0;
    
    TRISBbits.TRISB0 = 0;   //RB0 TRIGGER            
    //TRISBbits.TRISB4 = 1;   //RB4 (ECHO)
    
    TRISD = 0;
    
    PORTB = 0;
    PORTD = 0;
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
    
    I2C_Slave_Init(0x30);
    ioc_init(4);
}
