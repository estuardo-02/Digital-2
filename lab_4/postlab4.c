/*
 * File:   postlab4.c
 * Author: estua
 *
 * Created on August 6, 2023, 12:36 PM
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

//*****************************************************************************
// Librerias
//*****************************************************************************
#include <stdint.h>
#include <pic16f887.h>
#include "i2c.h"
#include "lcd_4bits.h"
#include "ioc.h"
#include <xc.h>

//*****************************************************************************
// Variables globales
//*****************************************************************************
#define _XTAL_FREQ 8000000
int limite = 0;
int registro = 0;
int registro_temp = 0;
int modo = 1;

void setup(void);
void DS3231_write(int reg, int val);
void DS3231_inc_reg(int up_limit, int reg);
/*******************************************************************************
 *  * ISR
 *******************************************************************************
*/
void __interrupt() isr(void){
    if(INTCONbits.RBIF){ //Subrutina para interrupciones por cambio en B
        if(PORTBbits.RB0){
            //while(PORTBbits.RB0);
                if(modo == 2){
                registro_temp++;
                DS3231_inc_reg(limite, registro);
                }
         }if(PORTBbits.RB1){
             //while(PORTBbits.RB1);
                modo = 2;
                registro++;
            }
        INTCONbits.RBIF = 0; //apagar bandera luego de interrupcion 
    } 
}

//*****************************************************************************
// Definici�n de funciones para que se puedan colocar despu�s del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************

