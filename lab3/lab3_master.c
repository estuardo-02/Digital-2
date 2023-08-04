/*
 * File:   lab3_master.c
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
#include "lcd_4bits.h"
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
int centenas, decenas, unidades;
int centenas2, decenas2, unidades2;

int conversion(int adc_val, int* centenas, int* decenas, int* unidades){
    adc_val = (int)(adc_val*2*0.981); //valor con 256 bits * valor normalizado a 500
    *centenas= adc_val/100;
    int residuo_cen = adc_val%100;
    *decenas = residuo_cen/10;
    int residuo_dec = adc_val%10;
    *unidades = residuo_dec;
    return 0;
}

void main(){
   setup();
   int slave_1_adc, slave_1_cont, slave_2;
   while(1){
              //comunicacion con esclavos 
       RC6 = 0;       //Slave Select
       __delay_ms(1);
       
       spiWrite(1); //valor de ejemplo para iniciar comunicacion
       slave_1_adc = spiRead();
       
       __delay_ms(1);
       RC6 = 1;       //Slave Deselect 
       RC7 = 0;       //Slave Select
       __delay_ms(1);
       
       spiWrite(1);
       slave_2 = spiRead();
       
       __delay_ms(1);
       RC7 = 1;       //Slave Deselect 
       //__delay_ms(1);
       Lcd_Clear();
       Lcd_Set_Cursor(1,1);
       Lcd_Write_String("S1: ");
       Lcd_Set_Cursor(1,6);
       Lcd_Write_String("S2: ");
       //escritura de valores de esclavo 1
        Lcd_Set_Cursor(2,1);
        conversion(slave_1_adc, &centenas, &decenas, &unidades);
        Lcd_Write_Char(centenas+48);
        Lcd_Set_Cursor(2,2);
        Lcd_Write_Char('.');
        Lcd_Set_Cursor(2,3);
        Lcd_Write_Char(decenas+48);
        Lcd_Set_Cursor(2,4);
        Lcd_Write_Char(unidades+48);
       //escritura de valores de esclavo 2
        Lcd_Set_Cursor(2,6);
        conversion(slave_2, &centenas2, &decenas2, &unidades2);
        Lcd_Write_Char(centenas2+48);
        Lcd_Set_Cursor(2,7);
        Lcd_Write_Char('.');
        Lcd_Set_Cursor(2,8);
        Lcd_Write_Char(decenas2+48);
        Lcd_Set_Cursor(2,9);
        Lcd_Write_Char(unidades2+48);
        //
        __delay_ms(1000);

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
    TRISD = 0x00;                 //PORTD as output
    PORTD = 0x00;                 //All LEDs OFF
    PORTB = 0x00;
    TRISC6 = 0; //puertos para habilitar esclavos
    TRISC7 = 0;
    RC7 = 1;
    RC6 = 1;
    ANSEL = 0;
    ANSELH = 0;
    Lcd_Init();
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    //perifericas y globales
    //INTCONbits.PEIE = 1;
    //INTCONbits.GIE = 1;
}
