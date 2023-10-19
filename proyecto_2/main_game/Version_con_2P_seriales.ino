//***************************************************************************************************************************************
/*
  Electrónica Digital 2
  Proyecto 2: Videojuego
  Autores: Carlos Valdés
           Estuardo Castillo
  Nombre del programa: Space Invaders 
  Descripción:  Programa del Videojuego para la TIVA C
  Hardware: Una Tiva, Una pantalla ILI9341, 2 ESP32, Un Arduino, 2 joysticks, 2 conversores de nivel logico, resistencias, 1 transistor y 1 buzzer pasivo
*/
//***************************************************************************************************************************************
#include <Energia.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};

//player1 variables//////////////////////////////
#define boton_j1 PUSH2
uint8_t antirebote1 = 0;
int flag = 0;
int tempo = 0;
int change_pos;

struct CHARAC {   //estrutura para los personajes del juego
  int px, py;
  int width_b, height_b;
  int width_p, height_p;
  int ndisparos;
  int maxdisparos;
  int tipo;
  int old_x;
  int old_y;
  int vida;
  unsigned long MomentoMuerte;   //variable para saber en que momento muere el enemigo
  unsigned long MomentoDisparo;  //variable para saber en que momento dispara el enemigo
};

int dir = -1; //variables para el cambio de direccion de los enemigos
int q = 0;    //variables utilizada para la deteccion de coliciones

//Disparos/////////////////////////////

struct Balas {  //estructura para las municiones de los personajes
  int x, y;
  int dx, dy;
};


//player 2 variables////////////////////////////
uint8_t antirebote2 = 0;
int flag2 = 0;
int tempo2 = 0;
int change_pos2;

//enemies variables//////////////////////////////
int flag_Enemy = 0;
int tempo_Enemy = 0;
int ran = random(45);   //variable para generar un enemigo aleatorio

//Menu variables////////////////////////////////////
bool salida = false;
int ufo_x = 60;
bool win = false;

//Variables para los puntajes///////////////////////////
char buffer[20];
char buffer2[20];
int puntaje = 0;
int puntaje2 = 0;
int inByte;      //variables para la comunicacion serial
int inByte2;
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void Timer0IntHandler();
void Timer1IntHandler();
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

