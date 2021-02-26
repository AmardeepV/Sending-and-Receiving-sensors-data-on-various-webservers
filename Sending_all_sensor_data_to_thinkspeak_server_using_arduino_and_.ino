/*
[In this project we are sending all the 6 sensor data to the thinkspeak server using arduino and the gsm without using AT Command.]

 URL to Check the Data on CLoud
//https://thingspeak.com/channels/227783

 Connect the + on the board to +5V on the Arduino
 Connect the - on the board to GND on the Arduino

 Connect the R on the board to 5 on the Arduino 
 Connect the P on the board to 4 on the Arduino 
 Connect the X on the board to 3 on the Arduino 

 Connect the T on the board to A2 on the Arduino 
 Connect the L on the board to A4 on the Arduino 
 Connect the H on the board to A5 on the Arduino 
*/
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

int X=3;                                              //PIR 

int P=4;                                              //POWER SWITCH

int R=5;                                              //REED SWITCH

// TEMPERATURE
int T= A2;                                            // A2 has already been defined as Analog Pin 2 in arduino 

// LIGHT
int L=A4;                                             // A4 has already been defined as Analog Pin 4 in arduino

//Humidity
int H=A5;                                             // A5 has already been defined as Analog Pin 5 in arduino 

float temp;                                           // Variable to store the temperature value

float hum=0;                                          // Variable to store the humidity value 

float lig=0;                                          // Variable to store the light

String pwr="0";                                       // 0 means OFF and 1 means ON
String door="0";                                      // 0 means CLOSED and 1 means OPEN
String person="0";                                    // 0 means NO and 1 means YES
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
       TEMPERATURE();                                // calling temperature function
       HUMIDITY();                                   // calling Humidity function
       LIGHT();                                      // calling light function
       POWER();                                      // calling power function
       REED();                                       // calling reed function
       PIR();                                        // calling pir function
      
          // Create the JSON for the data to be uploaded on the Cloud
    
          String data;
          
          data += "{";
          data += "\"field1\":\"";              //door_operation either OPEN (0) or CLOSED (1)
          data += door;
          data += "\",";
    
          data += "\"field2\":\"";              //power_status either OFF or ON
          data += pwr;
          data += "\",";
    
          data += "\"field3\":\"";              //temperature
          data += String(temp);
          data += "\",";
    
          data += "\"field4\":\"";              //humidity
          data += String(hum);
          data += "\",";
    
          data += "\"field5\":\"770\",";        //trip_id replace with your last 3 digits of mobile
          data += "\"field6\":\"Cold Chain\","; //device_type
          data += "\"field7\":\"9529853846\","; // device_no need to put your own mobile number, replace with your own mobile number
          data += "\"field8\":\"transaction\""; //type
    
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
////////////////////////////////////////////////////////////////////////// TEMPERATURE SENSOR ////////////////////////////////////////////////////////////////////////////////////////////////
    void TEMPERATURE()
    {    
            // read the value from the Analog variable 
            // and store it to an integer variable
            int value_temp = analogRead(T);   
    
            // Divide the obtained value with the resolution of the ADC (i.e., 1023.0)
            // multiply the result with reference milli volts. (5V = 5000mV)
            float millivolts_temp = ((value_temp/1023.0)*5000);
    
            // There will be 1°C change for every 10mV of output
            temp = millivolts_temp/10;
    
            // Print the output on the Serial Monitor
            Serial.print("Temperature =  ");
            Serial.println(temp);
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
///////////////////////////////////////////////////////////////  LIGHT SENSOR //////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
    void LIGHT()
    {
          // read the value from the Analog variable 
          // and store it to an integer variable
          int value_lig=analogRead(L);
    
          // Divide the obtained value with the resolution of the ADC (i.e., 1023.0)
          // multiply the result with reference milli volts. (5V = 5000mV)
          float millivolts_lig =(value_lig /1023.0)*5000;
    
          // There will be 1  change for every 10mV of output
          lig=millivolts_lig /10;
    
          // Prints the value of the Light Intensity on the Serial Monitor
          Serial.print("Light Intensity =  ");
          Serial.println(lig);
    }
////////////////////////////////////////////////////////////////////  POWER SENSOR  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void POWER()
    {
          Serial.print("Power :");
          if(digitalRead(P)==LOW)
          {
          // 0 means OFF and 1 means ON  
          pwr="0";
          Serial.println(" OFF");
          }
          else
          {
          // 0 means OFF and 1 means ON  
          pwr="1";  
          Serial.println(" ON");
          }
    } 
////////////////////////////////////////////////////////////  PIR SENSOR ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void PIR()
    {
          Serial.print("Person :");
          if(digitalRead(X)==LOW)
          {
          // 0 means NO and 1 means YES  
          person="1";  
          Serial.println(" YES");
          }
          else
          {
          // 0 means NO and 1 means YES  
          person="0";  
          Serial.println(" NO");
          }
    }
///////////////////////////////////////////////////////////// Reed Sensor ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void REED()
    {
          Serial.print("Door :");
          if(digitalRead(R)==LOW)
          {
          // 0 means CLOSED and 1 means OPEN  
          door="0";  
          Serial.println(" CLOSE");
          }
          else
          {
          // 0 means CLOSED and 1 means OPEN  
          door="1";  
          Serial.println(" OPEN");
          }
    }
//////////////////////////////////////////////////////////////////////////////////////END ////////////////////////////////////////////////////////////////////////////////////////////////////
