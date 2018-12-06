#include <Arduino.h>

//GLOBAL VARIABLES
int BASE_SPEED = 125; //125 = "half power"
int SMALL_SLOW_SPEED_CHANGE = 113; //10% decrease for sensor slightly hitting black
int SMALL_FAST_SPEED_CHANGE = 134; //10% increase
int MED_SLOW_SPEED_CHANGE = 94; //25% decrease
int MED_FAST_SPEED_CHANGE = 156; //25% increase


//===========Motors definitions==========
#define Motors_nD2 4 // Enable Pin
//Motor L(Left)
#define MotorL_DIR 6 // Direction Pin
#define MotorL_PWM 9 // PWM Pin
//Motor R(Right)
#define MotorR_DIR 12 // Direction Pin
#define MotorR_PWM 10 // PWM Pin
//==========IR REFLECTION definitions========
#define RIGHT_SENSOR_PIN 11
#define LEFT_SENSOR_PIN 1
//==========ULTRASONIC defintions=========
#define TRIGGER_PIN 5
#define ULTRASENSOR_PIN 3

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

//Ultrasonic Setup...
	pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ULTRASENSOR_PIN, INPUT);

//Serial Setup...
	Serial.begin(9600);

	delay(2000);

//Starting movement...
	//Start moving left motor
    analogWrite(MotorL_PWM, BASE_SPEED); 
    digitalWrite(MotorL_DIR, HIGH); //HIGH = 1, meaning forward
    //Start moving right motor
    analogWrite(MotorR_PWM, BASE_SPEED);
    digitalWrite(MotorR_DIR, HIGH); //HIGH = 1, meaning forward
}


//Local vars for loop
    int rightSensorVal;
    int leftSensorVal;
    int readQD(int PIN);
	long convertUltra(long duration);
	long disance;
	bool other_sensor_black;
	int clearance;