void inicializar(CHARAC* CHARAC, int pos_x, int pos_y, int _ancho_b, int _alto_b, int _ancho_p, int _alto_p, int n_disparos, int max_disparos, int _tipo, int _vida);
bool colicion(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
bool detector_de_coliciones_Enemy (struct CHARAC& nave, struct CHARAC& enemigo, struct Balas B[]);
bool detector_de_coliciones_Nave (struct CHARAC& nave, struct CHARAC& enemigo, struct Balas B[]);
bool AllEnemiesAreDead(const CHARAC enemigo[]);
bool limites(struct CHARAC enemigo[], int& dir);
void mover_enemigos(struct CHARAC enemigo[], int& dir);

void coliciones_Enemigo_con_Nave (struct CHARAC nave[], struct CHARAC enemigo[]);
void cleanup();
extern uint8_t fondo[];

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Crear un objeto para nave y otro para enemigo
CHARAC nave[2];
CHARAC enemigo[45];

Balas* disparos;  // Declarar puntero
Balas* disparos_Enemy;  // Declarar puntero

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  //Inicializar los valores de los objetos (nave y enemigos)

  inicializar(&nave[0], 48, 208, 2, 2, 32, 32, 0, 500, 0, 3); //inicializar las naves
  inicializar(&nave[1], 208, 208, 2, 2, 32, 32, 0, 500, 1, 3);

  //inicializar enemigos
  int indice = -1;
  for (int i = 0; i < 1; i++) {
    for (int j = 0; j < 9; j++) {
      indice++;
      inicializar(&enemigo[indice], 257 - (j * 26), 0 + (i * 16), 3, 7, 10, 8, 0, 1, 1, 1);
    }
  }

  int indice1 = 8;
  for (int i1 = 1; i1 < 3; i1++) {
    for (int j1 = 0; j1 < 9; j1++) {
      indice1++;
      inicializar(&enemigo[indice1], 256 - (j1 * 26), 0 + (i1 * 16), 3, 7, 13, 8, 0, 1, 2, 1);
    }
  }

  int indice2 = 26;
  for (int i2 = 3; i2 < 5; i2++) {
    for (int j2 = 0; j2 < 9; j2++) {
      indice2++;
      inicializar(&enemigo[indice2], 256 - (j2 * 26), 0 + (i2 * 16), 3, 7, 14, 8, 0, 1, 3, 1);
    }
  }

  // Initialize the arrays after nave and enemigo are initialized
  disparos = new Balas[nave[0].maxdisparos];
  disparos_Enemy = new Balas[enemigo[0].maxdisparos];

  /////////////////////////////////////////////////////////////////////////////////////////
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);  //serial con la computadora
  Serial5.begin(115200); //serial para el Arduino

  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  //Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);
  //Menu/////////////////////////////////////////////////////////////////////////////////////////////////////
  while (!salida) {  //Menu

    if (Serial.available() > 0) {
      inByte = Serial.read();
    }

    if (inByte == 'a' || inByte == 'b') {    //inicio del juego si se detecta que algun jugador presiona el boton
      Serial5.println(115);
      inByte = 0;
      salida = true;
    }

    LCD_Bitmap(151, 40, 16, 7, ufo);

    String text1 = "Space";
    LCD_Print(text1, 120, 50, 2, 0xffff, 0x0000);

    String text5 = "Invaders";
    LCD_Print(text5, 95, 75, 2, 0x2fa1, 0x0000);

    String text2 = "Push any player button";
    LCD_Print(text2, 75, 140, 1, 0xc882, 0x0000);

    delay(100);                                   //Delays para el cambio de color de las letras
    LCD_Print(text2, 75, 140, 1, 0xd6e1, 0x0000);
    //LCD_Print(text3, 155, 160, 1, 0xd6e1, 0x0000);
    //LCD_Print(text4, 81, 180, 1, 0xd6e1, 0x0000);
    delay(100);
    //LCD_Bitmap(151, 40, 16, 7, ufo);
  }
  //Desplegar las vidas y la distribucion de puntos
  FillRect(0, 0, 319, 206, 0x0000);
  String text6 = "You have 3 lives";
  LCD_Print(text6, 30, 80, 2, 0x2fa1, 0x0000);
  LCD_Bitmap(110, 130, 96, 45, puntos);
  delay(3000);
  FillRect(0, 0, 319, 206, 0x0000);

  //**********************************************************************************************************************************************

  //config tmr0///////////////////////////////////////////////////////////////////////////////////////
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

  float period_0 = 0.015; // 15ms periodo tmr0

  MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, MAP_SysCtlClockGet()*period_0); //(80MHZ / MAP_SysCtlClockGet)

  TimerIntRegister(TIMER0_BASE, TIMER_A, &Timer0IntHandler);

  MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  MAP_TimerEnable(TIMER0_BASE, TIMER_A);

  //config tmr1/////////////////////////////////////////////////////////////////////////////////////
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  MAP_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

  float period_1 = 0.11; // 1100ms periodo tmr1

  MAP_TimerLoadSet(TIMER1_BASE, TIMER_A, MAP_SysCtlClockGet()*period_1); //5 veces por seg

  TimerIntRegister(TIMER1_BASE, TIMER_A, &Timer1IntHandler);

  MAP_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

  MAP_TimerEnable(TIMER1_BASE, TIMER_A);

}

