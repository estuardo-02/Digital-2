/*
 Archivo: dht11.c
 * Modificado de c[odigo original por Giovanny Anglas
 * 
 */

#include <xc.h>
#include <stdint.h>
#include "dht11.h"
#define _XTAL_FREQ 8000000
////////////////////////////////// ENVIAMOS LA SE�AL DE START HACIA EL DHT11 ////////////////////////////////////////////////////////////////////////////////////
void StartSignal(){                                                                                                                                            //
     TRIS_DHT11=0;   //configuro el puerto de DHT11 como salida                                                                                                       //
     PORT_DHT11=0;        //envio 0 hacia el sensor //                                                                                                          //
     __delay_ms(18);       //durante 18ms                                                                                                                        //
     PORT_DHT11=1;        //envio un 1 hacia el sensor                                                                                                          //
     __delay_us(30);       //espero la respuesta del sensor                                                                                                   //
     TRIS_DHT11=1;}  ///configuro el puerto de DHT11 como entrada                                                                                                  //
////////////////////////////////// RESPUESTA DEL SENSOR DHT11 ///////////////////////////////////////////////////////////////////////////////////////////////////
void CheckResponse(){                                                                                                                                          //
     check=0;                 //creo una variable q solo sirve para ver si el dht11 respondio o esta teniendo problemas                                        //
     __delay_us(40);            //espero 40us (la mitad de 80us es decir la trama central del nivel bajo)                                                        //
     if(PORT_DHT11==0){           //consulto si el DHTPIN esta a nivel bajo                                                                                     //
        __delay_us(80);         //espero 80us(trama central del nivel alto)                                                                                      //
        if(PORT_DHT11==1){        //consulto si el DHTPIN esta a nivel alto                                                                                     //
           check=1;           //la comunicacion es correcta y espero a leer los 40bits del sensor                                                              //
           __delay_us(40);}}}   //completo los 40us de la trama de nivel alto                                                                                    //
////////////////////////////////// LECTURA DE DATOS DEL DHT11 ///////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t ReadData(){                 // lectura de 8bits o 1Byte o una trama de datos                                                                              //
     uint8_t i,j;                   //i=0b00000000=0x00=0                                                                                                         //
     for(j=0;j<8;j++){           //8bits                                                                                                                       //
         while(!PORT_DHT11);         //espera hasta que la entrada de DHTPIN sea un alto aprox 50us segun el datasheet                                          //
         __delay_us(30);           // tiempo de espera segun el datasheet 26-28us, le agrego 2us mas para dejarle pasar al estado bajo                           //
         //si leyo un cero, es decir que el pulso de 26-28us se dio y paso a bajo (la cual le dimos 2us mas de seguridad)                                      //
         if(PORT_DHT11==0){          //si despues del puslo de 26-28us paso a cero que quiere decir que transmitio un "0"logico                                 //
            i=(uint8_t)(i & ~(1<<(7-j)));} //entonces escribe un "0"(23-28us datasheet) en la posicion (7-j) de i                                                        //
         //si leyo un uno, es decir que el pulso de 26-28us se dio y no paso a bajo (la cual debe ser un pulso mayor a 26-28us)                                //
         else{                   //si despues del puslo de 26-28us no paso a cero que quiere decir que transmitio un "1"logico o un pulso de 70us              //
            i=(uint8_t)(i | (1<<(7-j)));   //entonces escribe un "1" en la posicion (7-j) de i                                                                           //
            while(PORT_DHT11);}}     //espera hasta que la entrada de DHTPIN sea un bajo                                                                        //
     return i;}                  //retorna el valor leido, para que i no sea cero nuevamente                                                                   //
                                                                                                           //
void Leer_DHT11(uint8_t *temperatura, uint8_t *humedad){ //creo dos variables tipo punteros(*)enteras                                                                  //
     uint8_t datosDHT11[5],k;               //variable para almacenar los 5bytes ----- variable para la iteracion de los 5bytes                                   //
     StartSignal();                      //enviamos la funcion de start                                                                                        //
     CheckResponse();                    //esperamos la respuesta del sensor                                                                                   //
     if(check==1){                       //si el sensor contesta:                                                                                     //
        for(k=0;k<5;k++){                //leemos los 5bytes                                                                                                   //
            datosDHT11[k]= ReadData();}} //los almacenamos en la matriz datosDHT11[k]                                                                          //
     if(datosDHT11[4] == (datosDHT11[0]+datosDHT11[1]+datosDHT11[2]+datosDHT11[3])){   //verifico bit de pariedad, en caso contrario se desecha la lectura 
        *humedad     = datosDHT11[0];  //guarda los bits MS de la lectura.                                                     //
        *temperatura = datosDHT11[2];}}                              //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
