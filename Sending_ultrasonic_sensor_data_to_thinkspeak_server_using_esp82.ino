#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

 WiFiClient client;
const char* ssid     = "New_connection";
const char* password = "@ADverma82";
const char* Srver = "api.thingspeak.com";
String apiKey ="WS1QC2ZXMKXT8XXQ";
const int trigPin = 2;
const int echoPin = 0;
long duration;
int distance;
ESP8266WebServer server;
String webString=""; 
 
 void setup(void)
{
 
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v ttl cable
//    WiFi.mode(WIFI_AP_STA)
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input;
  WiFi.begin(ssid,password);
   while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.print("Connecting..");
   }
     Serial.println("Connected to");
        Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  }
  
  void loop()
{
 
  server.handleClient();
  
     if (client.connect(Srver, 80)) {
      Serial.println("WiFi Client connected ");
   
   String postStr ="?api_key="+apiKey;
   postStr += "&field1=";
   Ultrasonic();
   postStr += String(distance);
   postStr +=" cm";
   postStr += "\r\n\r\n";
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(1000);
   }
//  Serial.println(ssid);
//  Serial.print("IP address: ");
 // Serial.println(WiFi.localIP());
   // server.on("/",[](){server.send(200,"text/plain","Humidity is ");});
 // server.on("/", gettemperature);
  // server.on("/", [](){ 
  Ultrasonic();        // read sensor
//    webString="Humidity: "+String((int)humidity)+"%";
//    Serial.println("humidity is");
//      Serial.println(webString);
//    server.send(200, "text/plain",webString );               
//  });     
   server.begin();
  

}


void Ultrasonic()
{
  digitalWrite(trigPin, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
// Calculating the distance
//distance= duration*0.034029/2;
distance =(duration/2)/29.1;
Serial.print("Distance: ");
Serial.print(distance);
Serial.println(" cm");
//delay(2000);
}
