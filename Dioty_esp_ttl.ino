#include <DHT.h>  
#include <ESP8266WiFi.h>
#include <PubSubClient.h>  

#define ssid      "AD"  
#define password  "adverma200" 

#define mqtt_server "mqtt.dioty.co"      
#define port        1883                 
#define dioty_id    "sylvesterferns@gmail.com" 
// This is the password which you receive in your email
#define dioty_password  "1b7b49ad"   

// used for client id (increment when deploying multiple instances at the same time)
#define clientNr        "01"        
                                    

// all topics are prefixed with slash and your dioty_id
#define slash     "/"   

// topic we are publishing to
#define topicConnect    "/ESP8266"   

// DIoTY requires topic names to start with your userId, so stick it in the front
#define concat(first, second) first second
const char* connectTopic = concat(concat(slash, dioty_id), topicConnect);


// failing to use unique client id's will cause you connection to drop, 
// so make it unique's unique by using your userId and a sequence number
const char* client_id = concat(clientNr, dioty_id);
WiFiClient espClient;     
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

DHT dht(2, DHT11, 11); 


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



void loop() {
 
  while (WiFi.status() != WL_CONNECTED)  // Checking the Status of the Wifi
  {
    WiFi.begin(ssid, password);
    Serial.print(".");
        delay(500);
  }
    if (!client.connected())
    {
            reconnect();
        }
         delay(1000);
  getAndSendTemperatureAndHumidityData();
  client.loop(); 
}

float getHumidity()
{
      // Reading temperature or humidity takes about 250 milliseconds!
      // Call the humidity function of the DHT11 Library to read the value and 
      // store in a variable
      float h = dht.readHumidity();  

      // Prints the value of the humidity on the Serial Monitor
      Serial.print("Humidity =  ");
      Serial.print(h);
      Serial.println(" %");
      
      return h;
}

float getTemp()
{
      // Reading temperature or humidity takes about 250 milliseconds!
      // Call the humidity function of the DHT11 Library to read the value and 
      // store in a variable
      float t = dht.readTemperature(true); 

      // Prints the value of the humidity on the Serial Monitor
      Serial.print("Temperature =  ");
      Serial.print(t);
      Serial.println(" *C ");

      return t;
}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  float h = getHumidity();
  
  float t = getTemp();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temperature\":"; 
  payload += String(t); 
  payload += ",";
  payload += "\"humidity\":"; 
  payload += String(h);
  payload += "}";
        
  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  Serial.print("Publish message: ");
  Serial.println(attributes);
  client.publish(connectTopic, attributes, true);
}

