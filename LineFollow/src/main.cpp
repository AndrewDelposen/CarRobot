#include <Arduino.h>

#define RIGHT_SENSOR_PIN 2
#define LEFT_SENSOR_PIN 12
int readQD(int pin);

void setup() {
    Serial.begin(9600);
}

void loop() {
    int rightSensorVal = readQD(RIGHT_SENSOR_PIN);
    int leftSensorVal = readQD(LEFT_SENSOR_PIN);

    Serial.print("Left Sensor Value: "); 
    Serial.println(leftSensorVal);
    Serial.print("Right Sensor Value: ");
    Serial.println(rightSensorVal);

    // if(sensorVal >= 3000)
    //     Serial.print("B ");
    // else
    //     Serial.print("W ");

     delay(1000);
}

int readQD(int pin){
    pinMode(pin, OUTPUT);
    digitalWrite( pin, HIGH );
    delayMicroseconds(10);
    pinMode(pin, INPUT );

    long time = micros();
    while (digitalRead(pin) == HIGH && micros() - time < 3000);
         int diff = micros() - time;
         return diff;
}
