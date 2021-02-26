/*
this is the code of sending dht11 sensor data to mqtt server(dioty) using arduino and the gsm without using AT command and it is tested.
// connection 1:-------------------------------->
               Arduino                                Gsm
               D8                     |                Rx
               D7                     |                Tx
               Gnd                    |                Gnd
// Connection 2:-------------------------------->
               Arduino                                DHT11
               D4                    |                 pin2
               +5V                   |                 pin1
               GND                   |                 Pin4
               
               Connect 10k resistance between pin1 and pin2 of DHT 11 sensor
*/
// you can check the list of supportable modem with this library in examples 
#define TINY_GSM_MODEM_SIM800                                              // Select your modem:                                    
#include <TinyGsmClient.h>                                                 // Library for the interfacing of arduino with the gsm without using AT command 
#include <PubSubClient.h>                                                  // Arduino Mqtt library
#include <DHT.h>                                                           // DHT sensor library
#define DHTTYPE DHT11                                                      // Defining the type of dht sensor
#define DHTPIN  4                                                          // Defining the pin to which dht11 is connected to the arduino
#include <Adafruit_Sensor.h>
#include <SoftwareSerial.h>

// Your DIoTY settings for mqtt online cloud broker
#define dioty_id    "amardeepverma113@gmail.com"                            // email address used to register with DIoTY
#define dioty_password  "e664c885"                                          // your DIoTY password received at your email
#define clientNr        "01"                                                // used for client id (increment when deploying
                                    

// Some project settings reqd for DIoTY Broker

#define slash     "/"                                                      // all topics are prefixed with slash and your dioty_id
#define topicConnect    "/ESP8266/connected"                               // topic we are publishing to
DHT dht(DHTPIN, DHTTYPE, 11);                                              // Creating the referance of dht11 sensor
float humidity, temp_t;                                                    // variable for holding the data of temp and humidity

// DIoTY requires topic names to start with your userId, so stick it in the front
#define concat(first, second) first second                                 // Defining the concatenation of the string 
const char* connectTopic = concat(concat(slash, dioty_id), topicConnect);  // concatenation the slash and dioty_id and topicConnect

const char apn[]  = "imis/internet";                                       // Your network internet apn
SoftwareSerial SerialAT(7, 8); // TX,Rx

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

const char* broker = "mqtt.dioty.co";

// failing to use unique client id's will cause you connection to drop, 
// so make it unique's unique by using your userId and a sequence number
const char* client_id = concat(clientNr, dioty_id);
long lastMsg = 0;
char msg[50];
int value = 0;
String temperature,Humidity;

void setup()
{
  // Set console baud rate
  Serial.begin(115200);
  delay(10);
  SerialAT.begin(9600);                                                  // Set GSM module baud rate
  dht.begin();                                                           // initialization of the Dht sensor 
  setup_wifi();
  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
}
  void setup_wifi() 
  {
    delay(10);
   // Restart takes quite some time
   // To skip it, call init() instead of restart()
   Serial.println("Initializing modem...");
   modem.restart();                                                       // Restart the Connection with the gsm module 
   String modemInfo = modem.getModemInfo();                               // Return the information of the module 
   Serial.print("Modem: ");
   Serial.println(modemInfo);

   Serial.print("Waiting for network...");
   if (!modem.waitForNetwork())                                          // Checking the network of your sim card
   {
     Serial.println(" fail");
     while (true);
   }
   Serial.println(" OK");
   Serial.print("Connecting to ");
   Serial.print(apn);
   if (!modem.gprsConnect(apn, dioty_id, dioty_password))
    {
     Serial.println(" fail");
     while (true);
    }
   Serial.println(" connected");
  }
void reconnect() 
{
  
   Serial.print("Connecting to ");
  Serial.print(broker);
  // Loop until we're reconnected
  while (!mqtt.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    ++value;  //connection attempt
    if (mqtt.connect(client_id, dioty_id, dioty_password))                       // checking the connection with the Dioty id and password
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      snprintf (msg, 50, "Successful connected to publish topic #%ld", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      mqtt.publish(connectTopic, msg, true);// printing the message at the gievn topic
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {

  if (!mqtt.connected()) {
      reconnect();
  } 
 mqtt.loop();
    // Reconnect every 10 seconds
//small code snippet to increment value for every publish
 
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;

//creating the json
  gettemperature();
           String payload = "{";
          payload += "\"temperature\":"; 
          payload += temperature;
          payload += ",";
          payload += "\"Humidity\":"; 
          payload += Humidity;
          payload += "}";
  
//encoding the json into an array
  
  char attributes[100];
  payload.toCharArray( attributes, 100 );


//publishing the json encoded array
      
      Serial.print("Publish message: ");
      Serial.println(attributes);
      mqtt.publish(connectTopic, attributes, true);
  }
}

void gettemperature() {  

   
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_t = dht.readTemperature();     // Read temperature as Celsius

          Serial.print("Humidity: ");
          Serial.print(humidity);
          Serial.print(" %\t");
          Serial.print("Temperature: ");
          Serial.print(temp_t);
          Serial.print(" *C ");
           temperature = String(temp_t);
          Humidity = String(humidity);
        
          // Just debug messages
          Serial.print( "Sending temperature and humidity : [" );
          Serial.print( temperature );
          Serial.print( "," );
          Serial.print( Humidity );
          Serial.print( "]   -> " );
  }

