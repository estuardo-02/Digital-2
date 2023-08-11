/*
 * File:   oscilador.c
 * Author: estua
 *
 * Created on July 14, 2023, 5:28 PM
 */

#include "ioc.h"
#include <xc.h>
void ioc_init(char pin){
    int var = var | (1<<pin);
    //configuracion para interrupcion en cambio para puerto B pagina 49 manual
    TRISB = TRISB | var;
        //configuracion para interrupciones, WPUB e IOCB de puerto B
    OPTION_REGbits.nRBPU = 0;
    WPUB = WPUB | var;
    //interrupciones en cambio de estado en B
    IOCB = IOCB | var;
    //interrupciones en cambio de estado en B
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
}
