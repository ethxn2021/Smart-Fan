#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL5Ba8D0TYS"
#define BLYNK_TEMPLATE_NAME "DH11 Humidity and Temperature Sensor Data"
#define BLYNK_AUTH_TOKEN "ZzL_tAR3vN9T1A6QmOL0CC3gUVf-rzks"


#include <Arduino.h>
//Included required libraries for Adafruit Ink display 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_EPD.h>
#include <Adafruit_ThinkInk.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Ethan's Iphone 12";
char pass[] = "Ipad12345";



//Defining the width and height of the Adafruit Think  
#define SCREEN_WIDTH 200 // display width, in pixels 
#define SCREEN_HEIGHT 200 // display height, in pixels 

//Configure Pins for Adafruit display 
#define EPD_DC 21 
#define EPD_CS 22
#define SRAM_CS 13 
#define EPD_RESET 2 //can set -1 and share with microcontroller reset
#define EPD_BUSY 14 // can set -1 to not use pin (will wait fixed delay)

//Defining DHTPIN & Fan  
#define DHTPIN 25
#define DHT_TYPE DHT11  
// Create a global DHT object
DHT dht(DHTPIN, DHT_TYPE);

#define FAN_PIN 15




/// Use Adafruit_EPD object for the display
//Adafruit_EPD display(SCREEN_WIDTH, SCREEN_HEIGHT, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
ThinkInk_154_Mono_D67 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
//Pin Definitions that sensor connected to 
const int trigPin = 12; // Pin to trigger the ultrasonic sensor 
const int echoPin = 16; // Pin to receive the echo from the sensor 

//Constant for calculations & defining sound speed in cm/uS
#define SOUND_SPEED 0.034 // Speed of spund in cm/microsecond 
#define CM_TO_INCH 0.393701 // Conversion factor from cm to inches 

//Variables to store sensor data 
long duration; //Time taken for the ultrasonic signal to return 
float distanceCm; //Conversion factor from cm to inches 
float distanceInch; //Distance in inches 


// //Sample readings from DHT11 Sensor 
// temperature_readings = [22.5, 22.6, 22.7]
// humidity_readings = [45.3, 45.2, 45.4]

// //Data fusion through averaging 
// average_temperature = sum(temperature_readings) / len(temperature_readings)
// average_humidty =   sum(humidity_readings) / len(humidity_readings)


// //Sample readings from DHT11 Sensor 
// average_distance = [50.2, 50.1, 50.3]
// average_distance =   sum(distance_readings) / len(distance_readings)



// hw_timer_t* timer;
// bool updateSensor = false;
// #define TIMER_INTERVAL    1000000 // microsecond, 1000000 = 1 second

// void IRAM_ATTR tick() {
//   updateSensor = true;
// }

  void setup() {
    //baud rate 115200 to print on serial monitor
    Serial.begin(115200); //Starts the serial communcation 

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    // Initialize DHT sensor
    // dht.begin();

    // timer = timerBegin(0, 80, true);
    // timerAttachInterrupt(timer, &tick, true);
    // timerAlarmWrite(timer, TIMER_INTERVAL, true);
    // timerAlarmEnable(timer);

    
    pinMode(trigPin, OUTPUT); //Sets the trigPin as an Output 
    pinMode(echoPin, INPUT); //Sets the echoPin as an Input 

    // Attempt to initialize the eInk display
    while(!Serial) {
      delay(10);
    }
    Serial.println("Adafruit EPD full update test in mono");
    display.begin(THINKINK_MONO);
    
    display.setRotation(3); //Adjust rotation if Needed 
    display.setTextSize(2);
    display.setTextColor(EPD_BLACK);

  
  }

  void loop() {

    Blynk.run();
    // if (updateSensor) {
    //   updateSensor = false;
    // }

     //Read DHT data 
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // // Send data to Blynk
    Blynk.virtualWrite(V0, h);
    Blynk.virtualWrite(V1, t);

  //    if (isnan(h) || isnan(t))
  // {
  //   Serial.println("Failed to read from DHT sensor!");
  //   return;
  // }
    //Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    //Sets the trigPin on HIGH state for 10 microseconds 
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    //reads the echoPi, returns the sound wave travel time in microseconds 
    duration = pulseIn(echoPin, HIGH);

    //Calculate the distance 
    distanceCm = duration * SOUND_SPEED / 2;

    //Convert to inches 
    distanceInch = distanceCm * CM_TO_INCH;

    //Prints the distance in the Serial Monitor 
    Serial.println("=== Sensor Data ===");
    Serial.println("Ultrasonic:");
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);
    Serial.print("Distance (inch): ");
    Serial.println(distanceInch);

    Serial.println("DHT Humidity and Temperature:");
    Serial.print("DHT Humidity: ");
    Serial.print(h);
    Serial.print("%");
    Serial.print("  Temperature: ");
    Serial.print(t);
    Serial.println("°C");

    

    //Clear the OLED display to write new readings 
    display.clearBuffer();

     // Display distance in cm
    display.setCursor(0, 40);
    display.setTextSize(2);
    display.print("Distance: ");
    display.print(distanceCm);
    display.println(" cm");


    // Display DHT data on the OLED display
    display.setCursor(0, 80);
    display.setTextSize(1);
    display.print("DHT Humidity: ");
    display.print(h);
    display.print("%");
    display.print("  Temperature: ");
    display.print(t);
    display.println("^C");

    // Update and show the OLED display
    display.display();

    /// Check conditions to turn on the fan
    if (t > 10 && distanceCm < 8 && h > 10) {
        digitalWrite(FAN_PIN, HIGH); // Turn on the fan
    } else {
        digitalWrite(FAN_PIN, LOW); // Turn off the fan
    }

    delay(1000);
  
  }