//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {

  /////////Nave////////////////////////////////////////////////////////////////////////////////////////////////

  for (int i = 0; i < 2; i++) {
    if (nave[i].vida > 0) {
      //NAVE 1
      if (nave[i].tipo == 0) {
        if (nave[i].px <= 0) {         //comprobar si la posicion en x de la nave se sale del limite izquierdo
          nave[i].px = 0;
          FillRect(nave[i].old_x + 30, nave[i].old_y, 7, nave[i].height_p, 0x0000);
          LCD_Bitmap(nave[i].px, nave[i].py, nave[i].width_p, nave[i].height_p, ship);
        }
        else if (nave[i].px >= 128) {  //comprobar si la posicion en x de la nave se sale del limite derecho
          nave[i].px = 128;
          FillRect(nave[i].old_x - 3, nave[i].old_y, 5, nave[i].height_p, 0x0000);
          LCD_Bitmap(nave[i].px, nave[i].py, nave[i].width_p, nave[i].height_p, ship);
        }
        else {                      //en el rango de la pantalla moverla y borrar lo que se deja atras con un factor de 2
          FillRect(nave[i].old_x - 4 /*(nave.old_x  > nave.px ? + 30 : -1)*/, nave[i].old_y, 7, nave[i].height_p, 0x0000);
          LCD_Bitmap(nave[i].px, nave[i].py, nave[i].width_p, nave[i].height_p, ship);
          FillRect(nave[i].old_x + 30 /*(nave.old_x > nave.px ? + 29 : -1)*/, nave[i].old_y, 8, nave[i].height_p, 0x0000);
        }
      }
      //NAVE 2
      if (nave[i].tipo == 1) {
        if (nave[i].px <= 165) {         //comprobar si la posicion en x de la nave se sale del limite izquierdo
          nave[i].px = 165;
          FillRect(nave[i].old_x + 30, nave[i].old_y, 7, nave[i].height_p, 0x0000);
          LCD_Bitmap(nave[i].px, nave[i].py, nave[i].width_p, nave[i].height_p, ship);
        }
        else if (nave[i].px >= 288) {  //comprobar si la posicion en x de la nave se sale del limite derecho
          nave[i].px = 288;
          FillRect(nave[i].old_x - 3, nave[i].old_y, 5, nave[i].height_p, 0x0000);
          LCD_Bitmap(nave[i].px, nave[i].py, nave[i].width_p, nave[i].height_p, ship);
        }
        else {                      //en el rango de la pantalla moverla y borrar lo que se deja atras con un factor de 2
          FillRect(nave[i].old_x - 3 /*(nave.old_x  > nave.px ? + 30 : -1)*/, nave[i].old_y, 5, nave[i].height_p, 0x0000);
          LCD_Bitmap(nave[i].px, nave[i].py, nave[i].width_p, nave[i].height_p, ship);
          FillRect(nave[i].old_x + 30 /*(nave.old_x > nave.px ? + 29 : -1)*/, nave[i].old_y, 7, nave[i].height_p, 0x0000);
        }
      }

    }
  }
  ////////Disparo nave/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  for (int i = 0; i < 2; i++) {
    if (nave[i].vida > 0) {
      if (nave[i].ndisparos > 0 && nave[i].ndisparos < nave[i].maxdisparos) {          //verificar si hay disparo
        for (int cont = 1; cont <= nave[i].ndisparos; cont++) {   //por cada disparo dibujar una bala con su velocidad correspondiente
          disparos[cont].x += disparos[cont].dx;
          disparos[cont].y += disparos[cont].dy;
          FillRect(disparos[cont].x - 1, disparos[cont].y, nave[i].width_b, nave[i].height_b, 0xffff);
          FillRect(disparos[cont].x - 1, disparos[cont].y + 2, nave[i].width_b, nave[i].height_b, 0x0000);

          if (disparos[cont].y > 240 || disparos[cont].y < 0      //si sale la bala de pantalla decrementar el numero de disparos para que desaparezcan las que ya no estan
              || disparos[cont].x > 320 || disparos[cont].x < 0) {

            disparos[cont] = disparos[nave[i].ndisparos];
            nave[i].ndisparos--;
            H_line(0, 240, 320, 0x0000); //borrar residuo de la bala de la nave hasta abajo

            if (nave[i].ndisparos < 0)
              nave[i].ndisparos = 0;
          }
        }
      }
    }
  }

  ///////Revisar coliciones del proyectil de la nave con todos los enemigos//////////////////////////////////////////////////////////////////////////////////////

  for (int in  = 0; in < 45 ; in++) {     //Revisar para todos los enemigos
    if (detector_de_coliciones_Enemy(nave[0], enemigo[in], disparos)) {    //Verifiar colicion con P1 y generar la animacion de muerte de enemigo
      FillRect(enemigo[in].px - 2, enemigo[in].py, enemigo[in].width_p + 2, enemigo[in].height_p + 2, 0x0000);
      LCD_Bitmap(enemigo[in].px, enemigo[in].py, 13, 8, explo_Enemy);
      delay(25);
      FillRect(enemigo[in].px - 1, enemigo[in].py, enemigo[in].width_p + 1, enemigo[in].height_p + 2, 0x0000);

      if (enemigo[in].tipo == 1) {
        FillRect(enemigo[in].px - 2, enemigo[in].py, enemigo[in].width_p + 5, enemigo[in].height_p + 2, 0x0000);
        LCD_Bitmap(enemigo[in].px - 2, enemigo[in].py, 13, 8, explo_Enemy);
        delay(25);
        FillRect(enemigo[in].px - 2, enemigo[in].py, enemigo[in].width_p + 3, enemigo[in].height_p + 2, 0x0000);
        puntaje += 30;
      }

      if (enemigo[in].tipo == 2) {        //Dependiendo del enemigo que muera aumentar puntaje
        puntaje += 20;
      }
      if (enemigo[in].tipo == 3) {
        puntaje += 10;
      }

    }
    if (detector_de_coliciones_Enemy(nave[1], enemigo[in], disparos)) {    //Verificar la colicion con P2 y generar la animacion de muerte del enemigo
      FillRect(enemigo[in].px - 2, enemigo[in].py, enemigo[in].width_p + 2, enemigo[in].height_p + 2, 0x0000);
      LCD_Bitmap(enemigo[in].px, enemigo[in].py, 13, 8, explo_Enemy);
      delay(25);
      FillRect(enemigo[in].px - 1, enemigo[in].py, enemigo[in].width_p + 1, enemigo[in].height_p + 2, 0x0000);

      if (enemigo[in].tipo == 1) {
        FillRect(enemigo[in].px - 2, enemigo[in].py, enemigo[in].width_p + 5, enemigo[in].height_p + 2, 0x0000);
        LCD_Bitmap(enemigo[in].px - 2, enemigo[in].py, 13, 8, explo_Enemy);
        delay(25);
        FillRect(enemigo[in].px - 2, enemigo[in].py, enemigo[in].width_p + 3, enemigo[in].height_p + 2, 0x0000);
        puntaje2 += 30;
      }

      if (enemigo[in].tipo == 2) {            //Dependiendo del enemigo que muera aumentar puntaje
        puntaje2 += 20;
      }
      if (enemigo[in].tipo == 3) {
        puntaje2 += 10;
      }

    }
  }

  ////Enemigos/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //Dibujar a los 45 enemigos (3 tipos) con 5 filas y 9 columnas. Ademas verificar si salen de la pantalla
  for (int i = 0; i < 45; i++) {
    //indice++;
    if (enemigo[i].vida > 0 ) { //pintar al enemigo solo si su vida es 1 (sumo 2 en height por si la bala se queda pintada justo debajo del enemigo)
      int anim = (enemigo[i].px / 10) % 2;

      if (enemigo[i].tipo == 1) {
        FillRect(enemigo[i].px, enemigo[i].py - 16, enemigo[i].width_p, enemigo[i].height_p, 0x0000);
        LCD_Sprite(enemigo[i].px, enemigo[i].py, enemigo[i].width_p, enemigo[i].height_p, Enemigo_1, 2, anim, 0, 0);
        V_line(enemigo[i].px + (enemigo[i].old_x > enemigo[i].px ? + enemigo[i].width_p : - 1), enemigo[i].py, enemigo[i].height_p + 2, 0x0000);
      }

      if (enemigo[i].tipo == 2) {
        //FillRect(enemigo[i].px, enemigo[i].py - 8, enemigo[i].width_p, enemigo[i].height_p, 0x0000);
        LCD_Sprite(enemigo[i].px, enemigo[i].py, enemigo[i].width_p, enemigo[i].height_p, Enemigo_2, 2, anim, 0, 0);
        V_line( enemigo[i].px + (enemigo[i].old_x > enemigo[i].px ? + enemigo[i].width_p : - 1), enemigo[i].py, enemigo[i].height_p + 2, 0x0000);
      }

      if (enemigo[i].tipo == 3) {
        //FillRect(enemigo[i].px, enemigo[i].py - 8, enemigo[i].width_p, enemigo[i].height_p, 0x0000);
        LCD_Sprite(enemigo[i].px, enemigo[i].py, enemigo[i].width_p, enemigo[i].height_p, Enemigo_3, 2, anim, 0, 0);
        V_line( enemigo[i].px + (enemigo[i].old_x > enemigo[i].px ? + enemigo[i].width_p : - 1), enemigo[i].py, enemigo[i].height_p + 2, 0x0000);
      }
    }
    else {
      FillRect(enemigo[i].px, enemigo[i].py, enemigo[i].width_p, enemigo[i].height_p, 0x0000);  //Si la vida del enemigo es 0, pintar un cuadro negro en su lugar
    }
  }


  ////Disparo enemigos////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if ((enemigo[ran].vida > 0) || (enemigo[ran].MomentoDisparo < enemigo[ran].MomentoMuerte)) {   //Dibujar la bala del enemigo cuando este vivo o si dispara antes de morir, caso contrario cambiar de enemigo para que dispare otro


    if (enemigo[ran].ndisparos > 0 && enemigo[ran].ndisparos <= enemigo[ran].maxdisparos) {
      for (int cont = 1; cont <= enemigo[ran].ndisparos; cont++) {


        disparos_Enemy[cont].x += disparos_Enemy[cont].dx;
        disparos_Enemy[cont].y += disparos_Enemy[cont].dy;

        FillRect(disparos_Enemy[cont].x - 1, disparos_Enemy[cont].y - 4, enemigo[ran].width_b, enemigo[ran].height_b + 3, 0x0000);
        LCD_Bitmap(disparos_Enemy[cont].x - 1, disparos_Enemy[cont].y, enemigo[ran].width_b, enemigo[ran].height_b, disp_Ene);


        if (disparos_Enemy[cont].y > 240 || disparos_Enemy[cont].y < 0
            || disparos_Enemy[2].x > 320 || disparos_Enemy[cont].x < 0) {

          disparos_Enemy[cont] = disparos_Enemy[enemigo[ran].ndisparos];
          FillRect(disparos_Enemy[cont].x, disparos_Enemy[cont].y, 2, 2, 0x0000);   //borrar bala al llegar al fondo

          if (enemigo[ran].ndisparos < 0) {
            enemigo[ran].ndisparos = 0;
          }
        }
        if (disparos_Enemy[cont].y > 240 && cont == 1) {
          enemigo[ran].ndisparos = 0;   //reiniciar la cuenta de disparos del enemigo para que le pueda volver a tocar si esta vivo
          cont = 0;                     //reiniciar contador de disparos
          ran = random(45);             //Al salir generar un nuevo enemigo para que dispare
        }

      }
    }

    ///////Revisar coliciones del proyectil de los enemigos con la nave///////////////////////////////////////////////////////////////////////////////////////////////////////////
    for (int t = 0; t < 2; t++) {  //Revisar para ambas naves
      if (detector_de_coliciones_Nave (nave[t], enemigo[ran], disparos_Enemy)) {
        q++;

        for (int cont = 1; cont <= 3; cont++) { //borrar disparo al colicionar con la nave
          FillRect(disparos_Enemy[cont].x - 1, disparos_Enemy[cont].y, enemigo[ran].width_b, enemigo[ran].height_b, 0x0000);
        }

        for (int u = 0; u < 250; u++) {     //temporizador pequeño para la animacion de muerte de las naves
          int anim5 = (nave[t].px / 5) % 2;
          LCD_Sprite(nave[t].px + 1, nave[t].py + 7, 30, 18, explo_nave, 2, anim5, 0, 0);
          FillRect(nave[t].old_x + 1 + (nave[t].old_x + 1 > nave[t].px + 1 ? + 28 : - 28), nave[t].py + 7, 30, 18, 0x0000);
        }

        if (q == 1) {                    //si hay colision, generar otro enemigo diferente que dispare
          enemigo[ran].ndisparos = 0;   //reiniciar la cuenta de disparos del enemigo para que le pueda volver a tocar
          q = 0;                     //reiniciar contador de disparos
          ran = random(45);
        }

      }
    }
  }

  else {    //Si el enemigo muere o si muere antes de disparar buscar otro enemigo que dispare
    enemigo[ran].ndisparos = 0; //reiniciar cuenta del enemigo que murio
    ran = random(45);           //buscar otro enemigo para que dispare
    for (int cont = 1; cont <= 3; cont++) { //Dependiendo del numero de disparo, borrar disparo al colicionar con la nave
      FillRect(disparos_Enemy[cont].x - 1, disparos_Enemy[cont].y, enemigo[ran].width_b, enemigo[ran].height_b, 0x0000);
    }
  }

  coliciones_Enemigo_con_Nave(nave, enemigo);

  if (nave[0].vida == 0 && nave[1].vida == 0) {     //Rutina para Game Over y mostrar puntajes
    bool gameOver = true;
    FillRect(0, 0, 319, 240, 0x0000);
    while (gameOver) {
      String text = "Game Over";
      LCD_Print(text, 85, 100, 2, 0xc882, 0x0000);

      String text1 = "P1:";
      LCD_Print(text1, 80, 150, 2, 0xd6e1, 0x0000);

      sprintf(buffer, "%d", puntaje);
      String points = buffer;
      LCD_Print(buffer, 140, 150, 2, 0xd6e1, 0x0000);

      String text2 = "pts";
      LCD_Print(text2, 195, 150, 2, 0xd6e1, 0x0000);

      String text3 = "P2:";
      LCD_Print(text3, 80, 200, 2, 0xd6e1, 0x0000);

      sprintf(buffer2, "%d", puntaje2);
      String points2 = buffer;
      LCD_Print(buffer2, 140, 200, 2, 0xd6e1, 0x0000);

      String text4 = "pts";
      LCD_Print(text4, 195, 200, 2, 0xd6e1, 0x0000);

    }
  }

  if (AllEnemiesAreDead(enemigo)) {   //Rutina para victoria y mostrar puntajes
    win = true;
    FillRect(0, 0, 319, 240, 0x0000);
    while (win) {
      String text = "You won!";
      LCD_Print(text, 100, 100, 2, 0xffff, 0x0000);

      String text1 = "P1:";
      LCD_Print(text1, 80, 150, 2, 0xd6e1, 0x0000);

      sprintf(buffer, "%d", puntaje);
      String points = buffer;
      LCD_Print(buffer, 140, 150, 2, 0xd6e1, 0x0000);

      String text2 = "pts";
      LCD_Print(text2, 195, 150, 2, 0xd6e1, 0x0000);

      String text3 = "P2:";
      LCD_Print(text3, 80, 200, 2, 0xd6e1, 0x0000);

      sprintf(buffer2, "%d", puntaje2);
      String points2 = buffer;
      LCD_Print(buffer2, 140, 200, 2, 0xd6e1, 0x0000);

      String text4 = "pts";
      LCD_Print(text4, 195, 200, 2, 0xd6e1, 0x0000);
    }
  }

  cleanup();      //limpiar el uso de memoria dinamica
}

