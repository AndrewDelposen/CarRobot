#include <Arduino.h>

//GLOBAL VARIABLES
int BASE_SPEED = 125; //125 = "half power"
int SMALL_SLOW_SPEED_CHANGE = 113; //10% decrease for sensor slightly hitting black
int SMALL_FAST_SPEED_CHANGE = 134; //10% increase
int MED_SLOW_SPEED_CHANGE = 94; //25% decrease
int MED_FAST_SPEED_CHANGE = 156; //25% increase


//////Motors definitions//////////
#define Motors_nD2 4 // Enable Pin
//Motor L(Left)
#define MotorL_DIR 6 // Direction Pin
#define MotorL_PWM 9 // PWM Pin
//Motor R(Right)
#define MotorR_DIR 8 // Direction Pin
#define MotorR_PWM 10 // PWM Pin
//////IR REFLECTION definitions//////
#define RIGHT_SENSOR_PIN 2
#define LEFT_SENSOR_PIN 1

void setup(){

//Motors Setup...
    ///////SET PINS////////
    pinMode(Motors_nD2, OUTPUT); // Enable Pin
    // Motor Left
    pinMode(MotorL_DIR, OUTPUT); // Direction Pin
    pinMode(MotorL_PWM, OUTPUT); // PWM Pin
    // Motor Right
    pinMode(MotorR_DIR, OUTPUT); // Direction Pin
    pinMode(MotorR_PWM, OUTPUT); // PWM Pin
    /////ENABLE CONTROLLER//////
    // enable motor controller
    digitalWrite(Motors_nD2, HIGH);

	//Start moving left motor
    //analogWrite(MotorL_PWM, BASE_SPEED); //"Half power"
    //digitalWrite(MotorL_DIR, HIGH); //HIGH = 1, meaning forward
    //Start moving right motor
    //analogWrite(MotorR_PWM, BASE_SPEED); //"Half power"
    //digitalWrite(MotorR_DIR, HIGH); //HIGH = 1, meaning forward

// cnt 
    //ext
    Serial.begin(9600);
}

//Local vars for loop
    int rightSensorVal;
    int leftSensorVal;
    int readQD(int PIN);
void loop(){
    rightSensorVal = readQD(RIGHT_SENSOR_PIN);
    leftSensorVal = readQD(LEFT_SENSOR_PIN);
    
    Serial.print("Left Sensor Value: "); 
    Serial.println(leftSensorVal);
    Serial.print("Right Sensor Value: ");
    Serial.println(rightSensorVal);

    if(leftSensorVal >= 3000)
        Serial.print("B ");
    else
        Serial.print("W ");
    if(rightSensorVal >= 3000)
        Serial.println("B ");
    else
        Serial.println("W ");

    delay(1000);
}

/*
	leftSensorVal = readQD(LEFT_SENSOR_PIN);
	Serial.print("Left sensor value: "); Serial.println(leftSensorVal);
	
	if(leftSensorVal <= 799){
		continue; //don't waste processing time w/ other checks
	}
	else if(leftSensorVal >= 800 && leftSensorVal <= 850){
		//Slow left wheel by SMALL amount
		analogWrite(MotorL_PWM, SMALL_SLOW_SPEED_CHANGE);
		//Speed right wheel by SMALL amount
		analogWrite(MotorR_PWM, SMALL_FAST_SPEED_CHANGE);
	}
	else if(leftSensorVal >= 851 && leftSensorVal <= 1200){
		//Slow left wheel by MEDIUM amount
		analogWrite(MotorL_PWM, SMALL_SLOW_SPEED_CHANGE);
		//Speed right wheel by MEDIUM amount
		analogWrite(MotorR_PWM, SMALL_FAST_SPEED_CHANGE);
	}
	else if (leftSensorVal >= 1300){ //basically on black
		//Active stop BOTH wheels
		analogWrite(MotorL_PWM, 0);
		analogWrite(MotorR_PWM, 0);

		//Set left wheel to backwards direction
		digitalWrite(MotorL_DIR, LOW); //HIGH = 1, meaning forward

		//MOVE BOTH wheels to base speeds
		analogWrite(MotorL_PWM, BASE_SPEED);
		analogWrite(MotorR_PWM, BASE_SPEED);

		//MOVE until left sensor is OFF black line
		while(leftSensorVal > 850){
			leftSensorVal = readQD(LEFT_SENSOR_PIN);
		}
		
		//Active stop both wheels
		analogWrite(MotorL_PWM, 0);
		analogWrite(MotorR_PWM, 0);
		
		//Set left wheel to forward direction
		digitalWrite(MotorL_DIR, HIGH); //HIGH = 1, meaning forward
		
		//MOVE BOTH wheels to base speeds
		analogWrite(MotorL_PWM, BASE_SPEED);
		analogWrite(MotorR_PWM, BASE_SPEED);
		
		//IGNORE RIGHT WHEEL SENSOR check? 
	}
	
	rightSensorVal = readQD(RIGHT_SENSOR_PIN);
	Serial.print("Right sensor value: "); Serial.println(rightSensorVal);

	if(rightSensorVal <= 799){
		continue; //don't waste processing time w/ other checks
	}
	else if(rightSensorVal >= 800 && rightSensorVal <= 850){
		right wheel slow by SMALL %
		left wheel fast by SMALL %
	}
	else if(rightSensorVal >= 851 && rightSensorVal <= 1200){
		right wheel slow by MEDIUM %
		left wheel fast by MEDIUM %
	}
	else if (rightSensorVal >= 1300){ //basically on black
		active stop
		right wheel set to backwards
		right wheel forward
		left wheel forward
		while(rightSensorVal > 850){
			rightSensorVal = readQD(LEFT_SENSOR_PIN);
		}
		active stop wheels
		set right wheel to forwards 
	}
	
*/
//IMPORTANT IR REFLECTION DATA
// robot @ line    | L SENSOR | R SENSOR
// perfect between | 650-700      | 800
// left side partly over | 800-850 | x
// left sensor halfway over | 950-1200 | x
// left sensor on black | 1300 <= | x

// void setup() {
//     Serial.begin(9600);
// }

// void loop() {
//     int rightSensorVal = readQD(RIGHT_SENSOR_PIN);
//     int leftSensorVal = readQD(LEFT_SENSOR_PIN);

//     Serial.print("Left Sensor Value: "); 
//     Serial.println(leftSensorVal);
//     Serial.print("Right Sensor Value: ");
//     Serial.println(rightSensorVal);

//     // if(sensorVal >= 3000)
//     //     Serial.print("B ");
//     // else
//     //     Serial.print("W ");

//      delay(1000);
// }

//Get value from IR Reflection sensor based on PIN#
int readQD(int PIN){
    //Echo the sensor
    pinMode(PIN, OUTPUT);
    digitalWrite(PIN, HIGH);
    delayMicroseconds(10);
    pinMode(PIN, INPUT);
    //Figure out actual value
    long time = micros();
    while (digitalRead(PIN) == HIGH && micros() - time < 3000);
         int value = micros() - time;
         return value;
}
