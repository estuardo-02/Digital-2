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

#endif	/* ADC_CONFIG_H */

