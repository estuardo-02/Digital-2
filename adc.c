/*
 * File:   adc.c
 * Author: Estuardo Castillo
 *
 * Created on July 14, 2023, 5:28 PM
 */

#include "adc_config.h"
#include <xc.h>
#define _XTAL_FREQ 1000000

adc_init(int channel){
    //configuracion de ADC
    ADCON1bits.ADFM = 0;
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON0bits.ADCS = 0b00;
    //configuracion de pin
    int var = var | (1<<channel);
    switch(channel){
        case 0:
            ANSEL = ANSEL | var;
            TRISA = TRISA | var;
            break;
        case 1:
            ANSEL = ANSEL | var;
            TRISA = TRISA | var;
            break;
        case 2:
            ANSEL = ANSEL | var;
            TRISA = TRISA | var;
            break;
        case 3:
            ANSEL = ANSEL | var;
            TRISA = TRISA | var;
            break;
        default:
            break;
    }
    //seleccion del canal
    ADCON0bits.CHS = channel;
    ADCON0bits.ADON = 1;
    __delay_us(50);
}

int adc_read (){
    // Iniciar conversion ADC
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    //__delay_us(100);  
    return ADRESH; //regresar valor convertido
}
adc_change_channel(int channel){
    ADCON0bits.CHS = channel; //canal para conversion
    __delay_us(50);
}
int adc_get_channel(){
    return ADCON0bits.CHS;
}