//***************************************************************************************************************************************
// Función para saber si todos los enemigos están muertos y mostrar pantalla de ganar
//***************************************************************************************************************************************

bool AllEnemiesAreDead(const CHARAC enemigo[]) {
  for (int i = 0; i < 45; i++) {
    if (enemigo[i].vida > 0) {
      // Hay al menos uno vivo, devolver false
      return false;
    }
  }
  // Todos los enemigos estan muertos, devolver true
  return true;
}

//***************************************************************************************************************************************
// Función para inicializar los valores de los personajes
//***************************************************************************************************************************************
void inicializar(CHARAC * CHARAC, int pos_x, int pos_y, int _ancho_b, int _alto_b, int _ancho_p, int _alto_p, int n_disparos, int max_disparos, int _tipo, int _vida) {
  CHARAC->px = pos_x;               //funcion para desginar el valor que querramos a cualquier miembro de nuestro objeto (personaje)
  CHARAC->py = pos_y;
  CHARAC->width_b = _ancho_b;
  CHARAC->height_b = _alto_b;
  CHARAC->width_p = _ancho_p;
  CHARAC->height_p = _alto_p;
  CHARAC->ndisparos = n_disparos;
  CHARAC->maxdisparos = max_disparos;
  CHARAC->tipo = _tipo;
  CHARAC->vida = _vida;
}

