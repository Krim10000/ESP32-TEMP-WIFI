//  ESP32 conectado al sensor KY-001


//  KY-001 

//  [s] --> PIN 4

//  [+] --> VIN

//  [-] --> GROUND



// ESCRIBE INFORMACION A UNA BASE DE DATOS DEL PROYECTO  https://github.com/Krim10000/Django-TEMP

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into pin 4 on the Arduino
#define ONE_WIRE_BUS 4

const char* ssid ="TU_RED";                      //CAMBIAR
const char* password ="TU_CONTRASEÑA";           //CAMBIAR

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

float temp;
int time1 = 1; // time in minutes
int time2 = time1*60*1000; // delay entre post

void Conect_internet()
{
  Serial.println("");
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(10);
  }
  Serial.println("");
  Serial.println("Conexion exitosa ");
  Serial.print("La IP es :");
  Serial.println(WiFi.localIP());
}
///conecta a internet
void setup() {
Serial.println("INICIANDO SETUP");
sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
Serial.begin(115200);
delay(10);
Serial.println("SETUP COMPLETO");

}
void loop() {
Serial.println("INICIANDO LOOP");
if (WiFi.status() != WL_CONNECTED) {
    Conect_internet();
}
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
sensors.requestTemperatures(); // Send the command to get temperatures

temp = sensors.getTempCByIndex(0);

//String input = String(("{\"TEMPERATURA\":\") + temp + "(\"}");
// contruye el input para POST
String input = "{\"TEMPERATURA\":";
input+= temp;
input+= "}";

// POST, escribe la Temperatura en el servidor
 HTTPClient http;
if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
  http.begin("http://192.168.0.20:80/snippets/");  //Specify destination for HTTP request
  // ojo la URL y el puerto pueden variar
  http.addHeader("Content-Type", "application/json");             //Specify content-type header
Serial.print("JSON: ");
Serial.println(input);

int httpResponseCode = http.POST(input); //FUNCIONA
//("{\"name\":\"value\"}")

Serial.println("La temperatura es " + String(temp) +"°C");// mensaje con la temperatura

  if(httpResponseCode>0)
  {
      String response = http.getString();                       //Get the response to the request
      Serial.print("CODIGO: ");
      Serial.println(httpResponseCode);   //Print return code
      if (httpResponseCode == 201)
      {Serial.println("EXITO, valor ingresado a la base de datos.");
    }

  }
  else
  {Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    Serial.println("");
    Serial.println("Desconectando");
    WiFi.disconnect();
    Conect_internet();}
  http.end();  //Free resources
  }else
  {Serial.println("Error in WiFi connection");
    Serial.println("");
    Serial.println("Desconectando");
    WiFi.disconnect();
    Conect_internet();}

delay(time2);  //Send a request every  minute
}
