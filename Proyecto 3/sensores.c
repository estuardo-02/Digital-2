//**************************************************************************************************************
/**
 * PROYECTO FINAL (PARQUEO-MATIC)
 * ELECTRONICA DIGITAL 2
 * CARLOS JULIO VALDÉS #21110 & ESTUARDO CASTILLO #21559
 * main.c
 */
//**************************************************************************************************************
// Librerias
//**************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/pin_map.h"

#define XTAL 16000000

//**************************************************************************************************************
// Variables Globales
//**************************************************************************************************************
uint8_t charIn = 'a';
uint32_t ValPot = 0;
uint8_t flag;
uint8_t parking_state = 0;
uint32_t i = 0;
uint32_t ui32Period;

//**************************************************************************************************************
// PROTOTIPOS DE FUNCIONES
//**************************************************************************************************************
void delay(uint32_t msec);
void delay1ms(void);
void InitUART(void);
void ValorPotenciometro(void);
void Timer0Config(void);

//**************************************************************************************************************
// CODIGO PRINCIPAL
//**************************************************************************************************************
int main(void)
{
    //************************************************************
    //------------------------ RELOJ -----------------------------
    //************************************************************
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //SE SETEA EL RELOJ INTERNO A 16MHZ

    InitUART();                                        // SE INICIALIZA EL UART0

    //************************************************************
    //--------------------- PUERTOS A USAR ------------------------
    //************************************************************
    // Se asigna reloj a los puertos
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);        // SE ENCIENDE EL PUERTO A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);        // SE ENCIENDE EL PUERTO B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);        // SE ENCIENDE EL PUERTO C
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);        // SE ENCIENDE EL PUERTO D
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);        // SE ENCIENDE EL PUERTO E
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF);       //SE ENCIENDE PUERTO F Y RGB COMO SALIDA
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    //************************************************************
    //-------------------------GPIOS------------------------------
    //************************************************************

    //LEDS (SALIDAS)
    //VERDES
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    //ROJOS
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    //PINES DE LAS FOTO (ENTRADAS)
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); //PB6 y PB7 foto 2 & 3
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_6);

    //SE CONFIGURAN LOS PULLUPS
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //************************************************************
    //-------------------------TMR0------------------------------
    //************************************************************
    Timer0Config();


    //************************************************************
    //------------------------ UART0 -----------------------------
    //************************************************************
    //IntEnable(INT_UART0);                                           // SE HABILITA EL UART0
    //UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8); // SE HABILIDA EL FIFO EN 1/8 DE DATOS RECIBIDOS
    //UARTIntEnable(UART0_BASE, UART_INT_RX);                         // SE HABILITAN LAS INTERRUPCIONES

    //************************************************************
    //------------------------ ADC0 ------------------------------
    //************************************************************
    //SE CONFIGURA EL TMR1 PARA QUE ACTUE COMO INTERRUPCION PARA EL ADC
    /*SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);                   // SE ENCIENDE EL TIMER1
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);                // SE CONFIGURA PARA QUE SEA PERIODICO
    TimerLoadSet(TIMER1_BASE, TIMER_A, 1000);                       // SE CARGA EL VALOR DEL TIMER1
    TimerControlTrigger(TIMER1_BASE, TIMER_A, true);                // SE CONFIGURA PARA QUE FUNCIONE COMO INTERRUPCION PAL ADC
    TimerEnable(TIMER1_BASE, TIMER_A);                              // SE HABILITA EL TIMER1A

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);                                              // SE ENCIENDE EL ADC0
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);                                             // SE CONFIGURA EL PE1 PARA QUE SEA ADC
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD); // SE CONFIGURA EL PE1 PARA QUE SEA ENTRADA
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_TIMER, 1);                                // SE CONFIGURA LA SECUENCIA DEL ADC0
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);       // SE CONFIGURA LA SECUENCIA DE PASOS DEL ADC0
    ADCSequenceEnable(ADC0_BASE, 3);                                                         // SE HABILITA LA SECUENCIA DEL ADC0
    ADCIntEnable(ADC0_BASE, 3);                                                              // SE HABILITAN LAS INTERRUPCIONES PARA EL ADC0
    ADCIntRegister(ADC0_BASE, 3, ValorPotenciometro);                                        // SE CONFIGURA LA INTERRUPCION DEL ADC0
    IntEnable(INT_ADC0SS3);                                                                  // SE HABILITAN LAS INTERRUPCIONES PARA EL ADC0 SS3*/

    IntMasterEnable();                                              // SE HABILITAN LAS INTERRUPCIONES GLOBALES

    //**********************************************************************************************************
    // Loop Principal
    //**********************************************************************************************************
    while (true)
    {
        if (flag == 1){
            GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7, 0);
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 0);
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);

            flag = 0;
        }

        else{

        flag = 1;

        if (GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_5) == 0 ){  //FOTO 1
                 GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7);

                    parking_state |= (1<<0);            //encender primer bit de la variable

                }
                else{
                    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);

                    parking_state &= ~(1<<0);         //apagar el primer bit de la variable
                }

        if (GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_7) == 0 ){  //FOTO 2
                        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PIN_0);

                        parking_state |= (1<<1);            //encender 2do bit de la variable
                       }
                       else{
                           GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_4);

                           parking_state &= ~(1<<1);         //apagar el 2do bit de la variable
                       }

        if (GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_6) == 0 ){  //FOTO 3
                        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);

                        parking_state |= (1<<2);            //encender 3er bit de la variable
                       }
                       else{
                           GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_PIN_5);

                           parking_state &= ~(1<<2);         //apagar el 3er bit de la variable
                       }

        if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_4) == 0 ){  //FOTO 4
                        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);

                          parking_state |= (1<<3);            //encender 4to bit de la variable
                       }
                       else{
                           GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_PIN_6);

                           parking_state &= ~(1<<3);         //apagar el 4to bit de la variable
                       }

        if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_5) == 0 ){  //FOTO 5
                        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_3);

                        parking_state |= (1<<4);            //encender 5to bit de la variable
                       }
                       else{
                           GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_PIN_7);

                        parking_state &= ~(1<<4);         //apagar el 4to bit de la variable
                       }

        if (GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_4) == 0 ){  //FOTO 6
                        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);

                        parking_state |= (1<<5);            //encender 6to bit de la variable
                       }
                       else{
                           GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_PIN_6);

                        parking_state &= ~(1<<5);         //apagar el 6to bit de la variable
                       }

        if (GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5) == 0 ){  //FOTO 7
                        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);

                        parking_state |= (1<<6);            //encender 7mo bit de la variable
                       }
                       else{
                           GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_7, GPIO_PIN_7);

                        parking_state &= ~(1<<6);         //apagar el 7mo bit de la variable
                       }

        if (GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6) == 0 ){  //FOTO 8
                        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_PIN_3);

                        parking_state |= (1<<7);            //encender 8vo bit de la variable
                       }
                       else{
                           GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_PIN_2);
                        parking_state &= ~(1<<7);         //apagar el 8vo bit de la variable
                       }

        }

        //UARTCharPut(UART0_BASE, 65);    //Mandar el byte con el estado de los 8 parqueos
        //delay(100);

    }
}
//**************************************************************************************************************
// INICIALIZACION DEL UART
//**************************************************************************************************************
void InitUART(void)
{
   /* //Enable the GPIO Port A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //Enable the peripheral UART Module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Make the UART pins be peripheral controlled.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Sets the configuration of a UART.
    UARTConfigSetExpClk(
            UART0_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTEnable(UART0_BASE);*/

    //Enable the GPIO Port B
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

        //Enable the peripheral UART Module 1
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
        GPIOPinConfigure(GPIO_PB0_U1RX);
        GPIOPinConfigure(GPIO_PB1_U1TX);
        // Make the UART pins be peripheral controlled.
        GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1);

        // Sets the configuration of a UART.
        UARTConfigSetExpClk(
                UART1_BASE, SysCtlClockGet(), 115200,
                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

//**************************************************************************************************************
// LECTURA DEL UART
//**************************************************************************************************************
void UART0ReadIntHandler(void)
{
    UARTIntClear(UART0_BASE, UART_INT_RX);

    charIn = UARTCharGet(UART0_BASE);

    //UARTCharPut(UART0_BASE, charIn);
}

//**************************************************************************************************************
// FUNCION DE LA INTERRUPCION DEL ADC0 - CH2
//**************************************************************************************************************
void ValorPotenciometro(void)
{
    ADCIntClear(ADC0_BASE, 3);
    ADCSequenceDataGet(ADC0_BASE, 3, &ValPot);
}

//**************************************************************************************************************
// Función para hacer delay en milisegundos
//**************************************************************************************************************
void delay(uint32_t msec)
{
    for (i = 0; i < msec; i++)
    {
        delay1ms();
    }

}
//**************************************************************************************************************
// Función para hacer delay de 1 milisegundos
//**************************************************************************************************************
void delay1ms(void)
{
    SysTickDisable();
    SysTickPeriodSet(40000);
    SysTickEnable();

    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0); //Pg. 138

}

//**************************************************************************************************************
// Handler de la interrupción del TIMER 0 - Recordar modificar el archivo tm4c123ght6pm_startup_css.c
//**************************************************************************************************************
void Timer0IntHandler(void)
{
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    UARTCharPut(UART1_BASE, parking_state+48);    //Mandar el byte con el estado de los 8 parqueos con caracteres legibles
    //UARTCharPut(UART0_BASE, 65);    //Mandar el byte con el estado de los 8 parqueos
}
//**************************************************************************************************************
// CONFIG TMR0A
//**************************************************************************************************************
void Timer0Config(void){

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ui32Period = ( SysCtlClockGet() ) / 2; //Para 1seg
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);


    IntEnable(INT_TIMER0A);   // Configuro las interrupciones
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Configuro las interrupciones
    IntMasterEnable();   // Configuro las interrupciones

    TimerEnable(TIMER0_BASE, TIMER_A); // timers.
}
