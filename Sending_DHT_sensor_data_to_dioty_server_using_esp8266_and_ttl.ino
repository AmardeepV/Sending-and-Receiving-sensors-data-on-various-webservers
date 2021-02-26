#include <ESP8266WiFi.h>
#include <PubSubClient.h>  //MQTT library for esp8266
#include <string.h>

#include <DHT.h>      // DHT sensor library
#define DHTTYPE DHT11
#define DHTPIN  2
 #include <Adafruit_Sensor.h>
// Update these with values suitable for you.
// Your WiFi settings
#define ssid      "AD"  // the SSID of your WiFi network
#define password  "adverma200" // you wifi password

// Your DIoTY settings for mqtt online cloud broker
#define mqtt_server "mqtt.dioty.co"      //broker hostname
#define port        1883                 //port no.
#define dioty_id    "amardeepverma113@gmail.com" // email address used to register with DIoTY
#define dioty_password  "e664c885"      // your DIoTY password received at your email
#define clientNr        "01"        // used for client id (increment when deploying
                                    // multiple instances at the same time)

// Some project settings reqd for DIoTY Broker

#define slash     "/"   // all topics are prefixed with slash and your dioty_id
#define topicConnect    "/ESP8266/connected"     // topic we are publishing to
DHT dht(DHTPIN, DHTTYPE, 11);  //Defining the type of the dht sensor 
float humidity, temp_t;  // variable for holding the data of temp and humidity

// DIoTY requires topic names to start with your userId, so stick it in the front
#define concat(first, second) first second// Defining the concatenation of the string 
const char* connectTopic = concat(concat(slash, dioty_id), topicConnect);  // concatenation the slash and dioty_id and topicConnect



// failing to use unique client id's will cause you connection to drop, 
// so make it unique's unique by using your userId and a sequence number
const char* client_id = concat(clientNr, dioty_id);
WiFiClient espClient;     // Creating the referance of the WifiClient class
PubSubClient client(espClient);  // Passing the referance to the pubsubClient

long lastMsg = 0;
char msg[50];
int value = 0;

String temperature,Humidity;
//String temperature;


// Function for the Connection of the wifi
void setup_wifi() 
  {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)  // Checking the Status of the Wifi
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  // Calling the WiFi.LocalIp function and printing it on Serial monitor
  
  }

// Arduino Setup function for the Defination of the pins and port 

void setup() 
  {
  Serial.begin(115200); // Setting the Baud rate 
  Serial.println("setup...");
  dht.begin();           // initialization of the Dht sensor 
  setup_wifi();      // Calling the wifi setup function 
  client.setServer(mqtt_server, port);  // Setting the Mqtt server at a given port
  }

// function for the reconnection of the wifi to the network
void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    ++value;  //connection attempt
    if (client.connect(client_id, dioty_id, dioty_password))// checking the connection with the Dioty id and password
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      snprintf (msg, 50, "Successful connected to publish topic #%ld", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(connectTopic, msg, true);// printing the message at the gievn topic
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void loop() 
{
// checking the connection if the wifi is not connected to the network then it call the reconnection function
    if (!client.connected())
    {
            reconnect();
        }
  //Calling the loop function of the pubsubclient by it's referance
  client.loop(); 

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
      client.publish(connectTopic, attributes, true);
  }
}
void gettemperature() {  

   
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_t = dht.readTemperature();     // Read temperature as Celsius
    // Check if any reads failed and exit early (to try again).
//    if (isnan(humidity) || isnan(temp_t)) {
//      Serial.println("Failed to read from DHT sensor!");
//      return;
 // float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
 // float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
 // float f = dht.readTemperature(true);
  //  }
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


