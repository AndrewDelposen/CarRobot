#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

//GLOBAL VARIABLES
int BASE_SPEED = 125; //125 = "half power"
int SMALL_SLOW_SPEED_CHANGE = 113; //10% decrease for sensor slightly hitting black
int SMALL_FAST_SPEED_CHANGE = 134; //10% increase
int MED_SLOW_SPEED_CHANGE = 94; //25% decrease
int MED_FAST_SPEED_CHANGE = 156; //25% increase
uint32_t RIGHT_TURN_TIME = 950; //in seconds was 713
uint32_t LEFT_TURN_TIME = 800; //in seconds
int CLEARANCE_TIME = 300; //in microseconds
int ULTRA_DISTANCE_TRIGGER = 9; //How close to obj before object avoidance
int LIDAR_CLOSE_TRIGGER = 200;
int LIDAR_FAR_TRIGGER = 600; //Value for lidar to be considered "not hitting" object
VL53L0X lid_sensor;


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
//==========LED defintions==============
#define REAR_L_LED_PIN 14
#define REAR_R_LED_PIN 2

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

//Lidar sensor Setup...
	Wire.begin();
	lid_sensor.init();
	lid_sensor.setTimeout(500);
	//Take constant readings
	lid_sensor.startContinuous();

//LEDs Setup...
	pinMode(REAR_L_LED_PIN, OUTPUT);
	pinMode(REAR_R_LED_PIN, OUTPUT);

//Serial Setup...
	Serial.begin(9600);
	Serial.println("Settings done!");
	delay(2000);

//Starting movement...
	//Start moving left motor
	digitalWrite(MotorL_DIR, HIGH); //HIGH = 1, meaning forward
	analogWrite(MotorL_PWM, BASE_SPEED); 
    //Start moving right motor
	digitalWrite(MotorR_DIR, HIGH); //HIGH = 1, meaning forward
	analogWrite(MotorR_PWM, BASE_SPEED);
}


//Local vars for loop
    int rightSensorVal;
    int leftSensorVal;
    int readQD(int PIN);
	long convertUltra(long duration);
	bool other_sensor_black;
	int clearance;
	void ObjectAvoidance();
	void activeStop();
	void moveForward();
	void turnRight();
	void turnLeft();
	long lidar_value;
	bool did_hit = false;
	long readUltra();
	long ultra_distance = 0;


void loop(){
//Left sensor check...
	leftSensorVal = readQD(LEFT_SENSOR_PIN);
	//Serial.print("Left sensor value: "); Serial.println(leftSensorVal);
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
	//Serial.print("Right sensor value: "); Serial.println(rightSensorVal);
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
		
	}
	

