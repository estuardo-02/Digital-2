/*
 * File:   oscilador.c
 * Author: estua
 *
 * Created on July 14, 2023, 5:28 PM
 */

#include "oscilador.h"
#include <xc.h>
void setOscFrequency(int freq){
    OSCCON = OSCCON | freq;
}
void configIntOsc(void){
    OSCCONbits.SCS = 1;
}
