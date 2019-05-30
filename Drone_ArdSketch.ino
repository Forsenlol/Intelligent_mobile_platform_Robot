//ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//WiFi Access Point to connect
const char* ssid = "tps";
const char* password = "viewsonic";

const char* host = "10.77.70.186";
const int   port = 8181;

int pin1 = 16;
int pin2 = 0;
int pin3 = 4;
int pin4 = 5;
int pin_spd_r = 12;
int pin_spd_l = 14;

int sens_pin_l = 13;
int sens_pin_r = 15;

int i = 0;

#define Vcc 5.0

int v_l;
int v_r;

WiFiClient client;

int LMD = 90;

int cspdM1 = 0;
int cspdM2 = 0;

int dspdM1 = 0;
int dspdM2 = 0;

int drone_mode = 0;

void setup() {
//PinMode
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  
  pinMode(pin_spd_l, OUTPUT);
  pinMode(pin_spd_r, OUTPUT);
// Set initial rotation direction
  digitalWrite(pin_spd_l, LOW);
  digitalWrite(pin_spd_r, HIGH);

    
  pinMode(sens_pin_l, INPUT);
  pinMode(sens_pin_r, INPUT);
  digitalWrite(sens_pin_l, LOW);
  digitalWrite(sens_pin_r, LOW);
  
//Serial
  Serial.begin(115200);
  Serial.setTimeout(25);

  connectAP();
  connectTCP();
}

// Connecting to access point
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

// Connecting to the sever
void connectTCP()   
{
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  } else {
    Serial.println("Successfully connected");
    client.setTimeout(25);
  }
}

// Main cycle
void loop() {         
  command();                      // Recognizing drone mode through server command 
  if (drone_mode == 0){                 // Stopping drone
      cspdM1 = 0;
      cspdM2 = 0;
      int toPWM_M1 = map(abs(cspdM1), 0, 100, 0, 1024);
      int toPWM_M2 = map(abs(cspdM2), 0, 100, 0, 1024);
      analogWrite(pin_spd_l, toPWM_M1);
      analogWrite(pin_spd_r, toPWM_M2);
  } else if (drone_mode == 1){              // Automatic mode (following line)
      read_sens();
      Motors();  
  } else if (drone_mode == 2) {             // Manual mode (drone control from keyboard)
    parse_str();
    runMotors();
  }
}

// Line processing. Getting information about the speed
void parse_str()
{
  while (client.available()) {
    String line = client.readString();
    line = line.substring(1, line.length());
    line.trim();
    if (line.indexOf('M')>=0){
      String line_M1 = line.substring(line.indexOf(':')+1, line.indexOf(','));
      line_M1.trim();
      line = line.substring(line.indexOf(',')+1, line.indexOf('.'));
      line.trim();
      String line_M2 = line.substring(line.indexOf(':')+1, line.indexOf('.'));
      line_M2.trim();
      dspdM1 = line_M1.toInt();             // Getting a new speed for the left wheel
      dspdM2 = line_M2.toInt();             // Getting a new speed for the right wheel
    }
    else{
      dspdM1 = 0;
      dspdM2 = 0;
    }
  }
}    

