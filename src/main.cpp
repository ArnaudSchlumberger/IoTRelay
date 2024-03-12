#include <Arduino.h>
#include <ESP8266WiFi.h>

//WIFI PARAMETERS
const char* ssid = "ArnCo";
const char* password = "itlz1602";
#define LISTEN_PORT 80

WiFiServer server(LISTEN_PORT);
WiFiClient client;

//Webserver Strings variables
String header; //contient
char c; //utilisé pour concatener progresivement header

//IOs def and status
#define LED_INPUT 4 //[D2] ID of pin connected to computer LED indicator. led_state = digitalRead(LED_INPUT) at some point.
uint8_t led_state; //LED value, returned by computer. Indicates if computer is on or off.
#define RELAY_OUTPUT 5 //[D1] ID of pin connected to relay. Used to start computer.

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  pinMode(RELAY_OUTPUT, OUTPUT);
  digitalWrite(RELAY_OUTPUT, 0);

  pinMode(LED_INPUT, INPUT);

  Serial.begin(115200);
  Serial.println("\nHello !");
  delay(100);

  Serial.print("Starting Wifi connection");
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  server.setNoDelay(true);
}

void loop() {
  led_state = digitalRead(LED_INPUT);  

  client = server.accept();
  Serial.print("Clients: ");
  Serial.println(client);

  if(client){
    while(client.available() and c != '\n'){
        c = client.read();
        Serial.print(c);
        header +=c;
    }
    if(header.indexOf("GET /relay/on") >= 0){
      Serial.println("PULSE RELAY!");
      if(!led_state){
        digitalWrite(RELAY_OUTPUT, 1);
        delay(400);
        digitalWrite(RELAY_OUTPUT, 0);
      }
      else{
        Serial.println("Computer already on !");
      }
    }
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    // Display the HTML web page
    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");
    // CSS to style the on/off buttons 
    // Feel free to change the background-color and font-size attributes to fit your preferences
    client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
    client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
    client.println(".button2 {background-color: #77878A;}</style></head>");
    
    // Web Page Heading
    client.println("<body><h1>PC Master Controller 2000 Web Server</h1>");
     client.println("<p>Computer status</p>");
            // If the output5State is off, it displays the ON button       
            if (led_state==1) {
              client.println("<p><a href=\"/relay\"><button class=\"button\">PC allum&eacute</button></a></p>");
            } else {
              client.println("<p><a href=\"/relay/on\"><button class=\"button button2\">OFF</button></a></p>");
            } 
  } 
  else{
    digitalWrite(LED_BUILTIN,0);
    delay(500);
    digitalWrite(LED_BUILTIN,1);
    delay(500);
  }
  //reinitialisation header and c pour garantir entrée dans la prochaine boucle if(client)/while
  header = "";
  c = ' ';
  
}
