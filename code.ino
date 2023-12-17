#include <NewPing.h>  //import libraries
#include <LiquidCrystal.h>

const int echo_L = 2;  //initialize pin numbers
const int trig_L = 3;
const int echo_M = 4;
const int trig_M = 5;
const int echo_R = 7;
const int trig_R = 8;

int motorLpin1 = 6;
int motorLpin2 = 9;
int motorRpin1 = 10;
int motorRpin2 = 11;
// const int button = 12;
// const int pump = 13;
int motor_speed = 255;   //speed of the motor can be set between 125 (minimum) and 255 (maximum)
int max_distance = 200;  //max distance of ultrasonic sensors is set to 200cm
int distance_L = 0;
int distance_M = 0;
int distance_R = 0;
char incomingByte;

NewPing sonar_L(trig_L, echo_L, max_distance);  //initialize all the 3 sensors
NewPing sonar_M(trig_M, echo_M, max_distance);
NewPing sonar_R(trig_R, echo_R, max_distance);
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);  //initialize LCD

void setup() {
  //intitialize pins as output or input
  pinMode(motorLpin1, OUTPUT);
  pinMode(motorLpin2, OUTPUT);
  pinMode(motorRpin1, OUTPUT);
  pinMode(motorRpin2, OUTPUT);
  // pinMode(button, INPUT);
  // pinMode(pump, OUTPUT);
  stop();
  // digitalWrite(pump, LOW);
  lcd.begin(16, 2);
  lcd.print("Welcome!");
  Serial.begin(9600);  //begin serial communication via bluetooth at 9600 baud rate
  delay(1000);
}


void loop() {
  
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if (incomingByte == 'M') {
      lcd.clear();  //manual mode
      lcd.print("Manual Mode");
      while (true) {
        manualMode();
        if (incomingByte == 'A') {
          stop();
          break;
        }
      }
      delay(100);
    } else if (incomingByte == 'A') {
      lcd.clear();
      lcd.print("Automatic Mode");
      while (true) {
        automaticMode();
        if (incomingByte == 'S') {
          stop();
          break;
        }
      }
      delay(100);
    }
  }
}

void manualMode() {
  stop();
  if (Serial.available() > 0)  //check if any data is available
  {
    incomingByte = Serial.read();  //read incoming data
    switch (incomingByte)          //based on received character execute respective commands
    {
      case 'F':
        forward();
        lcd.clear();
        lcd.print("Forward");
        incomingByte = '*';
        break;

      case 'B':
        backward();
        lcd.clear();
        lcd.print("Backward");
        incomingByte = '*';
        break;

      case 'L':
        left();
        lcd.clear();
        lcd.print("Left");
        incomingByte = '*';
        break;

      case 'R':
        right();
        lcd.clear();
        lcd.print("Right");
        incomingByte = '*';
        break;

      case 'S':
        stop();
        lcd.clear();
        lcd.print("Stop");
        incomingByte = '*';
        break;

        delay(5000);
    }
  }
}

void automaticMode() {
  distance_L = readSensor_L();  //read distance from all the 3 sensors
  distance_M = readSensor_M();
  distance_R = readSensor_R();
  lcd.clear();  //print distance on LCD
  lcd.print("L=");
  lcd.print(distance_L);
  lcd.print("cm ");
  lcd.print("M=");
  lcd.print(distance_M);
  lcd.print("cm");
  lcd.setCursor(0, 1);
  lcd.print("R=");
  lcd.print(distance_R);
  lcd.print("cm");

  incomingByte = Serial.read();
  if (incomingByte == 'S') {
    stop();
  }

  if (distance_M <= 20)  //if middle sensor distance is less than 20cm
  {
    if (distance_R == distance_L) {
      stop();
      delay(200);
      backward();
      delay(2000);
      right();
      delay(2000);
    }

    if (distance_R > distance_L)  //check if there is place at right or left
    {
      if ((distance_R <= 20) && (distance_L <= 20))  //if there is no place on both sides
      {
        stop();
        delay(200);
        backward();  //move back
        delay(2000);
      } else {
        backward();  //move back then turn right
        delay(500);
        right();
        delay(2000);
      }
    } else if (distance_R < distance_L) {
      if ((distance_R <= 20) && (distance_L <= 20)) {
        stop();  //move back
        delay(200);
        backward();
        delay(2000);
      } else {
        backward();  //move back then turn left
        delay(500);
        left();
        delay(2000);
      }
    }
  }

  else if (distance_R <= 15)  //if right sensor distance is less than 20cm
  {
    left();  //turn left
    delay(500);
  } else if (distance_L <= 15)  //if left sensor distance is less than 20cm
  {
    right();  //turn right
    delay(500);
  } else {
    forward();  //in all other cases keep on moving forward
  }
}

int readSensor_L()  //read distance in centimeters from left sensor
{
  delay(70);
  int cm_L = sonar_L.ping_cm();
  if (cm_L == 0) {
    cm_L = 250;
  }
  return cm_L;
}

int readSensor_M()  //read distance in centimeters from left sensor
{
  delay(70);
  int cm_M = sonar_M.ping_cm();
  if (cm_M == 0) {
    cm_M = 250;
  }
  return cm_M;
}

int readSensor_R()  //read distance in centimeters from left sensor
{
  delay(70);
  int cm_R = sonar_R.ping_cm();
  if (cm_R == 0) {
    cm_R = 250;
  }
  return cm_R;
}

void forward() {
  lcd.print("F");
  digitalWrite(motorLpin1, HIGH);
  digitalWrite(motorLpin2, LOW);
  digitalWrite(motorRpin1, HIGH);
  digitalWrite(motorRpin2, LOW);
}

void backward() {
  lcd.print("B");

  digitalWrite(motorLpin1, LOW);
  digitalWrite(motorLpin2, HIGH);
  digitalWrite(motorRpin1, LOW);
  digitalWrite(motorRpin2, HIGH);
}

void left() {
  lcd.print("L");

  digitalWrite(motorLpin1, LOW);
  digitalWrite(motorLpin2, HIGH);
  digitalWrite(motorRpin1, HIGH);
  digitalWrite(motorRpin2, LOW);
}

void right() {
  lcd.print("R");

  digitalWrite(motorLpin1, HIGH);
  digitalWrite(motorLpin2, LOW);
  digitalWrite(motorRpin1, LOW);
  digitalWrite(motorRpin2, HIGH);
}

void stop() {
  lcd.print("S");

  digitalWrite(motorLpin1, LOW);
  digitalWrite(motorLpin2, LOW);
  digitalWrite(motorRpin1, LOW);
  digitalWrite(motorRpin2, LOW);
}
