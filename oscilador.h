/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef OSCILADOR_H
#define	OSCILADOR_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
//******************************************************************************
//PROTOTIPOS DE FUNCIONES
//******************************************************************************
void setOscFrequency(int freq);
void configIntOsc(void);

#endif	/* OSCILADOR_H */


