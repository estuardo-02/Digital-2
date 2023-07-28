
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

#define _XTAL_FREQ 8000000

#include <xc.h>
#include "lcd_4bits.h"
#include "oscilador.h"
#include "adc_config.h"

//conversion de valor de potenciometro a decimal en string, formato x.xx
int centenas, decenas, unidades;

int conversion(int adc_val, int* centenas, int* decenas, int* unidades){
    adc_val = (int)(adc_val*2*0.981); //valor con 256 bits * valor normalizado a 500
    *centenas= adc_val/100;
    int residuo_cen = adc_val%100;
    *decenas = residuo_cen/10;
    int residuo_dec = adc_val%10;
    *unidades = residuo_dec;
    return 0;
}

int main(){
  setOscFrequency(7);
  configIntOsc();
  adc_init(2);
  unsigned int a, adc_val;
  TRISD = 0x00;
  Lcd_Init();
  while(1)
  {
    adc_val = adc_read();
    conversion(adc_val, &centenas, &decenas, &unidades);
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Pot 1: ");
    Lcd_Set_Cursor(2,1);
    Lcd_Write_Char(centenas+48);
    Lcd_Set_Cursor(2,2);
    Lcd_Write_Char('.');
    Lcd_Set_Cursor(2,3);
    Lcd_Write_Char(decenas+48);
    Lcd_Set_Cursor(2,4);
    Lcd_Write_Char(unidades+48);
    __delay_ms(2000);
  }
  return 0;
}