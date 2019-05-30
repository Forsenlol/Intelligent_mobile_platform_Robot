//ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//WiFi Access Point to connect
const char* ssid = "tps";
const char* password = "viewsonic";

const char* host = "10.77.70.179";
const int   port = 8080;

int pin1 = 14;
int pin2 = 12;
int pin3 = 13;
int pin4 = 15;

WiFiClient client;

void setup() {
  //PinMode
  pinMode(pin1,OUTPUT);
  pinMode(pin2,OUTPUT);
  pinMode(pin3,OUTPUT);
  pinMode(pin4,OUTPUT);
  
  //Serial
  Serial.begin(115200);
  Serial.setTimeout(25);

  connectAP();
  connectTCP();
}

void connectAP()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectTCP()
{
  if (!client.connect(host, port)) {
      Serial.println("connection failed");
      return;
    }else{
      Serial.println("Successfully connected");
      client.setTimeout(25);
    }
}

void loop() {
  checkSerial();
  checkTCP();
}

void checkSerial()
{
  while(Serial.available()){
    String line = Serial.readString();
    //client.print(line);

    if (line.indexOf("f") != -1)
      {
        digitalWrite(pin1,HIGH);
        digitalWrite(pin2,LOW);
        digitalWrite(pin3,HIGH);
        digitalWrite(pin4,LOW); 
      }else

      if (line.indexOf("s")!= -1)
      {
        digitalWrite(pin1,LOW);
        digitalWrite(pin2,LOW);
        digitalWrite(pin3,LOW);
        digitalWrite(pin4,LOW); 
      }else

      if (line.indexOf("l")!= -1)
      {
        digitalWrite(pin1,LOW);
        digitalWrite(pin2,HIGH);
        digitalWrite(pin3,HIGH);
        digitalWrite(pin4,LOW); 
      }else

      if (line.indexOf("r")!= -1)
      {
        digitalWrite(pin1,HIGH);
        digitalWrite(pin2,LOW);
        digitalWrite(pin3,LOW);
        digitalWrite(pin4,HIGH); 
      }
  }
}

void checkTCP()
{
  // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readString();
      line = line.substring(1,line.length());
      line.trim();
      Serial.println(line);

      if (line.indexOf('f')>=0)
      {
        Serial.println("forward");
        digitalWrite(pin1,HIGH);
        digitalWrite(pin2,LOW);
        digitalWrite(pin3,HIGH);
        digitalWrite(pin4,LOW); 
      }else

      if (line.indexOf('s')>=0)
      {
        digitalWrite(pin1,LOW);
        digitalWrite(pin2,LOW);
        digitalWrite(pin3,LOW);
        digitalWrite(pin4,LOW); 
      }else

      if (line.indexOf('l')>=0)
      {
        digitalWrite(pin1,LOW);
        digitalWrite(pin2,HIGH);
        digitalWrite(pin3,HIGH);
        digitalWrite(pin4,LOW); 
      }else

      if (line.indexOf('r')>=0)
      {
        digitalWrite(pin1,HIGH);
        digitalWrite(pin2,LOW);
        digitalWrite(pin3,LOW);
        digitalWrite(pin4,HIGH); 
      }
    }
}


