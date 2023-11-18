/*************************************************************************************************
  Elctrónica Digital 2
  Proyecto 3 Parqueo
  Estuardo Castillo 21559 & Carlos Valdés 21110
  Código para comunicación serial con ESP32 y dos TIVA c conectadas en Serial 0 y Serial 2. 
**************************************************************************************************/
//************************************************************************************************
// Librerías
//************************************************************************************************
#include <WiFi.h>
#include <WebServer.h>
#include "time.h"
#include "sntp.h"
//************************************************************************************************
// Variables globales
//************************************************************************************************
// SSID & Password
const char* ssid = "PIC16F887";  // Enter your SSID here
const char* password = "3.14159265";  //Enter your Password here
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char* time_zone = "CST6";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)
int _7segment_port[] = {22, 21, 19, 23}; 
int _7segment_numbers[9][4] = {
{0,0,0,0},
{0,0,0,1},
{0,0,1,0},
{0,0,1,1},
{0,1,0,0},
{0,1,0,1},
{0,1,1,0},
{0,1,1,1},
{1,0,0,0}
};
uint8_t LED1pin = 2;
bool LED1status = LOW;

String values[] = {"1", "2", "3", "4", "5", "6", "7", "8"
                      };
int park_state = 0x01;
int park_available = 0;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return;
  }
}
// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
  printLocalTime();
}



WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)