//***************************************************************************************************************************************
// Función para coliciones
//***************************************************************************************************************************************
bool colicion(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {


  return ((x1 < x2 + w2) && (x2 < x1 + w1) && (y1 < y2 + h2) && (y2 < y1 + h1));
}

//***************************************************************************************************************************************
// Función para elminar bala de colisiones y reducir vida de enemigos
//***************************************************************************************************************************************
bool detector_de_coliciones_Enemy (struct CHARAC& nave, struct CHARAC& enemigo, struct Balas B[]) {

  //for (int in  = 0; in < 45 ; in++) {
  if (nave.ndisparos > 0 && nave.ndisparos < nave.maxdisparos) {          //verificar si hay disparo
    for (int contt = 1; contt <= nave.ndisparos; contt++) {   //por cada disparo dibujar una bala con su velocidad correspondiente
      if (colicion(enemigo.px, enemigo.py, enemigo.width_p, enemigo.height_p,
                   disparos[contt].x, disparos[contt].y, nave.width_b, nave.height_b) && enemigo.vida > 0) { //ver si hubo colision y si el enemigo tiene vida

        disparos[contt] = disparos[nave.ndisparos]; //eliminar bala
        nave.ndisparos--;     //Borrar el disparo que choca
        enemigo.vida--;       //Reducir la vida del enemigo si coliciona

        if (enemigo.vida <= 0) {
          enemigo.MomentoMuerte = millis();  // Obtener momento en que muere el enemigo
        }

        if (nave.ndisparos < 0) {
          nave.ndisparos = 0;
        }
        return true;

      }
    }
  }
  //}
  return false;
}
//***************************************************************************************************************************************
// Función para elminar bala de colisiones y reducir vida de nave
//***************************************************************************************************************************************
bool detector_de_coliciones_Nave (struct CHARAC& nave, struct CHARAC& enemigo, struct Balas B[]) {

  if (enemigo.ndisparos > 0 && enemigo.ndisparos <= enemigo.maxdisparos) {          //verificar si hay disparo
    for (int contt = 1; contt <= enemigo.ndisparos; contt++) {   //por cada disparo dibujar una bala con su velocidad correspondiente
      if (colicion(nave.px, nave.py, nave.width_p, nave.height_p,
                   disparos_Enemy[contt].x, disparos_Enemy[contt].y, enemigo.width_b, enemigo.height_b) && nave.vida > 0) { //ver si hubo colision y si el enemigo tiene vida

        disparos_Enemy[contt] = disparos_Enemy[enemigo.ndisparos]; //eliminar bala
        enemigo.ndisparos--;
        nave.vida--;            //Bajar la vida de la nave

        if (enemigo.ndisparos < 0) {
          enemigo.ndisparos = 0;
        }
        return true;

      }
    }
  }

  return false;
}

//***************************************************************************************************************************************
// Función para conocer si un enemigo toca a ambas naves
//***************************************************************************************************************************************
void coliciones_Enemigo_con_Nave (struct CHARAC nave[], struct CHARAC enemigo[]) {

  for (int contt = 0; contt < 45; contt++) {                    //revisar por cada enemigo si toca a la nave
    for (int i = 0; i < 2; i++) {
      if (colicion(nave[i].px, nave[i].py, nave[i].width_p, nave[i].height_p,
                   enemigo[contt].px, enemigo[contt].py, enemigo[contt].width_p, enemigo[contt].height_p) && nave[i].vida > 0) { //ver si hubo colision y si el enemigo tiene vida


        nave[i].vida = 0;   //si un enemigo toca a la nave, automaticamente la nave pasa a tener vida 0
      }
    }
  }

}

//***************************************************************************************************************************************
// Función para conocer la direccion de los enemigos
//***************************************************************************************************************************************
bool limites(struct CHARAC enemigo[], int& dir) {
  for (int i = 0; i < 45; i++ ) {     //revisar para todos los enemigos
    if ((enemigo[i].px > 306 || enemigo[i].px < 2) && (enemigo[i].vida != 0)) { //detectar el camhio de direccion para los que estan vivos
      dir = -1 * dir;   //hacer el cambio multiplicando por -1
      return true;
    }
  }
  return false;
}

//***************************************************************************************************************************************
// Función para mover a los enemigos
//***************************************************************************************************************************************
void mover_enemigos(struct CHARAC enemigo[], int& dir) {
  for (int i = 0; i < 45; i++) {//revisar para todos los enemigos
    enemigo[i].px += dir;       //Sumar segun la direccion
  }

  if (limites(enemigo, dir)) {    //Si llega al limite bajarlos a todos
    for (int j = 0; j < 45; j++) {
      enemigo[j].py += 16;
    }
  }

}

//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
/*void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
  }
*/

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + w;
  y2 = y + h;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = w * h * 2 - 1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);

      //LCD_DATA(bitmap[k]);
      k = k - 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}