void loop(){

//Left sensor check...
	leftSensorVal = readQD(LEFT_SENSOR_PIN);
	Serial.print("Left sensor value: "); Serial.println(leftSensorVal);
	if(leftSensorVal <= 750); //don't waste processing time w/ other checks
	
	else if(750 < leftSensorVal && leftSensorVal <= 950){
		//Slow left wheel by SMALL amount
		analogWrite(MotorL_PWM, SMALL_SLOW_SPEED_CHANGE);
		//Speed right wheel by SMALL amount
		analogWrite(MotorR_PWM, SMALL_FAST_SPEED_CHANGE);
	}
	else if(950 < leftSensorVal && leftSensorVal <= 1300){
		//Slow left wheel by MEDIUM amount
		analogWrite(MotorL_PWM, MED_SLOW_SPEED_CHANGE); //was small_slow_speed_change
		//Speed right wheel by MEDIUM amount
		analogWrite(MotorR_PWM, MED_FAST_SPEED_CHANGE); //was small_fast_speed_change
	}
	else{ //basically on black all values GREATER than 1300
		//Active stop BOTH wheels
		analogWrite(MotorL_PWM, 0);
		analogWrite(MotorR_PWM, 0);

		//Set left wheel to backwards direction
		digitalWrite(MotorL_DIR, LOW); //HIGH = 1, meaning forward

		//MOVE BOTH wheels to base speeds
		analogWrite(MotorL_PWM, BASE_SPEED);
		analogWrite(MotorR_PWM, BASE_SPEED);

		other_sensor_black = false;
		clearance = 0;
		//MOVE until left sensor is OFF black line
		while(leftSensorVal > 850 || other_sensor_black){
			leftSensorVal = readQD(LEFT_SENSOR_PIN);
			rightSensorVal = readQD(RIGHT_SENSOR_PIN);
			if(leftSensorVal < 850 && rightSensorVal > 1200){
				other_sensor_black = true;
				analogWrite(MotorL_PWM, SMALL_SLOW_SPEED_CHANGE);
				analogWrite(MotorR_PWM, MED_FAST_SPEED_CHANGE);
			}
			else if(leftSensorVal < 850 && rightSensorVal < 1200){
				if(clearance == 0){
					clearance++;
				}
				else {
					other_sensor_black = false;
				}
			}
		}
		
		//if left is black, goes into turning
		//if left is out of black from turning, but right is in black
		//keep turning left till right is out of black

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
//Right Sensor check...
	rightSensorVal = readQD(RIGHT_SENSOR_PIN);
	Serial.print("Right sensor value: "); Serial.println(rightSensorVal);
	if(rightSensorVal <= 450){} //don't waste processing time w/ other checks
	else if(450 < rightSensorVal && rightSensorVal <= 600){ //was 600, 1200
		//Slow right wheel by SMALL amount
		analogWrite(MotorR_PWM, SMALL_SLOW_SPEED_CHANGE);
		//Speed left wheel by SMALL amount
		analogWrite(MotorL_PWM, SMALL_FAST_SPEED_CHANGE);
	}
	else if(600 < rightSensorVal && rightSensorVal <= 1200){ //WAS NOT HERE BEFORE
		//Slow right wheel by SMALL amount
		analogWrite(MotorR_PWM, MED_SLOW_SPEED_CHANGE);
		//Speed left wheel by SMALL amount
		analogWrite(MotorL_PWM, MED_FAST_SPEED_CHANGE);
	} 
	else{ //basically on black all values GREATER than 1200
		//Active stop BOTH wheels
		analogWrite(MotorL_PWM, 0);
		analogWrite(MotorR_PWM, 0);

		//Set right wheel to backwards direction
		digitalWrite(MotorR_DIR, LOW); //HIGH = 1, meaning forward

		//MOVE BOTH wheels to base speeds
		analogWrite(MotorL_PWM, BASE_SPEED);
		analogWrite(MotorR_PWM, BASE_SPEED);

		other_sensor_black = false;
		clearance = 0;
		//MOVE until right sensor is OFF black line
		while(rightSensorVal > 800 || other_sensor_black){
			rightSensorVal = readQD(RIGHT_SENSOR_PIN);
			leftSensorVal = readQD(LEFT_SENSOR_PIN);

			if(rightSensorVal < 800 && leftSensorVal > 1200){
				other_sensor_black = true;
				analogWrite(MotorR_DIR, LOW);
				analogWrite(MotorR_PWM, 50);
				analogWrite(MotorL_PWM, MED_FAST_SPEED_CHANGE);
			}
			else if(leftSensorVal < 800 && rightSensorVal < 1200){
				if(clearance <= 1){
					clearance++;
				}
				else {
					other_sensor_black = false;
				}
			}
		}

		//Active stop both wheels
		analogWrite(MotorL_PWM, 0);
		analogWrite(MotorR_PWM, 0);
		
		//Set RIGHT wheel to forward direction
		digitalWrite(MotorR_DIR, HIGH); //HIGH = 1, meaning forward
		
		//MOVE BOTH wheels to base speeds
		analogWrite(MotorL_PWM, BASE_SPEED);
		analogWrite(MotorR_PWM, BASE_SPEED);
		
		//IGNORE RIGHT WHEEL SENSOR check? 
	}
	delay(2);
}

// SAVE for getting raw values from lines
/*
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
*/
//IMPORTANT IR REFLECTION DATA
// robot @ line    			| L SENSOR 				| R SENSOR
// perfect between 			| 650-700, <= 750  		| <= 450
// left side partly over 	| 800-850, 750 <= 950	| 450 < 600
// left sensor halfway over | 950-1200,950 <= 1300	| 600 < 1000
// left sensor on black 	| 1300 <=, 1300<=		| 1000
	
/*void loop(){
	long distance; //MOVE OUTSIDE OF LOOP
	
	//Get a pulse of data
	digitalWrite(TRIGGER_PIN, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIGGER_PIN, HIGH);
	delayMicroseconds(5);
	digitalWrite(TRIGGER_PIN, LOW);

	//Read the data from sensor
	distance = convertUltra(pulseIn(ULTRASENSOR_PIN, HIGH));
	Serial.print("Disance = ");Serial.println(distance);

	delay(10);
}
*/

/*
	if(distance <= xxx){
		//Active stop both wheels
		analogWrite(MotorL_PWM, 0);
		analogWrite(MotorR_PWM, 0);
		
		//Switch direction of wheel
		
		//wheel turns for x time to get 90 degrees
		
		//active stop
		
		//reset wheel direction
		
		//forward
		//UNTIL lidar stops hitting
		
		//forward for x time (get back end to exceed the obj)
		
		//active stop
		
		//switch direction of wheel
		//wheel turns for x time to get 90 degrees
		
		//active stop
		
		//forward
		//UNTIL lidar hits
		
		//continue forward
		//UNTIL lidar stops hitting
		
		//move forward a bit more??? x time
		
		//active stop
		
		//switch direction of wheels
		//move until 90 degrees for xxx time
		
		//switch direction
		
		//move forward
		//UNTIL black line detected by both???
		//.....????
		
		
	}
	
	//Print out the correct real world example
	Serial.println(distance);
}
*/


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

//Convert value into cm
long convertUltra(long duration){
	return (0.0168 * duration) - 0.3704;
}