//************************************************************************************************
// Configuración
//************************************************************************************************
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  //time config
  // set notification call-back function
  sntp_set_time_sync_notification_cb( timeavailable );

  /**
   * NTP server address could be aquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE aquired NTP server address
   */
  sntp_servermode_dhcp(1);    // (optional)

  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagicaly.
   */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  /**
   * A more convenient approach to handle TimeZones with daylightOffset 
   * would be to specify a environmnet variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
   */
  configTzTime(time_zone, ntpServer1, ntpServer2);





  pinMode(LED1pin, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);

  // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  //Serial.print("Got IP: ");
  //Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

  server.on("/", handle_OnConnect); // Directamente desde e.g. 192.168.0.8
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  
  server.onNotFound(handle_NotFound);

  server.begin();
  delay(100);
}
//************************************************************************************************
// loop principal
//************************************************************************************************
void loop() {
  server.handleClient();
  for(int i = 0; i <=3; i++){
    digitalWrite(_7segment_port[i], _7segment_numbers[park_available][i]);
  }
  //23, 19, 21, 22
  digitalWrite(LED1pin, HIGH);
  int prev_park_state = park_state;
  if (Serial2.available()) {
    // Read the incoming byte
    park_state = Serial2.read() - 48;
    park_state = park_state<<1;
    // Print the received data to the Serial Monitor
    //Serial.println(park_state);
  }
  if (Serial.available()) {
    // Read the incoming byte
    int park_state_2 = Serial.read() - 48;
    park_state = park_state + park_state_2;
    // Print the received data to the Serial Monitor
    //Serial.println(park_state);
  }
  if(park_state != prev_park_state){
    server.send(200, "text/html", SendHTML(0));
  }
  delay(500);
  //printLocalTime();     // it will take some time to sync time :)
}
//************************************************************************************************
// Handler de Inicio página
//************************************************************************************************
void handle_OnConnect() {
  LED1status = LOW;
  Serial.println("GPIO2 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status));
}
//************************************************************************************************
// Handler de led1on
//************************************************************************************************
void handle_led1on() {
  LED1status = HIGH;
  server.send(200, "text/html", SendHTML(LED1status));
}
//************************************************************************************************
// Handler de led1off
//************************************************************************************************
void handle_led1off() {
  LED1status = LOW;
  server.send(200, "text/html", SendHTML(LED1status));
}
//************************************************************************************************
// Procesador de HTML
//************************************************************************************************
String SendHTML(uint8_t led1stat) {
String ptr = "<!DOCTYPE html>\n";
ptr +="<html lang=en>\n";
ptr +="<head>\n";
ptr +="<title>Parking a-lot</title>\n";
ptr +="<script>";
ptr +="function autoRefresh(){";
ptr +="window.location = window.location.href;";
ptr +="}";
ptr +="setInterval('autoRefresh()', 1500)" ;
ptr +="</script>\n";
ptr +="<meta charset=utf-8>\n";
ptr +="<meta name='viewport' content='width=device-width,  initial-scale=1'>\n";
ptr +="<style>*{box-sizing:border-box;}body{font-family:Arial, Helvetica, sans-serif;}header{background-color:#cee3ff;padding:30px;text-align:center;font-size:35px;color:white}nav{float:left;width:40%;height:750px;background:#8dbdfc;padding:70px}article{float:left;padding:20px;width:60%;background-color:#378eff;height:750px}section::after{content:"";display:table;clear:both}footer{background-color:#cee3ff;\n";
ptr +="padding:10px;text-align:center;color:BLACK;}@media(max-width:600px){nav, article {width:100%; height:auto;}}</style>\n";
ptr +="</head>\n";
ptr +="<body>\n";
ptr +="<header>\n";
ptr +="<img alt='Imagen del logo' src='https://lh5.googleusercontent.com/JPFqtoTKaPpSFeKwS1Ce4gv2s_O2C8UkIHFZc_Nz1Za6DRy-G5YxyMIdrhxxoXgKlls_ZjwL24Esd1bPwHX6DMMRMnRMhlSpRdSgd_yW0wBHH-DEPltOeLsKXgikgEpsNw=w1280'>\n";
ptr +="</header>\n";
ptr +="<section>\n";
ptr +="<nav>\n";
ptr +="<ul>\n";
ptr +="<style>table, th, td{border:2px solid #cee3ff;border-collapse:collapse}tr:nth-child(1){background-color:#cee3ff}</style>\n";
ptr +="<table style=width:100%>\n";
ptr +="<tr>\n";
ptr +="<th style=width:25%>\n";
ptr +="<p style=color:#666;font-family:verdana>Espacio de Parqueo</p>\n";
ptr +="</th>\n";
ptr +="<th style=width:10%>\n";
ptr +="<p style=color:#666;font-family:verdana>estado actual</p>\n";
ptr +="</th>\n";
ptr +="</tr>\n";
park_available = 0;
//Inicio de columna de la tabla
for(int i = 0; i <=7; i++){
  ptr +="<tr>\n";
  ptr +="<td>\n";
  ptr += write_park_state(i, park_state);
  ptr +="</td>\n";
  ptr +="</tr>\n";
};
//Fin de las columnas
ptr +="</table>\n";
ptr +="</ul>\n";
ptr +="</nav>\n";
ptr +="<article>\n";
ptr +="<h1 style=font-size:160%;color:#252525><i>Actualizaciones en tiempo real</i></h1>\n";
ptr +="<p>Manten el control del los espacios de parqueo por medio del sitio web y el servidor de ESP32</p>\n";
ptr +="<p>más texto</p>\n";
ptr +="<h1 style=font-size:160%;color:#252525;text-align:right><i>Manten la calma</i></h1>\n";
ptr +="<style>.right{text-align:right}</style>\n";
ptr +="<div class=right>\n";
ptr +="<img alt='Mujer manejando' src='https://lh5.googleusercontent.com/b4zkNd0hNGB-uwvfHMiMWMrplYKLzHShszG8s7xMZrqgjugFcgdEJ9msCQNJu3iD-3X29XFufQ1v1gOtNmMGLRcJLAFrervRK_myPyCljFFDDB1N5e6A1nN6mKhqItdZaA=w1280'>\n";
ptr +="</div>\n";
ptr +="<p style=text-align:right>Deja que nosotros nos encarguemos de todo.</p>\n";
ptr +="</article>\n";
ptr +="</section>\n";
ptr +="<footer>\n";
ptr +="<p>Última actualización hace 1 segundo: </p>\n";
ptr += "</p>\n";
ptr +="</footer>\n";
ptr +="</body>\n";
ptr +="</html>\n";

  return ptr;
}
//************************************************************************************************
// Handler de not found
//************************************************************************************************
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
String write_park_state(int park_n, int park_state){
  String ptr;
  String availablent = "&#128308";
  String available = "&#128309";
  int current_state = 0;
  ptr += "<p style=color:#666;font-family:verdana>Parqueo ";
  ptr += values[park_n];
  ptr += "</p>\n";
  ptr += "</td>\n";
  ptr += "<td>\n";
  ptr += "<p style=text-align:center>\n";
  
  if(park_state & 1<<park_n){
    current_state = 1;
    park_available++;
    ptr += available;
  }else{
    ptr += availablent;
  }
  ptr += ";</p>\n";
  //Verificacion del estado en serial
  //returnar nueva variable para seguir concatenando
  return ptr;
}
