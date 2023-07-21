/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SEVEN_SEGMENT_H
#define	SEVEN_SEGMENT_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
//******************************************************************************
//PROTOTIPOS DE FUNCIONES
//******************************************************************************
int conversion_hex(int adc_val, int* decenas, int* unidades);
int hex_to_7seg(int val, int pos);

#endif	/* SEVEN_SEGMENT_H */