// Processing of the received speed. Motor control
void runMotors()
{
    if ((cspdM1 != dspdM1)or(cspdM2 != dspdM2)){    // If the speed has changed
      if (cspdM1 != dspdM1){              // Handling the left motor speed
        cspdM1 = dspdM1;
        int toPWM_M1 = map(abs(cspdM1), 0, 100, 0, 1024);
        if (cspdM1 > 0){                // The speed of left motor increased
          digitalWrite(pin1, HIGH);
          digitalWrite(pin2, LOW);  
          Serial.println("Left: " + String(toPWM_M1));
        }
        else if (cspdM1 == 0){              // The speed of left motor became 0
          digitalWrite(pin1, LOW);
          digitalWrite(pin2, LOW);
          Serial.println("Left: " + String(toPWM_M1));
        }
        else {                      // The speed of left motor decreased
          digitalWrite(pin1, LOW);
          digitalWrite(pin2, HIGH);
          Serial.println("Left: " + String(toPWM_M1));
        }
        analogWrite(pin_spd_l, toPWM_M1);       
      }
      if (cspdM2 != dspdM2){              // Handling the right motor speed
        cspdM2 = dspdM2;
        int toPWM_M2 = map(abs(cspdM2), 0, 100, 0, 1024);
        if (cspdM2 > 0){                // The speed of right motor increased
          digitalWrite(pin3, HIGH);
          digitalWrite(pin4, LOW);
          Serial.println("Right: " + String(toPWM_M2));
        }
        else if (cspdM2 == 0){              // The speed of right motor became 0
          digitalWrite(pin3, LOW);
          digitalWrite(pin4, LOW);
          Serial.println("Right: " + String(toPWM_M2));
        }
        else {                      // The speed of right motor decreased
          digitalWrite(pin3, LOW);
          digitalWrite(pin4, HIGH);
          Serial.println("Right: " + String(toPWM_M2));
        }
        analogWrite(pin_spd_r, toPWM_M2); 
      }   
    }
}

// Receiving a signal from sensors
void read_sens()
{
  v_l = digitalRead(sens_pin_l);
  v_r = digitalRead(sens_pin_r); 
}

// Processing the received signal from the sensors. Feed rate to the motors.
void Motors(){  
  if ((v_l == 1)and(v_r == 1)){             // Resetting the speed to 0
    cspdM1 = 0;
    cspdM2 = 0;
    int toPWM_M1 = map(abs(cspdM1), 0, 100, 0, 1024);
    int toPWM_M2 = map(abs(cspdM2), 0, 100, 0, 1024);
    analogWrite(pin_spd_l, toPWM_M1);
    analogWrite(pin_spd_r, toPWM_M2);
  }
  else if ((v_l == 1)and(v_r == 0)){          // Increasing the speed of left motor, decresing - of right (negative value, backward motion)
    cspdM1 = 30;
    int toPWM_M1 = map(abs(cspdM1), 0, 100, 0, 1024);
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      analogWrite(pin_spd_l, toPWM_M1);
    cspdM2 = 20;
    int toPWM_M2 = map(abs(cspdM2), 0, 100, 0, 1024);
      digitalWrite(pin3, HIGH);
      digitalWrite(pin4, LOW);
      analogWrite(pin_spd_r, toPWM_M2);
  }
  else if ((v_l == 0)and(v_r == 1)){          // Increasing the speed of right motor, decresing - of left (negative value, backward motion)
    cspdM1 = 20;
    int toPWM_M1 = map(abs(cspdM1), 0, 100, 0, 1024);
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
      analogWrite(pin_spd_l, toPWM_M1);
    cspdM2 = 30;
    int toPWM_M2 = map(abs(cspdM2), 0, 100, 0, 1024);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, HIGH);
      analogWrite(pin_spd_r, toPWM_M2);
  }
  else{                         // Leveling of speeds
    cspdM1 = 30;
    int toPWM_M1 = map(abs(cspdM1), 0, 100, 0, 1024);
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
      analogWrite(pin_spd_l, toPWM_M1);
    cspdM2 = 30;
    int toPWM_M2 = map(abs(cspdM2), 0, 100, 0, 1024);
      digitalWrite(pin3, HIGH);
      digitalWrite(pin4, LOW);
      analogWrite(pin_spd_r, toPWM_M2);
  }
}

// Getting a command from the server
void command()
{
  while (client.available()) {              // Switching drone modes from the server
    String line = client.readString();
    line = line.substring(1, line.length());
    line.trim();
    Serial.println(line);
    if (line.indexOf ("stop") >= 0)           //Stopping drone 
      drone_mode = 0;
    else if (line.indexOf ("start") >= 0)       // Automatic mode (following line)
      drone_mode = 1;
  else if (line.indexOf ("manual") >= 0)       // Manual mode (drone control from keyboard)
    drone_mode = 2;
  }
}
