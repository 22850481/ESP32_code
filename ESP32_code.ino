#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <WiFi.h>
#include <ESP32Servo.h>


#define LED_BUILTIN_PIN 13
#define SERVER_PORT 4080
#define PIN_FS90 17                       // Output pin used for Servo Motor
#define MOTOR_DRIVER_ENABLE_PIN 18        //PWM signal to motor driver "ENA" pin to signal speed to drive motors
#define MOTOR_DRIVER_IN1_PIN 14           //Digital signal to motor driver "IN1" pin to signal forwards or backwards
#define MOTOR_DRIVER_IN2_PIN 12           //Digital signal to motor driver "IN2" pin to signal forwards or backwards


// Set your access point network credentials...replace with your own credentials
const char* ssid = "********";              
const char* password = "********";

//setting PWM parameters
//const int twentyfive_dutyCycle_for_Motor = 64;        //not used since its too slow
const int fifty_dutyCycle_for_Motor = 127;
const int seventyfive_dutyCycle_for_Motor = 191;
const int hundred_dutyCycle_for_Motor = 255;

 
Servo FS90;          // Create a servo object
WiFiServer server(SERVER_PORT);

void setup() {
  
  // initialize USB serial converter
  Serial.begin();
  
  // initialize digital pins
  pinMode(LED_BUILTIN_PIN, OUTPUT);   //used for debugging purposes
  pinMode(MOTOR_DRIVER_IN1_PIN, OUTPUT);    
  pinMode(MOTOR_DRIVER_IN2_PIN, OUTPUT);

  //initialise pwm pins
  FS90.setPeriodHertz(50); // PWM frequency for FS90
  pinMode(MOTOR_DRIVER_ENABLE_PIN, OUTPUT);
  analogWrite(MOTOR_DRIVER_ENABLE_PIN, fifty_dutyCycle_for_Motor);    //set speed of RC Car to intially be 50%
  
  
  // attach the channel to the GPIO to be controlled
  FS90.attach(PIN_FS90, 900, 2100); // Minimum and maximum pulse width (in µs) to go from 0° to 180
  FS90.write(90);                   //by default, steering is centred
    

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.begin();

  // Print your local IP address:
  Serial.print("ESP32 #2: TCP Server IP address: ");
  Serial.println(WiFi.localIP());

  
}

void loop() {
  
  //Index for RC Car movement:
  //
  //"1" received to steer car left..."2" received to reset steering
  //"3" received to steer car right..."2" received to reset steering
  //"5" received to move car backwards..."6" received to stop movement
  //"7" received to move car forwards..."8" received to stop movement
  //"65" received for 25% speed
  //"66" received for 50% speed
  //"67" received for 75% speed
  //"68" received for 100% speed

  

  WiFiClient client = server.available();
  
  if (client) {
    while (client.connected()) {
      while (client.available()>0) {
        int command = client.read();
        Serial.print("ESP32 #2: - Received command: ");
        Serial.println(command);

        
        //section to steer RC car left
        if ((command == int(1))||(command == int(49)))
          FS90.write(40);
        else if ((command == int(2))||(command == int(50)))
          FS90.write(90);


         //section to steer RC car right
        if ((command == int(3))||(command == int(51)))
          FS90.write(140);
          
        
         //section to make RC car move backwards
        if ((command == int(5))||(command == int(53)))
        {
          //write digital pins to move backwards (IN1-low ; IN2-high)
          digitalWrite(MOTOR_DRIVER_IN1_PIN, LOW);
          digitalWrite(MOTOR_DRIVER_IN2_PIN, HIGH);
        }
        else if ((command == int(6))||(command == int(54)))
        {
          //write digital pins to stop movement (IN1-low ; IN2-low)
          digitalWrite(MOTOR_DRIVER_IN1_PIN, LOW);
          digitalWrite(MOTOR_DRIVER_IN2_PIN, LOW);
        }


         //section to make RC car move forwards
        if ((command == int(7))||(command == int(55)))
        {
          //write digital pins to move forwards (IN1-high ; IN2-low)
          digitalWrite(MOTOR_DRIVER_IN1_PIN, HIGH);
          digitalWrite(MOTOR_DRIVER_IN2_PIN, LOW);
        }
        else if ((command == int(8))||(command == int(54)))
        {
          //write digital pins to stop movement (IN1-low ; IN2-low)
          digitalWrite(MOTOR_DRIVER_IN1_PIN, LOW);
          digitalWrite(MOTOR_DRIVER_IN2_PIN, LOW);
        }


        //section to set RC car speed
        if (command == int(65))
        {
          //analogWrite(MOTOR_DRIVER_ENABLE_PIN, twentyfive_dutyCycle_for_Motor);
        }
        else if (command == int(66))
        {
          analogWrite(MOTOR_DRIVER_ENABLE_PIN, fifty_dutyCycle_for_Motor);
        }
        else if (command == int(67))
        {
          analogWrite(MOTOR_DRIVER_ENABLE_PIN, seventyfive_dutyCycle_for_Motor);
        }
        else if (command == int(68))
        {
          analogWrite(MOTOR_DRIVER_ENABLE_PIN, hundred_dutyCycle_for_Motor);
        }
      }
      
      //delay(10);
    }
    client.stop();
  }
  
}