int get_DS3231(int reg){
    int val;
    I2C_Master_Start();
    I2C_Master_Write(0xD0); //direccion de RTC (escribo desde maestro)
    I2C_Master_Write(reg); //direccion del registro del RTC DS3231 (segundos)
    I2C_Master_Stop();
        __delay_ms(20);
    I2C_Master_Start();
    I2C_Master_Write(0xD1); //direccion de RTC (ahora leo desde maestro)
    val = I2C_Master_Read(0); //con Acknowledge activado
    I2C_Master_Stop();
        __delay_ms(20);
    return val;
}
void DS3231_write(int reg, int val){
    I2C_Master_Start();
    I2C_Master_Write(0xD0); //direccion de RTC (escribo desde maestro)
    I2C_Master_Write(reg); //direccion del registro del RTC DS3231 (segundos)
    I2C_Master_Write(val); //direccion del registro del RTC DS3231 (segundos)
    I2C_Master_Stop();
        __delay_ms(20);
}
/*
void DS3231_inc_reg(int up_limit, int reg){
    int msb, lsb;
    int val = get_DS3231(reg);
    msb = (val>>4) & 0b00001111;
    lsb = val & 0b00001111;
        if(lsb != 9){
            lsb= lsb + 1;
            DS3231_write(reg, ((msb<<4)+lsb));
        }else if(msb < up_limit){
            msb = msb+1;
            if(msb == up_limit){
                msb = 0;
            }
            lsb = 0;
            DS3231_write(reg, ((msb<<4)+lsb));
        }
}*/
void DS3231_inc_reg(int up_limit, int reg){
    int msb, lsb;
    int val = get_DS3231(reg);
    msb = (val>>4) & 0b00001111;
    lsb = val & 0b00001111;
    
    int limit_msb = (up_limit/10) & 0b00001111;
    int limit_lsb = up_limit & 0b00001111;
    if(msb<limit_msb){
       lsb++;
       if(lsb == 9){
           msb++;
           lsb = 0;
       }
    }if(msb == limit_msb){
        lsb++;
        if(lsb == limit_lsb){
            msb = 0;
            lsb = 0;
        }
    }
    DS3231_write(reg, ((msb<<4)+lsb));
}
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    int s1_read;
    int segundos, seg_lsb, seg_msb;
    int minutos, min_lsb, min_msb;
    int hora, hora_lsb, hora_msb, pm_mode;
    int fecha, fecha_lsb, fecha_msb; 
    int mes, mes_lsb, mes_msb; 
    int ano, ano_lsb, ano_msb; 
    modo = 1;
    while(1){
        switch(modo){
            case 1:
                //display de datos
                //conversion_segs(segundos, &decenas, &unidades);
                segundos = get_DS3231(0x00);
                minutos = get_DS3231(0x01);
                hora = get_DS3231(0x02);
                fecha = get_DS3231(0x04);
                mes = get_DS3231(0x05);
                ano = get_DS3231(0x06);

                seg_msb = (segundos>>4) & 0b00001111;
                seg_lsb = segundos & 0b00001111;

                min_msb = (minutos>>4) & 0b00001111;
                min_lsb = minutos & 0b00001111;

                hora_msb = (hora>>4) & 0b00000011;
                hora_lsb = hora & 0b00001111;

                fecha_msb = (fecha>>4) & 0b00000011;
                fecha_lsb = fecha & 0b00001111;

                mes_msb = (mes>>4) & 0b00000011;
                mes_lsb = mes & 0b00001111;

                ano_msb = (ano>>4) & 0b00000011;
                ano_lsb = ano & 0b00001111;

                pm_mode = hora & 0b01000000;
                //comunicacion con pic esclavo
                I2C_Master_Start();
                I2C_Master_Write(0x50);
                I2C_Master_Write(PORTB);
                I2C_Master_Stop();
                __delay_ms(200);

                I2C_Master_Start();
                I2C_Master_Write(0x51);
                s1_read = I2C_Master_Read(0);
                I2C_Master_Stop();
                __delay_ms(200);
                Lcd_Clear();
                //datos de sensor
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("S1:");
                Lcd_Set_Cursor(2,1);
                Lcd_Write_Char(s1_read+48);
                //fecha actual
                Lcd_Set_Cursor(2,5);
                Lcd_Write_Char(fecha_msb+48);
                Lcd_Set_Cursor(2,6);
                Lcd_Write_Char(fecha_lsb+48);
                Lcd_Set_Cursor(2,7);
                Lcd_Write_Char('/');
                Lcd_Set_Cursor(2,8);
                Lcd_Write_Char(mes_msb+48);
                Lcd_Set_Cursor(2,9);
                Lcd_Write_Char(mes_lsb+48);
                Lcd_Set_Cursor(2,10);
                Lcd_Write_Char('/');
                Lcd_Set_Cursor(2,11);
                Lcd_Write_Char(ano_msb+48);
                Lcd_Set_Cursor(2,12);
                Lcd_Write_Char(ano_lsb+48);
                //hora actual
                Lcd_Set_Cursor(1,5);
                Lcd_Write_Char(hora_msb+48);
                Lcd_Set_Cursor(1,6);
                Lcd_Write_Char(hora_lsb+48);
                Lcd_Set_Cursor(1,7);
                Lcd_Write_Char(':');
                Lcd_Set_Cursor(1,8);
                Lcd_Write_Char(min_msb+48);
                Lcd_Set_Cursor(1,9);
                Lcd_Write_Char(min_lsb+48);
                Lcd_Set_Cursor(1,10);
                Lcd_Write_Char(':');
                Lcd_Set_Cursor(1,11);
                Lcd_Write_Char(seg_msb+48);
                Lcd_Set_Cursor(1,12);
                Lcd_Write_Char(seg_lsb+48);

                __delay_ms(40);

                break;
            case 2: 
                //while(write_en){
                    switch(registro){
                        case 0:
                            registro_temp = get_DS3231(registro);
                            seg_msb = (registro_temp>>4) & 0b00001111;
                            seg_lsb = registro_temp & 0b00001111;
                            Lcd_Clear();
                            Lcd_Set_Cursor(1,11);
                            Lcd_Write_Char(seg_msb+48);
                            Lcd_Set_Cursor(1,12);
                            Lcd_Write_Char(seg_lsb+48);
                            __delay_ms(500);
                            break;
                        case 1: 
                            limite = 59;
                            registro_temp = get_DS3231(registro);
                            min_msb = (registro_temp>>4) & 0b00001111;
                            min_lsb = registro_temp & 0b00001111;
                            Lcd_Clear();
                            Lcd_Set_Cursor(1,8);
                            Lcd_Write_Char(min_msb+48);
                            Lcd_Set_Cursor(1,9);
                            Lcd_Write_Char(min_lsb+48);
                            __delay_ms(500);
                            break;
                        case 2: 
                            limite = 25;
                            registro_temp = get_DS3231(registro);
                            hora_msb = (registro_temp>>4) & 0b00001111;
                            hora_lsb = registro_temp & 0b00001111;
                            Lcd_Clear();
                            Lcd_Set_Cursor(1,5);
                            Lcd_Write_Char(hora_msb+48);
                            Lcd_Set_Cursor(1,6);
                            Lcd_Write_Char(hora_lsb+48);
                            __delay_ms(500);
                            break;
                        case 3:
                            registro = 4;
                        case 4: 
                            limite = 30;
                            registro_temp = get_DS3231(registro);
                            fecha_msb = (registro_temp>>4) & 0b00001111;
                            fecha_lsb = registro_temp & 0b00001111;
                            Lcd_Clear();
                            Lcd_Set_Cursor(2,5);
                            Lcd_Write_Char(fecha_msb+48);
                            Lcd_Set_Cursor(2,6);
                            Lcd_Write_Char(fecha_lsb+48);
                            __delay_ms(500);
                            break;
                        case 5: 
                            limite = 12;
                            registro_temp = get_DS3231(registro);
                            mes_msb = (registro_temp>>4) & 0b00001111;
                            mes_lsb = registro_temp & 0b00001111;
                            Lcd_Clear();
                            Lcd_Set_Cursor(2,8);
                            Lcd_Write_Char(mes_msb+48);
                            Lcd_Set_Cursor(2,9);
                            Lcd_Write_Char(mes_lsb+48);
                            __delay_ms(500);
                            break;
                        case 6: 
                            limite = 99;
                            registro_temp = get_DS3231(registro);
                            ano_msb = (registro_temp>>4) & 0b00001111;
                            ano_lsb = registro_temp & 0b00001111;
                            Lcd_Clear();
                            Lcd_Set_Cursor(2,11);
                            Lcd_Write_Char(ano_msb+48);
                            Lcd_Set_Cursor(2,12);
                            Lcd_Write_Char(ano_lsb+48);
                            __delay_ms(500);
                            break;
                        case 7: 
                            modo = 1;
                            registro = 0;
                            break;
                        default:
                            registro = 0;
                            modo = 1;
                            break;
                    }
                //}
                break;
            default:
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("default");
                break;
         }
    }
    return;
}
//*****************************************************************************
// Funci�n de Inicializaci�n
//*****************************************************************************
void setup(void){
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 1;
    ANSEL = 0;
    ANSELH = 0;
    TRISD = 0;
    TRISC = 0;
    PORTD = 0;
    PORTC = 0;
    //configuracion puertos B 
    TRISB = 0b0011;
    PORTB = 0;
    //OPTION_REGbits.nRBPU = 0;
    //WPUBbits.WPUB = 0b0111;
    //interrupciones en cambio de estado en B
    INTCONbits.RBIE = 1;
    IOCBbits.IOCB0 = 1;
    IOCBbits.IOCB1 = 1;
    //IOCBbits.IOCB2 = 1;
    //perifericas y globales
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    I2C_Master_Init(100000);        // Inicializar Comuncaci�n I2C
    Lcd_Init();
}
