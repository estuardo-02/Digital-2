/*
 * File:   maestro_2.c
 * Author: Estuardo Castillo y Carlos Valdés
 *  Hardare necesario: 3 PICs servidores, cada uno conectado a su propio sensor y/o motores
 *  Pantalla LCD configurado con 4 bits. 
 *  Comunicación a 9600 baudios con ESP32 a través de EUSART.  
 *  Comunicacion i2c con resto de PICS a 100kHz
 */

// Palabra de configuracion
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
#pragma config WRT = OFF            // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Librerias
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include "eusart.h"
#include "i2c.h"
#include "lcd_4bits.h"

//*****************************************************************************
// Definicion de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
int centenas, decenas, unidades;
int centenas_soil, decenas_soil, unidades_soil;
int centenas_dist, decenas_dist, unidades_dist;
uint8_t baudrate = 51;
char RX;
char medidor_4[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00};
char medidor_3[] = {0xFF, 0xFF, 0xFF, 0x00};
char medidor_2[] = {0xFF, 0xFF, 0x00, 0x00};
char medidor_1[] = {0xFF, 0x00, 0x00, 0x00};
uint8_t medida;
float alturaTot = 17.0;
float alturaReal;
//*****************************************************************************
//Funciones
//*****************************************************************************
void setup(void);
//int ser_out (int selector, int adc_val);
int conversion(int adc_val, int* centenas, int* decenas, int* unidades){
    //adc_val = (int)(adc_val*2*0.981); //valor con 256 bits * valor normalizado a 500
    *centenas= adc_val/100;
    int residuo_cen = adc_val%100;
    *decenas = residuo_cen/10;
    int residuo_dec = adc_val%10;
    *unidades = residuo_dec;
    return 0;
}
//ISR
void  __interrupt() isr(void){
    if(PIR1bits.RCIF)     //Datos recibidos?
    {    
        RX =RCREG;
    }
}

//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    int soil_moist, temperatura, distancia;
    int soil_moist_mapped;
    int toggle = 0;
    int contador = 0; 
    while(1){ //loop

        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("Temp: Hum: Agua:");
        
        conversion(temperatura, &centenas, &decenas, &unidades);
        soil_moist_mapped = map (soil_moist, 0, 255, 0, 200);
        conversion(soil_moist_mapped, &centenas_soil, &decenas_soil, &unidades_soil);
        conversion(alturaReal, &centenas_dist, &decenas_dist, &unidades_dist);
        
        Lcd_Set_Cursor(2,1);
        Lcd_Write_Char(centenas+48);
        Lcd_Set_Cursor(2,2);
        Lcd_Write_Char(decenas+48);
        Lcd_Set_Cursor(2,3);
        Lcd_Write_Char(unidades+48);
        
        Lcd_Set_Cursor(2,7);
        Lcd_Write_Char(centenas_soil+48);
        Lcd_Set_Cursor(2,8);
        Lcd_Write_Char(decenas_soil+48);
        Lcd_Set_Cursor(2,9);
        Lcd_Write_Char(unidades_soil+48);
            
        if (medida >= 2 && medida <= 4){    
        Lcd_Set_Cursor(2,12);
        Lcd_Write_String(medidor_4); 
        }
        else if (medida > 4 && medida <= 8){
        Lcd_Set_Cursor(2,12);
        Lcd_Write_String(medidor_3); 
        }
        else if (medida > 8 && medida <= 11){  
        Lcd_Set_Cursor(2,12);
        Lcd_Write_String(medidor_2); 
        }
        else if (medida > 11 && medida <= 18){     
        Lcd_Set_Cursor(2,12);
        Lcd_Write_String(medidor_1); 
        }
        __delay_ms(200);
            //sensor de humedad 
        I2C_Master_Start();
        I2C_Master_Write(0x50);
        I2C_Master_Write(medida); //necesita este valor para conocer estado de tanque
        I2C_Master_Stop();
        __delay_ms(200);
        I2C_Master_Start();
        I2C_Master_Write(0x51);
        soil_moist = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);
        
        //sensor de temperatura
        I2C_Master_Start();
        I2C_Master_Write(0x40);
        I2C_Master_Write(PORTB);
        I2C_Master_Stop();
        __delay_ms(200);
        I2C_Master_Start();
        I2C_Master_Write(0x41);
        temperatura = I2C_Master_Read(0);
        I2C_Master_Stop();
        
        
        __delay_ms(200);
        
        //sensor ultrasonico

        I2C_Master_Start();
        I2C_Master_Write(0x31);
        medida = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);

        alturaReal = alturaTot - medida;      //calcular la altura del agua 
        if(alturaReal<0){
            alturaReal = 0;
        }
        
        contador++;
        //envio de datos, uno diferente cada ciclo 
        if(contador>3){
            contador = 0;
            switch(toggle){
                case 0: 
                    UART_READ(temperatura+48);
                    toggle = 1;
                    PORTB = 0x01;
                    break;
                case 1: 
                    UART_READ(soil_moist_mapped+48);
                    toggle = 2;
                    PORTB = 0x02;
                    break;
                case 2: 
                    UART_READ(alturaReal+48);
                    toggle = 0;
                    PORTB = 0x04;
                    break;
                default:
                    UART_READ(100);
                    PORTB = 0x03;
                    break;
            }
         }
        __delay_ms(50);    //estaba en 2800
    }
    return;
}
//*****************************************************************************
// setup
//*****************************************************************************
void setup(void){
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 1;
    
    ANSEL = 0;
    ANSELH = 0;
    TRISB = 0;
    TRISD = 0;
    TRISC = 0;
    PORTB = 0;
    PORTD = 0;
    PORTC = 0;
    I2C_Master_Init(100000);        // Inicializar Comunicacion 12c
    Lcd_Init();
    
    UART_RX_CONFIG(baudrate);
    UART_TX_CONFIG(baudrate);
    //interrupciones 
    INTCONbits.PEIE = 1; //Habilitar interrupciones perifericas
    INTCONbits.GIE = 1; // Habilita interrupciones generales
    
}
