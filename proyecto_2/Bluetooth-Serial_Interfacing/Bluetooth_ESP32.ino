/*
  Electrónica Digital 2
  Proyecto 2: Videojuego
  Autores: Carlos Valdés
           Estuardo Castillo
  Nombre del programa: Bluetooth_ESP32
  descripción: Programa para controles 1 y 2 del ESP32. Parametros que cambian entre controles:
  Nombre del dispositivo Bluetooth
  Boton de disparo
  Input para jostick
  Hardware: Joystick Analogico, botones. 
*/

//Utiliza libreria serial 
#include <BluetoothSerial.h>

BluetoothSerial SerialBT; // Crea objeto

int lastButtonState = HIGH; // Estados para boton con Pullup 
int buttonState = HIGH;

const int debounceDelay = 50; // Ajuste de valor para delay
unsigned long lastDebounceTime = 0;
const int button1Pin = 4;

const int joystickXPin = 15; //Pin analogo del ESP32
char state; 
void setup() {
  Serial.begin(115200);
  SerialBT.begin("Control_1"); // Inicializar Bluetooth con nombre del dispositivo 
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(joystickXPin, INPUT);
  //pinMode(2, OUTPUT);
}

void loop() {
  
  //Boton de disparo 
  int reading = digitalRead(button1Pin);
  if (reading != lastButtonState) {
    // Reset
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      
      // Si efectivamente fue presionado, ejecutar:
      if (buttonState == LOW) {
          Serial.print('b');
          SerialBT.print('b');
        }
      }
    }
  // Actualizar siempre
  lastButtonState = reading;

  //Joysticks

  int joystickXValue = analogRead(joystickXPin); // Leer datos de pin analogo
  mapeo
  int mappedXValue = map(joystickXValue, 0, 4095, -100, 100);
  
  if(mappedXValue > 50){
    //Serial.println("izquierda");
    state = 'i';
    digitalWrite(2, HIGH);
  }else if (mappedXValue < -80){
    //Serial.println("derecha");
    state = 'd';
    digitalWrite(2, HIGH);
  }else {
    //Serial.println("centro");
    state = 'c';
    digitalWrite(2, LOW);
  }
  // Envio de datos a traves de Bluetooth para Joysticks
  if(state != 'c'){
    SerialBT.print(state); //solo se envia si no está en el centro 
    Serial.print(state);
  }
  delay(50);
}
