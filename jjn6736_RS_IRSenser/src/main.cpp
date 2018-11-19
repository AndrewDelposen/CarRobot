#include <Arduino.h>

#define sensor A0

void setup() {
   Serial.begin(9600);
   pinMode(sensor, INPUT);
}

void loop() {
    int value = 0;
    int avg = 0;
    int distance = 0;

    for(int i = 0; i<10 ;i++)
    value = value + analogRead(sensor);
    avg = value / 10;
    distance = 6127.9*pow(avg, -0.9);
    Serial.println(distance);
   //Serial.println(avg);
    delay(1000);
}