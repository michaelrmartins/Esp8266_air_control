/*
    ------------ AIR CONTROL ESP8266 ------------

    Software de controle do Robô Explorador 
    Versão de Demosntração

    ---


Changelog:

10-08-2020 - Criação.
11-08-2020 - Mudança para ativação dos motores por PWM


 Baseado no Sketch de demonstração: 
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>

// Wifi Config
#ifndef STASSID
#define STASSID "GTI_2Ghz"
#define STAPSK  "eduardoemonica-legiao"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

// Variables
int i;
unsigned int contador = 1;
unsigned int invert_relay_counter = 1;
const int relay_1_pin = 4; //PINO DIGITAL UTILIZADO PELO RELÊ 1
const int relay_2_pin = 0; //PINO DIGITAL UTILIZADO PELO RELÊ 1
const int relay_3_pin = 2; //PINO DIGITAL UTILIZADO PELO RELÊ 1
const int relay_4_pin = 14; //PINO DIGITAL UTILIZADO PELO RELÊ 1
// const int analog_pin_1 = A0;
float sensor_1_rawdata_map;

// Relay Status / 0 = OFF / 1 = ON
int relay_1_status = 1;
int relay_2_status = 0;
int relay_3_status = 0;
int relay_4_status = 0;

// Relay Erros Status / 0 = OK / 1 = Failure
int relay_1_error = 0;
int relay_2_error = 0;
int relay_3_error = 0;
int relay_4_error = 0;

String readString = String(30); //VARIÁVEL PARA BUSCAR DADOS NO ENDEREÇO (URL)
// --------- Configuração  ------------
void setup() {
  Serial.begin(115200);

  // Pins mode Initialize
  pinMode(relay_1_pin, OUTPUT);
  pinMode(relay_2_pin, OUTPUT);
  pinMode(relay_3_pin, OUTPUT);
  pinMode(relay_4_pin, OUTPUT);
  
  // Pins State Initialize
  digitalWrite(relay_1_pin, 1);
  digitalWrite(relay_2_pin, 0);
  digitalWrite(relay_3_pin, 1);
  digitalWrite(relay_4_pin, 0);

  // Sensor Pin Initialize
  //pinMode(A0, INPUT);  // Sensor 1

  // Connect to WiFi network  - DEBUG
  /* Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);
*/
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}

// ------------- Loop Principal -------------- 
void loop() {

// Relay inversion counter
if ( invert_relay_counter >= 20 && relay_1_error == 0 && relay_2_error == 0){

  digitalWrite(relay_1_pin, !digitalRead(relay_1_pin));
  delay(1000);
  digitalWrite(relay_2_pin, !digitalRead(relay_2_pin));
  invert_relay_counter = 1; // Reset Counter

  relay_1_status = digitalRead(relay_1_pin);
  relay_2_status = digitalRead(relay_2_pin);

  }

// READ SENSOR AND CONVERT DATA
// Serial.print(analogRead(A0));
sensor_1_rawdata_map = 10;
//float sensor_1_rawdata_map=map(sensor_1_rawdata, 0, 1023, 10, 50);

// Temp monitoring
if (sensor_1_rawdata_map >=26 && relay_1_error == 0 && relay_2_error == 0)
  {
      
    // Verify which relay is activated, and change error status to 1
    if (relay_1_status == 1)
    {
      relay_1_error = 1;
    } else { relay_2_error = 1; }

    digitalWrite(relay_1_pin, 1);
    digitalWrite(relay_2_pin, 1);

    relay_1_status = digitalRead(relay_1_pin);
    relay_2_status = digitalRead(relay_2_pin);
  }

  // Check if a client has connected
  WiFiClient client = server.available();
  if (client)
 {
      while (client.connected())
      {
        if(client.available())
          {
                        // HTTP CONNECTION HEADER
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Server: Arduino");
                        // client.println("Connection: close");
                       // client.println();

                        // Create JSON File
                        client.println("{"); // Open Json
                        client.println("\"ID\":\"01\",");
                        client.println("\"LOCATION\":\"HEAA\",");
                        client.println("\"SENSOR_TEMP_1\":\"" + String(sensor_1_rawdata_map) + "\",");
                        client.println("\"RELAY_1_STATUS\":\"" + String(relay_1_status) + "\",");
                        client.println("\"RELAY_2_STATUS\":\"" + String(relay_2_status) + "\",");
                        client.println("\"RELAY_3_STATUS\":\"" + String(relay_3_status) + "\",");
                        client.println("\"RELAY_4_STATUS\":\"" + String(relay_4_status) + "\",");
                        client.println("\"RELAY_1_ERROR\":\"" + String(relay_1_error) + "\",");
                        client.println("\"RELAY_2_ERROR\":\"" + String(relay_2_error) + "\",");
                        client.println("\"RELAY_3_ERROR\":\"" + String(relay_3_error) + "\",");
                        client.println("\"RELAY_4_ERROR\":\"" + String(relay_4_error) + "\"");
                        //client.println(sensor_1_rawdata_map);
                        //client.println("\"");          
                        client.println("}"); // End Json
                        //readString=""; //A VARIÁVEL É REINICIALIZADA
                        client.stop(); //FINALIZA A REQUISIÇÃO HTTP E DESCONECTA O CLIENTE
                                        
      }
    }
 }
 
  // Debug
  //Serial.println("Rele 1 ");
  //Serial.println(relay_1_status);
  //Serial.println("Rele 2 ");
  //Serial.println(relay_2_status);
  Serial.print(invert_relay_counter);
  invert_relay_counter +=1;
  
  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  // Serial.println(F("Disconnecting from client"));
  delay(1000);
}
