#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

int DEMO = 3;

int SENSOR_PIN = 2;
#define trigger_PIN 12
#define ULTRASENSOR_PIN 14
int readQD();
long convertUltra(long);
VL53L0X lid_sensor;
int LEDPIN = 13;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello...");

  if(DEMO == 2){
    pinMode(trigger_PIN, OUTPUT);
    pinMode(ULTRASENSOR_PIN, INPUT);
  }

  if(DEMO == 3){
    Serial.println("Starting set up...");
    //Set up onboard LED for indication
    pinMode(LEDPIN, OUTPUT);
    
    //Start serial port reading
    Serial.begin(9600);

    //Set up of the sensor
    Wire.begin();
    lid_sensor.init();
    lid_sensor.setTimeout(500);

    //Take constant readings back and forth
    lid_sensor.startContinuous();
    delay(2500);
    Serial.println("Set up finished");
  }

}

void loop() {

  if(DEMO == 1){
    int sensor_val = readQD();

    if(sensor_val >= 3000){
      Serial.print("B "); Serial.print(sensor_val);Serial.print(" ");}
    else{
      Serial.print("W "); Serial.print(sensor_val);Serial.print(" ");}
    
    delay(2000);
  }
  if(DEMO == 2){
    long duration;

    //Get a pulse of data
    digitalWrite(trigger_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(trigger_PIN, LOW);

    //Read the data from sensor
    duration = pulseIn(ULTRASENSOR_PIN, HIGH);

    //Print out the correct real world example
    Serial.println(convertUltra(duration));

    delay(500);
  }
  if(DEMO == 3){
    //Print the information in MM
    long val = lid_sensor.readRangeContinuousMillimeters();
    Serial.println(val);

    if(val < 5000 && val > 40){
        if(val > 200)
        {
            //used to exaggerate the blinking delay
            val = val * 5;
        }
        //Turn on LED
        digitalWrite(LEDPIN, HIGH);
        //Delay depending on how close the sensor detects
        delayMicroseconds(val * 75);
        digitalWrite(LEDPIN,LOW);
    }
    
    delay(100);
  }

}

long convertUltra(long duration){
  return (0.0168 * duration) - 0.3704;
}

int readQD(){
  pinMode(SENSOR_PIN, OUTPUT);
  digitalWrite(SENSOR_PIN, HIGH);
  delayMicroseconds(10);
  pinMode(SENSOR_PIN, INPUT);

  long time = micros();
  while(digitalRead(SENSOR_PIN) == HIGH && micros() - time < 3000);
    int diff = micros() - time;
    return diff;
}