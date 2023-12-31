
#include <SoftwareSerial.h>

SoftwareSerial SIM900(7, 8);

#define RED 3
#define GREEN 5
#define BLUE 6
#define RELAY1 11
#define RELAY2 13

boolean gas_state = false;
int sensorValue = 0;

void setup() {
  
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);
  
  // Arduino communicates with SIM900 GSM shield at a baud rate of 19200
  // It is made sure that corresponds to the baud rate of the module
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  
  setupColor(255, 255, 255);
  Serial.begin(19200);
  SIM900.begin(19200);
  
  // time is taken for GSM shield log on to network
  //10,000ms == 10 seconds
  delay(10000);
  Serial.println("\t+-----------------+");
  Serial.println("\t        OK         ");
  Serial.println("\t+-----------------+\n");
  
}

void loop() {
  setupColor(0, 0, 255);
  if (gas_state != false) {
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY1, LOW);
    Serial.println("\t+---------------------------+");
    Serial.println("\t       LEAK DETECTED         ");
    Serial.println("\t+---------------------------+\n\n");
    gas_state = true;
  }
  sensorValue = analogRead(A0);
  Serial.print("Sensor state: ");
  Serial.println(sensorValue);
  if (sensorValue >= 200) {
    if (!gas_state) {
      for (int i = 0; i < 5; i++) {
        setupColor(255, 0, 0);
        if (i >= 4) {
          delay(10000);
          digitalWrite(RELAY2, LOW);
          digitalWrite(RELAY1, LOW);
          gas_state = true;
          sendSMS3();
          break;
        } else {
          gas_state = false;
        }
        sendSMS3();
      }
      if (gas_state) {
        digitalWrite(RELAY2, LOW);
        digitalWrite(RELAY1, LOW);
        Serial.println("\t+---------------------------+");
        Serial.println("\t       LEAK DETECTED         ");
        Serial.println("\t+---------------------------+\n\n");
      }
    } else {
      gas_state = false;
    }
  }
  delay(100);
}
void setupColor(int red, int green, int blue) {
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}


void sendSMS3() {
  if (gas_state) {
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY1, LOW);
    gas_state = true;
  }
  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r");
  delay(150);
  // INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS are used
  SIM900.println("AT + CMGS = \"+8801728408321\""); 
  delay(150);
  
  SIM900.println("GAS LEAKED DETECTED!");
  delay(150);
  
  SIM900.println((char)26);
  delay(150);
  SIM900.println();

  delay(4400);
}
