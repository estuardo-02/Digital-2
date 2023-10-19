/*
  Electrónica Digital 2
  Proyecto 2: Videojuego
  Autores: Carlos Valdés
           Estuardo Castillo
  Nombre del programa: Buzzer_load_from_SD
  descirpción: Programa complementario para TivaC principal. Este programa corre en background mientras recibe 
  instrucciones de Tiva principal, por lo que se comporta como esclavo. Tambien se usa modulo SD conectado via SPI para 
  cargar datos en instrucciones de este programa.
  Hardware: LED en pin 7, Buzzer en Pin 9, Modulo SPI para tarjeta SD con pines: 
  SD_CLK   PIN13
  SD_MOSI  PIN12
  SD_MISO  PIN11
  SD_CS PIN10
*/



//librerias
#include <SPI.h>
#include <SD.h>
File dataFile; // File object to handle the text file
String dataString; // String to store the contents of the text file
/*PINES PARA SD en SPI
  SD_CLK   PIN13
  SD_MOSI  PIN12
  SD_MISO  PIN11
  SD_CS PIN10
*/
const int buzzerPin = 9; //declaración de constante para Buzzer 
float note_amplitudes[149]; //aqui se guardan los dos archivos de texto, segun corresponda. 
//Setup 
void setup() {
 pinMode(buzzerPin, OUTPUT); //Output para Buzzer
 pinMode(7, OUTPUT);         //pin para LED, solo de debuggeo
 //se va a encontrar en varias partes del programa esto, ya que al estar ocupado el puerto serial es conveniente
 //tener un LED para saber el estado del programa 
 Serial.begin(115200);       //inicializar serial
 // Inicializacion SD
  if (!SD.begin(10)) {
    //Serial.println("SD card initialization failed!");
    return;
  }

  // Empieza son sonido de disparo por defecto. Ya que es un evento mas recurrente y la intro es un evento especial. 
  select_file(2); //1 para intro, 2 para disparo.
}

// Loop 
void loop() {

  if(Serial.available() > 0 ){
    int inByte = Serial.parseInt();
      if (inByte == 97 or inByte == 98){ //llamar a funcion de laser 
          for(int i=0;i<149;i++){
            soundFX(400.0,note_amplitudes[i]*-1.0);
          }
      }
      if (inByte == 115){ //llamar a funcion de start cuando recibe una 's'
          select_file(1);
          intro_song();
          select_file(2);
      }
    }        
}
//Funciones 
void select_file(int select){
  if(select == 1){
    dataFile = SD.open("intro.txt");
  }
  if(select == 2){
    dataFile = SD.open("laser.txt");
  }
  if (dataFile) {
    digitalWrite(7, 1);
    int i = 0;
    // Leer de txt y almacenar en string. 
    while (dataFile.available()) {
      dataString = dataFile.readStringUntil(',');
      Serial.println("from file:");
      Serial.println(dataString);
      note_amplitudes[i] = dataString.toFloat(); //conversion de estring a digitos 
      Serial.println("into list:");
      Serial.println(note_amplitudes[i]);
      i++;
      
    }
    // Cerrar el archivo
    dataFile.close();
  } else {
    digitalWrite(7, 0); //Apagar Led si hubo errores. 
  }
}
void intro_song(){
  //melodia de inicio
  //valores que no estan en txt:
  int NOTE_G3 = 196; //frecuencias
  int NOTE_A4 = 440;
  for (int thisNote = 2; thisNote < 80; thisNote+= 3) {
    //se saltan unas notas a proposito para que suene mejor. Considerar que la composicion original tiene armonicos
    //diferentes y en este caso solo se escogen las notas mas altas para que reproduzca el buzzer. 
    int noteDuration = 1000/10;                     //las notas leidas de archivo txt tienen 100ms de duracion
    tone(buzzerPin, note_amplitudes[thisNote],noteDuration);

    int pauseBetweenNotes = noteDuration +15;      //delay entre pulsos
    delay(pauseBetweenNotes);
    
    noTone(buzzerPin);                // detener Buzzer 
  }
  int noteDuration = 1000/2;                       //media nota
    tone(buzzerPin, NOTE_G3 ,noteDuration);
    int pauseBetweenNotes = noteDuration +80;      //delay entre pulsos
    delay(pauseBetweenNotes);
    noTone(buzzerPin); 
  for (int thisNote = 0; thisNote < 3; thisNote++){
    int noteDuration = 1000/4;                     //1/4 de nota
    tone(buzzerPin, NOTE_G3 ,noteDuration);
    int pauseBetweenNotes = noteDuration +50;      //delay entre pulsos
    delay(pauseBetweenNotes);
    noTone(buzzerPin); 
    }
  noteDuration = 1000/1;
    tone(buzzerPin, NOTE_A4 ,noteDuration);
    pauseBetweenNotes = noteDuration +25;      //delay between pulse
    delay(pauseBetweenNotes);
    noTone(buzzerPin); 
}
void soundFX(float amplitude, float u_delay){ 
 int uDelay=2+amplitude+amplitude*u_delay;
 //Serial.println(u_delay);
 for(int i=0;i<5;i++){
   digitalWrite(buzzerPin,HIGH);
   delayMicroseconds(uDelay);
   digitalWrite(buzzerPin,LOW);
   delayMicroseconds(uDelay);
 }
 noTone(buzzerPin);
}
