/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef IOC_H
#define	IOC_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
//******************************************************************************
//PROTOTIPOS DE FUNCIONES
//******************************************************************************
void ioc_init(char pin);

#endif	/* IOC_H */
