// Adafruit IO Publish Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

#define RXD2 16
#define TXD2 17

char message;
int numericValue;
char receivedChar;

int humedad;
int temperatura;
int agua;
/************************ Example Starts Here *******************************/

// this int will hold the current count for our sketch
/*float humedad;
float temperatura;
int tanque;
int servo;
int dc;
bool estadoServo;
bool estadoDC;*/

// set up the 'counter' feed
AdafruitIO_Feed *humedadCanal = io.feed("humedad");
AdafruitIO_Feed *temperaturaCanal = io.feed("temperatura");
AdafruitIO_Feed *aguaCanal = io.feed("agua");

/*AdafruitIO_Feed *servoCanal = io.feed("servo");
AdafruitIO_Feed *dcCanal = io.feed("dc");*/

void setup() {

  //pinMode(2, OUTPUT);
  // start the serial connection
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);  //UART2 por donde se comunica con el PIC

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // set up a message handler for the count feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.

  //servoCanal->onMessage(handleServo);
  //dcCanal->onMessage(handleDC);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  //servoCanal->get();
  //dcCanal->get();

}

void loop() {

  
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  //leer variables
  if (Serial2.available()) {
    receivedChar = Serial2.read(); // Leer el carácter recibido
    numericValue = receivedChar - '0'; // Convertir el carácter ASCII a un número
    
    Serial.print("Caracter recibido: ");
    Serial.println(receivedChar);
    
    Serial.print("Valor numérico: ");
    Serial.println(numericValue);
   }
  //hacer el mapeo si es necesario

  // save count to the 'counter' feed on Adafruit IO

  if (numericValue >= 50 && numericValue <= 205){
  humedad = numericValue;    
  Serial.print("sending -> ");
  Serial.println(humedad);
  humedadCanal->save(humedad);
}

  else if (numericValue >= 20 && numericValue <= 36){
  temperatura = numericValue;  
  Serial.print("sending -> ");
  Serial.println(temperatura);
  temperaturaCanal->save(temperatura);
  }

  else if (numericValue >= 0 && numericValue <= 18){
  agua = numericValue;
  Serial.print("sending -> ");
  Serial.println(agua);
  aguaCanal->save(agua);
  }

  // Adafruit IO is rate limited for publishing, so a delay is required in
  // between feed->save events. In this example, we will wait three seconds
  // (1000 milliseconds == 1 second) during each loop.
  delay(3000);

}

// this function is called whenever a 'counter' message
// is received from Adafruit IO. it was attached to
// the counter feed in the setup() function above.

/*void handleServo(AdafruitIO_Data *data) {

  Serial.print("servo <- ");
  Serial.println(data->value());

  if (*data->value() == '1'){
    estado = true;
  }else{
    estado = false;
  } 

  void handleDC(AdafruitIO_Data *data) {

  Serial.print("dc <- ");
  Serial.println(data->value());

  if (*data->value() == '1'){
    estado = true;
  }else{
    estado = false;
  }*/


