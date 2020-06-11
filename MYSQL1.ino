#include <WiFiClient.h> 
#include "DHT.h"
#define DHTTYPE DHT11
#include <ESP8266WiFi.h>

//-------------------VARIABLES GLOBALES--------------------------
int contconexion = 0;
int salidaalta=5;
int salidabaja=4;
float baja=30.0;
float alta=30.0;
float a=0.0;
float chipip=0.0;

const int DHTPin = 2;
// Inicializar Sensor DHT 
DHT dht(DHTPin, DHTTYPE);

const char *ssid = "Proyecto_Inteligencia";
const char *password = "Tamr0906";

unsigned long previousMillis = 0;

char host[48];
String strhost = "192.168.3.10";
String strurl = "/proyecto/enviardatos.php";



//-------Función para Enviar Datos a la Base de Datos SQL--------

String enviardatos(String datos) {
  String linea = "error";
  WiFiClient client;
  strhost.toCharArray(host, 49);
  if (!client.connect(host, 80)) {
    Serial.println("Fallo de conexion");
    return linea;
  }

  client.print(String("POST ") + strurl + " HTTP/1.1" + "\r\n" + 
               "Host: " + strhost + "\r\n" +
               "Accept: */*" + "*\r\n" +
               "Content-Length: " + datos.length() + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded" + "\r\n" +
               "\r\n" + datos);           
  delay(10);             
  
  Serial.print("Enviando datos a SQL...");
  
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Cliente fuera de tiempo!");
      client.stop();
      return linea;
    }
  }
  // Lee todas las lineas que recibe del servidro y las imprime por la terminal serial
  while(client.available()){
    linea = client.readStringUntil('\r');
  }  
  Serial.println(linea);
  return linea;
}

//-------------------------------------------------------------------------

void setup() {

  // Inicia Serial
  Serial.begin(115200);
  dht.begin();
  pinMode(salidaalta,OUTPUT);
  pinMode(salidabaja,OUTPUT);
  Serial.println("");

 

  // Conexión WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion <50) {
      //para usar con ip fija
      IPAddress ip(192,168,3,20); 
      IPAddress gateway(192,168,3,1); 
      IPAddress subnet(255,255,255,0); 
      WiFi.config(ip, gateway, subnet); 
      
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
  }
  else { 
      Serial.println("");
      Serial.println("Error de conexion");
  }
}

//--------------------------LOOP--------------------------------
void loop() {

            float h = dht.readHumidity();
            //Temperatura en Celsius
            
            float t = dht.readTemperature();
       
            a=t;
            // Verifique si alguna lectura falló (intentar de nuevo).
            if (isnan(h) || isnan(t)) {
              Serial.println("Fallo al leer el sensor DHT11");
              return;     
            }
            if (a>alta){
          digitalWrite(salidaalta,HIGH);
          digitalWrite(salidabaja,LOW);
           }
          if (a<baja){
           digitalWrite(salidabaja,HIGH);
           digitalWrite(salidaalta,LOW);
           }

  unsigned long currentMillis = millis();



  if (currentMillis - previousMillis >= 30000) { //envia la temperatura cada 10 segundos
    previousMillis = currentMillis;
    int analog = analogRead(17);
    float temp = t;
    float chipip = h;
   
    enviardatos("&hum=" + String(h, 2) + "&tem=" + String(t, 2));
  }
}
