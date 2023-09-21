/*
  File: lab5.ino
  Author: Estuardo Castillo
  fecha: 19/09/23
  Hardware necesario: Leds y resistencias en puerto para contador de decada
*/

// Declaracion de variables para ejecucion 
const int redLED = RED_LED;       // Red LED
const int blueLED = BLUE_LED; // Yellow LED
const int greenLED = GREEN_LED;   // Green LED
const int button1Pin = PUSH1;     // Push button 1
const int button2Pin = PUSH2;      // Push button 2
const int counterPins_player1[] = {2, 3, 4, 5, 6, 7, 8, 9}; //arreglo con los puertos necesarios para contador de LEds de jugador 1
const int counterPins_player2[] = {23, 24, 25, 26, 27, 28, 29, 10}; //arreglo con los puertos necesarios para contador de LEds de jugador 2
int game_state;
int player_1, player_2;
//Funcion DE setup 
void setup() {
  // Inicializar leds internos como OUTPUTS
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  //leds externos tambien como output, jugador 1
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  //leds externos tambien como output, jugador 2
  pinMode(23, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(10, OUTPUT);

  // Inicializa botones como pull-up resistor
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  //valores iniciales de variables
  player_1 = 0;
  player_2 = 0;
  
}
//Repetir indefinidamente: 
void loop() {
  // Wait for the button press
  while (digitalRead(button1Pin) == HIGH) {
    delay(10); // Check the button state every 10ms
  }

  // Button is pressed, start the LED sequence
  digitalWrite(redLED, HIGH);    // Red light for 2 seconds
  delay(1000);
  digitalWrite(redLED, LOW);

  // Blinking red for 1 seconds
  for (int i = 0; i < 8; i++) {
    digitalWrite(redLED, HIGH);
    delay(125);
    digitalWrite(redLED, LOW);
    delay(125);
  }

  // Yellow for 2 seconds
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, HIGH);
  delay(1000);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);

  // Green light until the button is pressed again
  game_state = 0;
  while (game_state == 0) {
    game_state = wait_for_player();
    digitalWrite(greenLED, HIGH);
  }
  digitalWrite(greenLED, LOW);
  if(game_state == 1){
  digitalWrite(blueLED, HIGH);
  digitalWrite(redLED, HIGH);
  player_1++;
  displayleds(player_1, game_state);
  }
  else if(game_state == 2){
  digitalWrite(blueLED, HIGH);
  digitalWrite(greenLED, HIGH);
  player_2++;
  displayleds(player_2, game_state);
  }
  delay(1500);
  restart_leds();
}

//funciones 
void restart_leds(){
  digitalWrite(blueLED, LOW); //resetear todos los colores. 
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  }
int wait_for_player(){
  int buttonState1 = digitalRead(button1Pin);
  int buttonState2 = digitalRead(button2Pin);

  if (buttonState1 == LOW && buttonState2 == LOW) {
    // Both buttons pressed at the same time
    return 0;
  } else if (buttonState1 == LOW) {
    // Button 1 pressed first
    return 1;
  } else if (buttonState2 == LOW) {
    // Button 2 pressed first
    return 2;
  } else {
    // No button pressed
    return 0;
  }
}
void displayleds(int value, int player) {
  byte patterns[] = {
    B00000000, // 0
    B00000001, // 1
    B00000011, // 2
    B00000111, // 3
    B00001111, // 4
    B00011111, // 5
    B00111111, // 6
    B01111111, // 7
    B11111111, // 8
  };
  for (int i = 0; i < 8; i++) {
    if(player == 1){
      digitalWrite(counterPins_player1[i], bitRead(patterns[value], i));
    }
    else{
      digitalWrite(counterPins_player2[i], bitRead(patterns[value], i));
    }
  }
}
