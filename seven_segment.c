/*
 * File:   seven_segment.c
 * Author: Estuardo Castillo
 *
 * Created on July 14, 2023, 5:28 PM
 */

#include "seven_segment.h"
#include <xc.h>
#include <stdint.h>
int adc_val, decenas, unidades;
int tabla_hex[] = {0b11101111,
    0b10001100,
    0b11011011,
    0b11011110,
    0b10111100,
    0b01111110,
    0b01111111,
    0b11001100,
    0b11111111,
    0b11111110,
    0b11111101,
    0b00111111,
    0b01101011,
    0b10011111,
    0b01111011,
    0b01111001};
int hex_to_7seg(int val, int pos){
    conversion_hex(val, &decenas, &unidades);
    if(pos == 0){
    return tabla_hex[unidades];
    }
    if(pos == 1){
    return tabla_hex[decenas];
    }
}
int conversion_hex(int adc_val, int* decenas, int* unidades){
    *decenas = adc_val/16;
    int residuo_dec = adc_val%16;
    *unidades = residuo_dec;
    return 0;
}