//***************************************************************************************************************************************
// Funcion para limpiar
//***************************************************************************************************************************************
void cleanup() {
  // Don't forget to release memory when you're done
  delete[] disparos;
  delete[] disparos_Enemy;
}

//***************************************************************************************************************************************
// Función para mapear
//***************************************************************************************************************************************
int Map(int In, int In_min, int In_max, int Out_min, int Out_max)
{
  return ((((In - In_min) * (Out_max - Out_min)) / (In_max - In_min)) + Out_min);
}
//***************************************************************************************************************************************
// Interrupcion tmr0
//***************************************************************************************************************************************
void Timer0IntHandler() {
  MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  //Rutina para mover la nave1 con el joystick
  nave[0].old_x = nave[0].px;
  nave[0].old_y = nave[0].py;
  change_pos = 1;
  //int inByte;
  if (!q) {
    if (Serial.available() > 0) {   //Leer el serial y segun los caracteres recibidos mover o disparar para P1
      inByte = Serial.read();
    }
    if (inByte == 'r') { //mover derecha
      nave[0].px -= 2;
      inByte = 0;
    }
    if (inByte == 'l') { //mover izquierda
      nave[0].px += 2;
      inByte = 0;
    }
    if (inByte == 'a') { //boton de disparo
      antirebote1 = 1;
      inByte = 0;
      Serial5.println(97);    //mandar este caracter para el sonido
    }
    else {
      //Serial.println("Center");
      change_pos = 0;
    }
  }

  if (antirebote1 == 1 && !flag) {
    antirebote1 = 0;
    if (nave[0].ndisparos < nave[0].maxdisparos) {    //si se dispara aumentan el numero de disparos y se le asginan a las variables de la bala sus respectivos valores
      nave[0].ndisparos++;
      disparos[nave[0].ndisparos].x = nave[0].px + 16; //para que la bala salga de en medio de la nave
      disparos[nave[0].ndisparos].y = nave[0].py;
      disparos[nave[0].ndisparos].dx = 0;
      disparos[nave[0].ndisparos].dy = -2;     //para que se mueva hacia arriba debe ser negativo
      flag = 1;
    }

  }
  if (tempo++ > 5) {  //pequeño atraso para el siguiente disparo
    flag = 0;
    tempo = 0;
  }

  //Rutina para mover la nave2 con el joystick    //Leer el serial y segun los caracteres recibidos mover o disparar para P1
  nave[1].old_x = nave[1].px;
  nave[1].old_y = nave[1].py;
  change_pos2 = 1;

  if (!q) {
    if (Serial.available() > 0) {
      inByte = Serial.read();
    }
    if (inByte == 'd') { //mover derecha
      nave[1].px -= 2;
      inByte = 0;
    }
    if (inByte == 'i') { //mover izquierda
      nave[1].px += 2;
      inByte = 0;
    }
    if (inByte == 'b') { //boton de disparo
      antirebote2 = 1;
      inByte = 0;
      Serial5.println(98);    //mandar este caracter para el sonido
    }
    else {
      //Serial.println("Center");
      change_pos2 = 0;
    }
  }
  //Rutina para disparar con la nave con el boton

  if (antirebote2 == 1 && !flag2) {
    antirebote2 = 0;
    if (nave[1].ndisparos < nave[1].maxdisparos) {  //si se dispara aumentan el numero de disparos y se le asginan a las variables de la bala sus respectivos valores
      nave[1].ndisparos++;
      disparos[nave[1].ndisparos].x = nave[1].px + 16; //para que la bala salga de en medio de la nave
      disparos[nave[1].ndisparos].y = nave[1].py;
      disparos[nave[1].ndisparos].dx = 0;
      disparos[nave[1].ndisparos].dy = -2;     //para que se mueva hacia arriba debe ser negativo
      flag2 = 1;
    }

  }

  if (tempo2++ > 5) {  //pequeño atraso para el siguiente disparo
    flag2 = 0;
    tempo2 = 0;
  }

  cleanup();

}
//***************************************************************************************************************************************
// Interrupcion tmr1
//***************************************************************************************************************************************
void Timer1IntHandler() {
  MAP_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

  for (int i = 0; i < 45; i++) {
    enemigo[i].old_x = enemigo[i].px;   //Asignar la posicion anerior para todos los enemigos
    enemigo[i].old_y = enemigo[i].py;
  }
  
  if (!q) {                   //en el momento de impacto no cambiar el movimiento de los enemigos para evitar pintar de mas
    mover_enemigos(enemigo, dir);
  }

  //Rutina para que dispare enemigo

  if (enemigo[ran].ndisparos < enemigo[ran].maxdisparos && !flag_Enemy) {
    enemigo[ran].ndisparos++;                 //Si dispara el enemigo aumentar el numero de disparos y agregar las propiedades correspondientes a la bala
    enemigo[ran].MomentoDisparo = millis();  //obtener momento en el que dispara el enemigo para despues comparar si muere antes o dispara antes
    disparos_Enemy[enemigo[ran].ndisparos].x = enemigo[ran].px + 6;
    disparos_Enemy[enemigo[ran].ndisparos].y = enemigo[ran].py;
    disparos_Enemy[enemigo[ran].ndisparos].dx = 0;
    disparos_Enemy[enemigo[ran].ndisparos].dy = 4;     //hacia abajo
    flag_Enemy = 1;
  }

  if (tempo_Enemy++ > 1) {   //esperar un tiempo para siguente disparo
    flag_Enemy = 0;
    tempo_Enemy = 0;
  }
  cleanup();
}
//***************************************************************************************************************************************
// Funcion para obtener valor de Timer1
//***************************************************************************************************************************************
unsigned long Timer1_Value() {
  // Read the current value of Timer1
  return TimerValueGet(TIMER1_BASE, TIMER_A);
}
