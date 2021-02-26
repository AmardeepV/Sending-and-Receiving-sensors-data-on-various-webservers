
#define TINY_GSM_MODEM_SIM800                           // Select your modem:                                    
#include <TinyGsmClient.h>                                                                                                                                                          
#include <SoftwareSerial.h>

#define server "api.thingspeak.com"
//#define apiKey "9KH9WPJV0CN3PUUL"

// DHT
#include <DHT.h>                                      // DHT sensor library   
#define DHTPIN A5                                     // Defining the pin to which dht11 is connected to the arduino
#define DHTTYPE DHT11                                 // Defining the type of dht sensor    
DHT dht(DHTPIN, DHTTYPE);
#include <Adafruit_Sensor.h>

// Your GPRS credentials
const char apn[]  = "imis/internet";                  // Your network internet apn
      //APN for airtel:- airtelgprs.com 
      // APN for idea:- imis/internet
      // APN for vodafone:-   www
      // APN for reliance:- rcomnet
      // APN for bsnl:- bsnlnet
      // APN for Aircel:- aircelgprs.pr
const char user[] = "";                               // Leave empty, if missing user or pass
const char pass[] = "";
SoftwareSerial SerialAT(7, 8);                        // TX,Rx

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

//Humidity
int H=A5;                                             // A5 has already been defined as Analog Pin 5 in arduino 
float hum=0;                                          // Variable to store the humidity value 
unsigned long lastSend;
////////////////////////////////////////////////////////////  SETUP FUNCTION  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void setup()
  {
    // Set console baud rate
    Serial.begin(115200);
    delay(10);
    SerialAT.begin(9600);                              // Set GSM module baud rate
    dht.begin();                                       // initialization of the Dht sensor 
    setup_wifi();
    lastSend = 0;
  }
//////////////////////////////////////////////////////////// GSM CONNECTION FUNCTION  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  void setup_wifi() 
    {
      delay(10);
     // Restart takes quite some time
     // To skip it, call init() instead of restart()
     Serial.println("Initializing modem...");
     modem.restart();                                 // Restart the Connection with the gsm module 
     String modemInfo = modem.getModemInfo();         // Return the information of the module 
     Serial.print("Modem: ");
     Serial.println(modemInfo);
     Serial.print("Waiting for network...");
     if (!modem.waitForNetwork())                     // Checking the network of your sim card
     {
       Serial.println(" fail");
       while (true);
     }
     Serial.println(" OK");
     Serial.print("Connecting to ");
     Serial.print(apn);
     if (!modem.gprsConnect(apn, user, pass))
      {
       Serial.println(" fail");
       while (true);
      }
     Serial.println(" connected");
    }
////////////////////////////////////////////////////////////////// LOOP FUNCTION //////////////////////////////////////////////////////////////////////////////////////////////////////////////  
    void loop()
     {
       if ( !client.connected() )                   // Checking connection with the client
          {
           reconnect();                             // If the client is not connected then reconnect 
          }
          Serial.println(" connected");
          getAndSendDatas();                        // BY Calling getAndSendDatas function posting the data to the server
          client.stop();                            // After successfull posting the data on server stoping the client communication.This is the most important step 
                                                    // if you don't do this then the connection remain open hence it will send the data once or twice.
          delay(20000);                             // Thinkspeak server take data at a duration of 15 sec
     }
////////////////////////////////////////////////////////////////  JSON AND POSTING DATA FUNCTION //////////////////////////////////////////////////////////////////////////////////////////////    
    void getAndSendDatas()
     {
       Serial.println("Collecting  datas.");
       HUMIDITY();                                   // calling Humidity function
      
      
          // Create the JSON for the data to be uploaded on the Cloud
    
          String data;
          
          data += "{";
          data += "\"field1\":\"";              //door_operation either OPEN (0) or CLOSED (1)
          data += String(hum);
          data += "\"";
    
          data += "}";
    
    
         Serial.println(data);                                             //printing the data on serial monitor
         //Making http post request
         client.println("POST /update?api_key=9KH9WPJV0CN3PUUL HTTP/1.1");  //replace api key with your api_key="paste here"
         client.println("Host: api.thingspeak.com");
         client.println("Accept: */*");
         client.println("Content-Length: " + String(data.length()));
         client.println("Content-Type: application/json");
         client.println();
         client.println(data);  
    }
///////////////////////////////////////////////////////////// RECONNECTION FUNCTION  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void reconnect() 
    {
      // Loop until we're reconnected
      while (!client.connected())
       {
      Serial.print("Connecting to Thingspeak server ...");
      // Attempt to connect (clientId, username, password)
      if ( client.connect(server, 80) )
         {
          Serial.println( "Connected" );
         } 
         else 
            {
             Serial.print( "[FAILED] [ rc = " );
             // Serial.print( client.state() );
             Serial.println( " : retrying in 5 seconds]" );
            // Wait 5 seconds before retrying
            delay( 5000 );
            }
      }
   }

////////////////////////////////////////////////////////////////////////////  HUMIDITY SENSOR ////////////////////////////////////////////////////////////////////////////////////////////////    
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
