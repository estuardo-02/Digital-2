/* 
 * File:   adc_config.h
 * Author: Estuardo Castillo
 * Comments: dependencias: adc.c
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ADC_CONFIG_H
#define	ADC_CONFIG_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
//******************************************************************************
//PROTOTIPOS DE FUNCIONES
//******************************************************************************
adc_init(int channel);
int adc_read ();
adc_change_channel(int channel);
int adc_get_channel();
int map (int val, int minx, int maxx, int miny, int maxy);
float MAP_2(uint8_t IN, uint8_t INmin, uint8_t INmax, uint8_t OUTmin, uint8_t OUTmax);
#endif	/* ADC_CONFIG_H */

