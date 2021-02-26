#include <ESP8266WiFi.h>
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
 
 WiFiClient client;
const char* ssid     = "EdisonSir";
const char* password = "Bulb'sfather";
const char* Srver = "api.thingspeak.com";
String apiKey ="LZFNJO054174CHSA";


DHT dht(DHTPIN, DHTTYPE, 11);
int h;
 
 void setup()
{
 
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v ttl cable
  dht.begin();
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

    void Humidity()
{
  Serial.println("Collecting humidity data.");
  // Reading temperature or humidity takes about 250 milliseconds!
   h = dht.readHumidity();
  Serial.print("Humidity is ");
  Serial.println(h);
  
  }

  
  void loop()
{
     if (client.connect(Srver, 80)) {
      Serial.println("WiFi Client connected ");
   
   String postStr ="?api_key="+apiKey;
   postStr += "&field1=";
  Humidity(); 
   postStr += String(h);
   postStr +=" %";
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

  Humidity();        // read senso
}
