/*
 * File:   slave_sistema_clima.c
 * Author: Estuardo Castillo
 * Hardware necesario: Servo Motor y sensor de temperatura DHT11
 * Comunicacion 12c a 100kHz, dirección 40
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
#include "dht11.h"
#define _XTAL_FREQ 8000000 //frecuencia de cristal

#define servo_pin PORTDbits.RD2

//variables para comunicacion i2c
uint8_t z;
uint8_t dato;
int temp, hum;
int pulse_width;
//funciones 
void setup(void);
void TMR0_Init();
void TMR1_Init();

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
            PORTD = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = temp; //envio de datos a maestro
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
    if(T0IF){	            // TIMER0 Interrupt Flag
            servo_pin = 1;
            TMR1H = 0xFE;
            TMR1L = pulse_width;
            T1CONbits.TMR1ON = 1; //deja correr tmr1
            TMR0 = 99;             // Valor inicial para interrupciones cada 0.1ms
            T0IF = 0;	// se limpia bandera para siguiente int
        }
    if(TMR1IF){
            servo_pin = 0;
            T1CONbits.TMR1ON = 0; //apaga tmr1 mientras no se ejecute y espera a siguiente int de tmr0
            TMR1IF = 0; //se limpia bandera para siguiente int
        }
}

void main(void) {
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 1;
    //config I/O
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0;
    TRISD = 0;
    PORTA = 0;
    I2C_Slave_Init(0x40); //toma como parametro la direccion del esclavo 
    TMR0_Init();
    TMR1_Init();
    INTCONbits.GIE    = 1; //Habilito las interrupciones gloabales
    INTCONbits.PEIE   = 1; //Habilito  interrupciones internas o de perifericos (ADC,USB,USART,ETC)
    while(1){
        __delay_ms(50);
        Leer_DHT11(&temp,&hum);
        if(temp>27){
            pulse_width = 0;
        }else{
            pulse_width = 0xFF;
        }
        __delay_ms(1200);
        }
}   
void TMR0_Init(){ //tiempo para cada 1ms aprox para la lectura del encoder
    //Timer0 Registers Prescaler= 16 - TMR0 Preset = 131 - Freq = 1000.00 Hz - Period = 0.001000 seconds
    OPTION_REGbits.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.T0SE = 0;  // bit 4 TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0;   // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 1;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
    TMR0 = 99;    //20ms         preset for timer register
    INTCONbits.TMR0IE = 1; //habilito la interrupcion por TMR0  
    INTCONbits.TMR0IF = 0;      //limpio bandera
}
void TMR1_Init(){
    //Timer1 Registers Prescaler= 1 - TMR1 Preset = 63313 - Freq = 112.46 Hz - Period = 0.008892 seconds
    T1CONbits.T1CKPS1 = 1;   // bits 5-4  Prescaler Rate Select bits
    T1CONbits.T1CKPS0 = 1;   // bit 4
    T1CONbits.T1OSCEN = 0;   // bit 3 Timer1 Oscillator Enable Control bit 1 = on
    T1CONbits.T1SYNC = 1;    // bit 2 Timer1 External Clock Input Synchronization Control bit...1 = Do not synchronize external clock input
    T1CONbits.TMR1CS = 0;    // bit 1 Timer1 Clock Source Select bit...0 = Internal clock (FOSC/4)
    T1CONbits.TMR1ON = 1;    // bit 0 enables timer
    TMR1H = 0xFE;             // preset for timer1 MSB register
    TMR1L = 0x00; 
    PIR1bits.TMR1IF = 0;            // clear timer1 interupt flag TMR1IF
    PIE1bits.TMR1IE  =   1;         // enable Timer1 interrupts
}