//Ultrasonic sensor check...
	
	//Get value from ultrasonic sensor in cm
	ultra_distance = readUltra();
	//See if we need to go into object avoidance mode
	Serial.print("ulatra value is...");Serial.println(ultra_distance);
	if(ultra_distance <= ULTRA_DISTANCE_TRIGGER){
		Serial.println("Stopping...");
		//activeStop();
		//while(1){};
		ObjectAvoidance();
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

//Test code for the ultrasonic sensor
/*
void loop(){
	//long distance; //MOVE OUTSIDE OF LOOP
	
	//Get a pulse of data
	//digitalWrite(TRIGGER_PIN, LOW);
	//delayMicroseconds(2);
	//digitalWrite(TRIGGER_PIN, HIGH);
	//delayMicroseconds(5);
	//digitalWrite(TRIGGER_PIN, LOW);

	//Read the data from sensor
	//distance = convertUltra(pulseIn(ULTRASENSOR_PIN, HIGH));
	Serial.print("Disance = ");Serial.println(readUltra());

	delay(100);
}
*/

//Test loop for the LIDAR sensor
/*
void loop(){
	//Print the information in MM
    long val = lid_sensor.readRangeContinuousMillimeters();
    Serial.print("Value in mm: ");Serial.println(val);
	delay(100);
}
*/

//Function that will have the robot go around detected object and back onto black line
void ObjectAvoidance() {
	//Stop moving
	digitalWrite(REAR_L_LED_PIN, HIGH); //HIGH = on
	digitalWrite(REAR_R_LED_PIN, HIGH); //HIGH = on
	activeStop();
	//Turn 90 degrees to the right
	digitalWrite(REAR_L_LED_PIN, LOW); //HIGH = on
	turnRight();
	digitalWrite(REAR_L_LED_PIN, HIGH); //HIGH = on

	//Move forward until lidar stops detecting
	digitalWrite(REAR_L_LED_PIN, LOW); //HIGH = on
	digitalWrite(REAR_R_LED_PIN, LOW); //HIGH = on
	moveForward();

	delay(500); //delay for robot to get orientated

	lidar_value = 0;
	while(lidar_value < LIDAR_FAR_TRIGGER){ //TODO figureout what value is "exceeding"
		lidar_value = lid_sensor.readRangeContinuousMillimeters();
		Serial.print("Lidar1 is:");Serial.println(lidar_value);
	}

	//Continue to move forward to give back of robot clearance
	delayMicroseconds(CLEARANCE_TIME);
	//Stop moving
	digitalWrite(REAR_L_LED_PIN, HIGH); //HIGH = on
	digitalWrite(REAR_R_LED_PIN, HIGH); //HIGH = on
	activeStop();
	//Turn left on self
	digitalWrite(REAR_R_LED_PIN, LOW); //HIGH = on
	turnLeft();
	digitalWrite(REAR_R_LED_PIN, HIGH); //HIGH = on
	
	//Move forward until lidar hits... and then passes object
	digitalWrite(REAR_L_LED_PIN, LOW); //HIGH = on
	digitalWrite(REAR_R_LED_PIN, LOW); //HIGH = on
	moveForward();
		
	lidar_value = 10000;
	while(lidar_value > LIDAR_CLOSE_TRIGGER){
		lidar_value = lid_sensor.readRangeContinuousMillimeters();
		Serial.print("Lidar looking for close is:");Serial.println(lidar_value);
	}

	lidar_value = 0;
	while(lidar_value < LIDAR_FAR_TRIGGER){
		lidar_value = lid_sensor.readRangeContinuousMillimeters();
		Serial.print("Lidar looking for FAR is:");Serial.println(lidar_value);
	}

	//Continue moving forward to give rear robot clearance
	delayMicroseconds(CLEARANCE_TIME);
	//stop moving
	digitalWrite(REAR_L_LED_PIN, HIGH); //HIGH = on
	digitalWrite(REAR_R_LED_PIN, HIGH); //HIGH = on
	activeStop();
	//turn left again
	digitalWrite(REAR_R_LED_PIN, LOW); //HIGH = on
	turnLeft();
	digitalWrite(REAR_R_LED_PIN, HIGH); //HIGH = on


	//Move forward until one/both sensors hit black line
	digitalWrite(REAR_L_LED_PIN, LOW); //HIGH = on
	digitalWrite(REAR_R_LED_PIN, LOW); //HIGH = on
	moveForward();
	rightSensorVal = 0;
	//while right sensor is not hitting black, keep going until it really hits black
	while(rightSensorVal < 1200){
		rightSensorVal = readQD(RIGHT_SENSOR_PIN);
	}

	/////////TODO NOTE: this is exactly from within loop... maybe make function out of it
	//Active stop BOTH wheels
	digitalWrite(REAR_L_LED_PIN, HIGH); //HIGH = on
	digitalWrite(REAR_R_LED_PIN, HIGH); //HIGH = on
	analogWrite(MotorL_PWM, 0);
	analogWrite(MotorR_PWM, 0);

	//Set right wheel to backwards direction
	digitalWrite(MotorR_DIR, LOW); //HIGH = 1, meaning forward

	//MOVE BOTH wheels to base speeds
	analogWrite(MotorL_PWM, BASE_SPEED);
	analogWrite(MotorR_PWM, BASE_SPEED);
	digitalWrite(REAR_L_LED_PIN, LOW); //HIGH = on
	digitalWrite(REAR_R_LED_PIN, LOW); //HIGH = on

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
	digitalWrite(REAR_L_LED_PIN, HIGH); //HIGH = on
	digitalWrite(REAR_R_LED_PIN, HIGH); //HIGH = on
	analogWrite(MotorL_PWM, 0);
	analogWrite(MotorR_PWM, 0);
	
	//Set RIGHT wheel to forward direction
	digitalWrite(MotorR_DIR, HIGH); //HIGH = 1, meaning forward
	
	//MOVE BOTH wheels to base speeds
	digitalWrite(REAR_L_LED_PIN, LOW); //HIGH = on
	digitalWrite(REAR_R_LED_PIN, LOW); //HIGH = on
	analogWrite(MotorL_PWM, BASE_SPEED);
	analogWrite(MotorR_PWM, BASE_SPEED);


}

//Function that turns the robot 90 degrees to thr right on itself
void turnRight(){
	//Active stop both motors
	activeStop();
	//Set right motor to go backwards
	digitalWrite(MotorR_DIR, LOW); //HIGH = 1, meaning forward
	//MOVE both motors forward
	moveForward();
	//Turn for x time
	delay(RIGHT_TURN_TIME);
	//stop
	activeStop();
	//reset direction of right motor
	digitalWrite(MotorR_DIR, HIGH);
}

//Function that turns the robot 90 degrees to the left on itself
void turnLeft(){
	//Active stop both motors
	activeStop();
	//Set right motor to go backwards
	digitalWrite(MotorL_DIR, LOW); //HIGH = 1, meaning forward
	//MOVE both motors forward
	moveForward();
	//Turn for x time
	delay(LEFT_TURN_TIME);
	//stop
	activeStop();
	//reset direction of right motor
	digitalWrite(MotorL_DIR, HIGH);
}

//Helper function to move both wheels at base speeds
void moveForward() {
	analogWrite(MotorL_PWM, BASE_SPEED);
	analogWrite(MotorR_PWM, BASE_SPEED);
}

//Helper function to stop both wheels
void activeStop() {
	analogWrite(MotorL_PWM, 0);
	analogWrite(MotorR_PWM, 0);
}

//Get value from ultrasonic sensor
long readUltra(){
	
	long running_total = 0;
	for(int x = 0; x < 5; x++){
		//Get a pulse of data
		digitalWrite(TRIGGER_PIN, LOW);
		delayMicroseconds(2);
		digitalWrite(TRIGGER_PIN, HIGH);
		delayMicroseconds(5);
		digitalWrite(TRIGGER_PIN, LOW);
		running_total += convertUltra(pulseIn(ULTRASENSOR_PIN, HIGH));
	}
	
	return running_total / 3.0;
}

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