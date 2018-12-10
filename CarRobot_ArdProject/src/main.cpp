#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

//GLOBAL VARIABLES
int BASE_SPEED = 125; //125 = "half power"
uint32_t RIGHT_TURN_TIME = 713; //in seconds
uint32_t LEFT_TURN_TIME = 765; //in seconds
int CLEARANCE_TIME = 500; //in microseconds
int LIDAR_CLOSE_TRIGGER = 1;
int LIDAR_FAR_TRIGGER = 1;

//Parking variables
uint32_t DETECTION_DELAY = 3000;
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
#define FRONT_LED_PIN 1
#define REAR_L_LED_PIN 0
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


//Lidar sensor Setup...
	Wire.begin();
	lid_sensor.init();
	lid_sensor.setTimeout(500);
	//Take constant readings
	lid_sensor.startContinuous();

//LEDs Setup...
	pinMode(FRONT_LED_PIN, OUTPUT);
	pinMode(REAR_L_LED_PIN, OUTPUT);
	pinMode(REAR_R_LED_PIN, OUTPUT);
	//Turn on front LEDs
	digitalWrite(FRONT_LED_PIN, HIGH); //HIGH = on

//Serial Setup...
	Serial.begin(9600);

	delay(5000);

//Starting movement...
	//Start moving left motor
	digitalWrite(MotorL_DIR, HIGH); //HIGH = 1, meaning forward
	analogWrite(MotorL_PWM, BASE_SPEED); 
    //Start moving right motor
	digitalWrite(MotorR_DIR, HIGH); //HIGH = 1, meaning forward
	analogWrite(MotorR_PWM, BASE_SPEED+5);
}


//Local vars for loop
	int clearance;
	long lidar_value;
	bool did_hit = false;
  void parallel_Park();
  void pull_Out();
void loop(){
  //PARKING WITH LIDAR 
    int distance;
    distance = lid_sensor.readRangeContinuousMillimeters();
    Serial.print("Distance: ");
    Serial.println(distance);
    if (lid_sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

    //Detecting a spot to park in
    if(distance < 150){
      delay(DETECTION_DELAY);
      for(int i = 0; i < 20; i++){
        distance = lid_sensor.readRangeContinuousMillimeters();
        if(distance < 150){
          parallel_Park();
          pull_Out();
          break;
        }
        delay(100);
      }
    }

    //TESTING PARKING ALGORYTHM
    //parallel_Park();
    delay(100);
}

//PARKING CONSTANTS
uint32_t REVERSE_TIME = 1000;
int REVERSE_SPEED = 75;
int SLOW_MOTOR_SPEED = 50;
int FAST_MOTOR_SPEED = 150;
int TURN_TIME = 1500;
void parallel_Park(){
    //ACTIVE STOP
    analogWrite(MotorL_PWM, 0);
    analogWrite(MotorR_PWM, 0);

    //TURN ON TAIL LIGHTS
    digitalWrite(REAR_L_LED_PIN, HIGH);
    digitalWrite(REAR_R_LED_PIN, HIGH);

    //PUT IN REVERSE
    digitalWrite(MotorL_DIR, LOW);
    digitalWrite(MotorR_DIR, LOW);

    analogWrite(MotorL_PWM, REVERSE_SPEED);
    analogWrite(MotorR_PWM, REVERSE_SPEED);

    delay(REVERSE_TIME);

    //START TURNING INTO SPOT
    analogWrite(MotorL_PWM, SLOW_MOTOR_SPEED);
    analogWrite(MotorR_PWM, FAST_MOTOR_SPEED);

    delay(TURN_TIME);

    //STRAIGHTEN OUT
    analogWrite(MotorL_PWM, FAST_MOTOR_SPEED);
    analogWrite(MotorR_PWM, SLOW_MOTOR_SPEED);

    delay(TURN_TIME);

    //STOP
    analogWrite(MotorL_PWM, 0);
    analogWrite(MotorR_PWM, 0);

    delay(5000);
    
    digitalWrite(REAR_L_LED_PIN, LOW);
    digitalWrite(REAR_R_LED_PIN, LOW);
}
int FORWARD_SPEED = 75;
void pull_Out(){

    digitalWrite(MotorL_DIR, HIGH);
    digitalWrite(MotorR_DIR, HIGH);
    //START TURNING INTO SPOT
    analogWrite(MotorL_PWM, FAST_MOTOR_SPEED);
    analogWrite(MotorR_PWM, SLOW_MOTOR_SPEED);

    delay(TURN_TIME);

    //STRAIGHTEN OUT
    analogWrite(MotorL_PWM, SLOW_MOTOR_SPEED);
    analogWrite(MotorR_PWM, FAST_MOTOR_SPEED);

    delay(TURN_TIME + 100);

    //STOP
    analogWrite(MotorL_PWM, BASE_SPEED);
    analogWrite(MotorR_PWM, BASE_SPEED);

    delay(500);
}
	

