#include "DHT.h"
#include <WiFiEsp.h>
#include "SoftwareSerial.h"
#include <Adafruit_Sensor.h>

#define WIFI_AP "DREAMWALLETS"
#define WIFI_PASSWORD "DW123456#"
char server[] = "api.thingspeak.com";  // server name
String apiKey ="JX1DL1YFRACWSD3Y";  //Insert your write api here

// DHT
#define DHTPIN A5
#define DHTTYPE DHT11

// Initialize the Ethernet client object
 WiFiEspClient client;


// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial soft(7, 8); // TX, RX

int status = WL_IDLE_STATUS;
unsigned long lastSend;

// HUMIDITY
// A5 has already been defined as Analog Pin 5 in arduino 
int H=A5;    

// Variable to store the humidity value
float hum=0;

void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  dht.begin();
  InitWiFi(); 
  lastSend = 0;
}

void loop()
{
  status = WiFi.status();
  if ( status != WL_CONNECTED)
  {
    while ( status != WL_CONNECTED) 
    {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }
  if ( !client.connected() ) 
  {
    reconnect();
  }
    getAndSendDatas();
}
void getAndSendDatas()
 {
   Serial.println("Collecting  datas.");
   HUMIDITY();
  
      // Create the JSON for the data to be uploaded on the Cloud

      String data;
      
      data += "{";
      data += "\"field1\":\"";              //door_operation either OPEN (0) or CLOSED (1)
      data += String(hum);
      data += "\"";

      data += "}";


     Serial.println(data);
     client.println("POST /update?api_key=JX1DL1YFRACWSD3Y HTTP/1.1");
     client.println("Host: api.thingspeak.com");
     client.println("Accept: */*");
     client.println("Content-Length: " + String(data.length()));
     client.println("Content-Type: application/json");
     client.println();
     client.println(data);
     
}

void InitWiFi()
{
  // initialize serial for ESP module
  soft.begin(9600);
  // initialize ESP module
  WiFi.init(&soft);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to Thingspeak server ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect(server, 80) ) {
      Serial.println( "Connected" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
     // Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

    
void HUMIDITY()
{
      // Define the pin from where the DHT values will be read
      int chk = dht.read(H);

      // Call the humidity function of the DHT11 Library to read the value and 
      // store in a variable
      hum = dht.readHumidity();  

      // Prints the value of the humidity on the Serial Monitor
      Serial.print("Humidity =  ");
      Serial.println(hum);
}
