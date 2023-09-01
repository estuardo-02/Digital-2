/*
 * File:   slave_sistema_riego.c
 * Author: Estuardo Castillo
 * Hardware necesario: Integrado L293D de puente H, motor DC y sensor de humedad YL-38
 * Comunicacion 12c a 100kHz, dirección 50
 *
 */
// Palabra de configuraci�n
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
        
        //LIbrerias
#include <xc.h>
#include <stdint.h> //para uint8_t,uint16_t,etc
#include "i2c.h"
#include "adc_config.h"
#define _XTAL_FREQ 8000000 //frecuencia de cristal


//variables para comunicacion i2c
uint8_t z;
uint8_t dato;
int soil_moist;
int nivel_agua;

//definicion de pines para control de puente H
#define OUT_R PORTDbits.RD0 //senal para sentido horario
#define OUT_L PORTDbits.RD2 //senal para sentido antihorario
#define EN PORTDbits.RD1 //enable

//funciones 
void setup(void);

void __interrupt() interrupcion(){//cada 1ms
if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupci�n recepci�n/transmisi�n SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepci�n se complete
            nivel_agua = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = soil_moist; //envio de datos a maestro
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
}

void main(void) {
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 1;
    //config I/O
    ANSEL = 0;
    ANSELH = 0;
    TRISD = 0;
    TRISC = 0;
    TRISA = 0;
    PORTD = 0;
    PORTC = 0;
    adc_init(2);
    I2C_Slave_Init(0x50); //toma como parametro la direccion del esclavo  
    int soil_moist_mapped; 
    while(1){
        soil_moist = adc_read();  //lectura del sensor de humedad en tierra
        soil_moist_mapped = map (soil_moist, 0, 255, 0, 200);
        //soil_moist = map(soil_moist, 0, 511, 0, 100);
        if(soil_moist_mapped>190 && nivel_agua < 18){
            EN = 1;
            OUT_R = 1;
            OUT_L - 0;
            PORTDbits.RD3 = 1;
        }else{
            EN = 0;
            OUT_R = 0;
            OUT_L - 0;
            PORTDbits.RD3 = 0;
        }
    }   
}
