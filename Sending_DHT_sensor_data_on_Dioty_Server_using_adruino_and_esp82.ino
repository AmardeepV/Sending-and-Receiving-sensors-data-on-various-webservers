#include <DHT.h>
#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>
#include "SoftwareSerial.h"
// #include <Adafruit_Sensor.h>

#define WIFI_AP "AD"
#define WIFI_PASSWORD "adverma200"


#define mqtt_server "mqtt.dioty.co"
#define port        1883                 //port no.
#define dioty_id    "amardeepverma113@gmail.com" // email address used to register with DIoTY
#define dioty_password  "e664c885"      // your DIoTY password
#define clientNr        "01"        // used for client id (increment when deploying
                                    // multiple instances at the same time)

// Some project settings reqd for DIoTY Broker

#define slash     "/"   // all topics are prefixed with slash and your dioty_id
#define topicConnect    "/ESP8266"     // topic we are publishing to

// DIoTY requires topic names to start with your userId, so stick it in the front
#define concat(first, second) first second
const char* connectTopic = concat(concat(slash, dioty_id), topicConnect);



// failing to use unique client id's will cause you connection to drop, 
// so make it unique's unique by using your userId and a sequence number
const char* client_id = concat(clientNr, dioty_id);
WiFiEspClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

// DHT
#define DHTTYPE DHT11
#define DHTPIN  4    
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266
int status = WL_IDLE_STATUS;
SoftwareSerial soft(7, 8); // Tx,Rx

unsigned long lastSend;

void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  dht.begin();
  InitWiFi();
    client.setServer( mqtt_server, port );
lastSend = 0;
}

void loop() {
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  client.loop();
}

  void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

  String temperature = String(t);
  String humidity = String(h);


  // Just debug messages
  Serial.print( "Sending temperature and humidity : [" );
  Serial.print( temperature ); Serial.print( "," );
  Serial.print( humidity );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"humidity\":"; payload += humidity;
  payload += "}";
        
          // Send payload
          char attributes[100];
          payload.toCharArray( attributes, 100 );
         Serial.print("Publish message: ");
              Serial.println(attributes);
              client.publish(connectTopic, attributes, true);
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



void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
   ++value;  //connection attempt
    if (client.connect(client_id, dioty_id, dioty_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      snprintf (msg, 50, "Successful connected to publish topic #%ld", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(connectTopic, msg, true);
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print( client.state() );
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

