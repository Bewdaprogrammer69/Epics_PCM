#include <Stepper.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Key.h>
#include <Keypad.h>
//------------------Block for LCD--------------------------------//
LiquidCrystal_I2C lcd(0x3f,16,2);
//------------------Block for keypad-----------------------------//
const byte ROWS = 4; // four rows
const byte COLS = 3; // three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'0','0','0'}
};
byte rowPins[ROWS] = {8,7,6,5}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {4,2,3};    // connect to the column pinouts of the keypad
Keypad kpad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//------------------Block for ultrasonic-------------------------//
int trigPin=12; //Sensor Trip pin connected to Arduino pin 13
int echoPin=11;  //Sensor Echo pin connected to Arduino pin 11
float pingTime;  //time for ping to travel from sensor to target and return
float targetDistance; //Distance to Target in cms
float speedOfSound;
//-----------------Block for stepper----------------------------//
const int steps = 2048; //this has been changed to 2048 for the 28BYJ-48
Stepper stepper(steps,28,24,26,22); //defining stepper Stepper with pins location   
//----------------Block for variables---------------------------//
int count = 0;
int on_p;//variable which indicates target positon if on_p!= 0 then it is not on paper.
int tobe,temppin=A7;
boolean start = true;
float temp,tempVolt;
boolean go = true;
char go1;
byte x = 0;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  tempVolt = (analogRead(temppin)*0.00488758553);
  temp = ((tempVolt-0.5)*100.0)+273.15;
  speedOfSound = (0.5144444*(643.855*pow((temp/273.15),0.5)));
  Serial.print(temp);
  stepper.setSpeed(5);
//  StepperLeft.setSpeed(5);
  lcd.init();
  lcd.backlight();

}
 
void loop() {
  // put your main code here, to run repeatedly:
  if(start == true) {
    count=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("count?");
    tobe = kpad.waitForKey();
    tobe = (tobe - '0');
    lcd.setCursor(0,1);
    Serial.print(count);
    lcd.print(tobe);
    delay(2000);
    start = false;
    go = true;
    go1 = 'y';
  }
  if(go == true) {
    stepper.step(steps);
  }
  digitalWrite(trigPin, LOW); //Set trigger pin low
  delayMicroseconds(15); //Let signal settle
  digitalWrite(trigPin, HIGH); //Set trigPin high
  delayMicroseconds(15); //Delay in high state
  digitalWrite(trigPin, LOW); //ping has now been sent
  delayMicroseconds(10); //Delay in low state
  
  pingTime = pulseIn(echoPin, HIGH);  //pingTime is presented in microceconds
  pingTime = pingTime/1000000; //convert pingTime to seconds by dividing by 1000000 (microseconds in a second)
  targetDistance = (speedOfSound * pingTime);  //This will be in cm per sec. 
  Serial.print("The Distance to Target is: ");
  Serial.print(targetDistance);
  Serial.println(" unnits");
  if(count<tobe) {
   if(targetDistance < 1 ) {
      on_p++;
   }  
     if(targetDistance > 1) {
       if(on_p > 1) {
         count++;
         on_p=0;
      }
    }
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(count);
  if(count==tobe) {
    start = true;
    go = false;
    go1 = 'N';
  }
  Serial.println(count);
}


