// #include "QuadDecoder.h"
// #include <ADC.h>
// #include <Arduino.h>

// #define MotorA_nD2 4 // Enable Pin
// #define MotorA_DIR 6 // Direction Pin
// #define MotorA_PWM 9 // PWM Pin
// #define MotorA_nSF 12 // Fault Pin
// #define MotorA_FB A0 // Current Feedback Pin

// #define TicksPerRev 2249 // Number of ticks per wheel revolution
// #define Period 10000     // How fast velocity is calculated in microseconds

// QuadBase *encoderA = new QuadDecoder<1>(); //New Quadrature Encoder Object
// IntervalTimer velTimer;
// ADC *adc = new ADC(); // adc object
// int power_val = 125;


// void setup()
// {
//     // Quadtrature decoder setup
//     encoderA->init(TicksPerRev, 0);
//     encoderA->setFilter(31);
//     //encoderA->invertCHB(); // Uncomment if Motor is backwards
//     encoderA->reset();
//     encoderA->begin();

//     // Pin Assignments
//     pinMode(MotorA_nD2, OUTPUT); // Enable Pin
//     pinMode(MotorA_DIR, OUTPUT); // Direction Pin
//     pinMode(MotorA_PWM, OUTPUT); // PWM Pin
//     pinMode(MotorA_nSF, INPUT);  // Fault Pin
//     pinMode(MotorA_FB, INPUT);   // Current Feedback Pin

//     // enable motor controller
//     digitalWrite(MotorA_nD2, HIGH);

//     // Start Serial Monitor
//     Serial.begin(9600);
//     delay(100);

//     // Timer for velocity
//     velTimer.begin(calcVel, Period);

//     // Set ADC
//     adc->setAveraging(4);                                     // set number of averages
//     adc->setResolution(12);                                   // set bits of resolution
//     adc->setConversionSpeed(ADC_CONVERSION_SPEED::LOW_SPEED); // change the conversion speed
//     adc->setSamplingSpeed(ADC_SAMPLING_SPEED::LOW_SPEED);     // change the sampling speed
//     adc->startContinuous(MotorA_FB, ADC_0);

// 	//Initial settings and power to motor to start moving
//     analogWrite(MotorA_PWM, 125); //"Half power"
//     digitalWrite(MotorA_DIR, LOW); //HIGH = 1, meaning foward
// }

// void loop()
// {
// 	//Get the value of the velocity read by encoder
//     int actual = diffA * rpmConvert;

// 	//Print out information
//     Serial.print("Actual velocity = ");
//     Serial.println(actual); //Get the velocity value

//     //See if actual velocity is faster than 69
//     if(actual > 69)
//     {
//         power_val = power_val * 0.9; //Lower power to 90%
//     }
//     //See if actual velocity is slower than 66
//     else if(actual < 66){
//         power_val = power_val * 1.1; //Increase power by 10%
//     }
//     //Make sure not to overshoot value, set it to max if it is
//     if(power_val > 255){
//         power_val = 255;
//     }
//     //make sure not to undershoot value, set to a minimum if it is
//     else if(power_val < 10){
//         power_val = 10;
//     }

// 	//Write the new power to the PWM
//     analogWrite(MotorA_PWM, power_val); //"Half power"

// 	//Print out the information
//     Serial.print("New power is = ");
//     Serial.println(power_val);

//     delay(500);
// }