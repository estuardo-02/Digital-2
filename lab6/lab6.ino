/*
 * Código para lab6 y postlab6
 código basado en librería desarrollada por:
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 2 Feb 2014
 by Scott Fitzgerald
 
 modificado para el lab  6 Digital 2 por:
 Estuardo Castillo 
   Conexiones en la tivaC con spi.setmodule(0)
 ** MOSI - PA5
 ** MISO - PA4
 ** CLK - PA2
 ** CS - PA3
 */
//librerias
#include <SPI.h>
#include <SD.h>
//variables globales 
char ichar;
char input[50];
File root;
//const int CS = PA3;


//inicialización
void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  SPI.setModule(0);


  Serial.println("Inicializando tarjeta SD...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(12, OUTPUT);

  if (!SD.begin(12)) {
    Serial.println("initializacion fallida!");
    return;
  }
  Serial.println("initializacion completa.");
  int max_size = 5;
  char files[max_size];
  root = SD.open("/");
  Serial.println("Archivos disponibles: ");
  Serial.println("-------------------------------------------");
  for (uint8_t i=0; i<max_size; i++){
    File entry =  root.openNextFile();
    if(!entry.isDirectory()){
      Serial.print(i);
      Serial.print(". ");
      Serial.println(entry.name());
    }
  }
  Serial.println("5. crear archivo");
}

//repetir indefinidamente
void loop()
{
    File myFile; 
    if (Serial.available() > 0) {
    Serial.flush();
    int selectedOption = Serial.parseInt(); // Leer input
    //decidir que opcion se ha seleccionado
    switch (selectedOption) {
      case 1:
        Serial.println("Opcion 1");
        myFile = SD.open("coin.txt");
        Readfile(myFile);
        // Implement code for Option 1 here
        break;
        
      case 2:
        Serial.println("Opcion 2");
        myFile = SD.open("mushroom.txt");
        Readfile(myFile);
        break;
        
      case 3:
        Serial.println("Opcion 3");
        myFile = SD.open("question_mark.txt");
        Readfile(myFile);
        break;
      case 4: 
          SD.remove("foto_4.txt");
          myFile = SD.open("foto_4.txt", FILE_WRITE); //Opening the file 
          //utiliza el nombre del archivo como parametro
          if(myFile) {
            Serial.println("\nEscribe caracteres linea por linea. Presiona e para salir");
      
          int i = 0;
          //e is the scape character. While blocks reads the entered characters.
          while (ichar != 'e') {
            if(Serial.available() > 0){
                ichar = Serial.read();
                Serial.println(ichar);
                delay(20);
      
                  if (ichar != 'e') {
                    input[i] = ichar;
                    i++;
                  }
      
                if (i >= 50)
                    break;
            }
          }
            myFile.println(input);
       
            //Closing the file
            myFile.close();
          }
          else {
            Serial.println("Imagen creada ");
          }
          break;
          //selector = 0;   
      case 5: 
            myFile = SD.open("foto_4.txt"); //abrir archivo
          if(myFile) {
            Serial.println("\n \t \t Imagen creada: ");
            while (myFile.available()) {
              //escribe el contenido del archivo. 
              Serial.write(myFile.read()); 
            }
            //Closing the file
            myFile.close();
          }
          else {
            Serial.println("error abriendo");
          }
      default:
        //Serial.println("Opcion invalida");
        break;
    }
    Serial.println("terminado");
  }
}

void printDirectory(File dir, int numTabs) {
   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
void Readfile(File myFile){
  if (myFile) {
    //Serial.println("coin.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error abriendo");
  }
}
