#include <HUSKYLENS.h>
#include <Wire.h>
#include <Servo.h>

Servo xservo;
Servo yservo;

int servoxpin = 5;
int servoypin = 6;
int laserpin = A3;

int xc=0.0;
int yc=0.0;
int wx=0.0;
int wy=0.0;

float distanceX = 0.0;
float distanceY = 0.0;
float adj = 0.0;

float tanx = 0.0;
float tany = 0.0;

#define RAD_TO_DEG 57.2957786

const int minPixelHor = 60;
const int lowPixelHor = 130;
const int highPixelHor = 190;
const int startAngleHor = 180;
const int maxServoHor = 180;
const float deltaHor    = 1.5;
const byte servoPinHor  = 9;

const int minPixelVert = 10;
const int lowPixelVert = 90;
const int highPixelVert = 150;
const int startAngleVert = 90;
const int maxServoVert = 180;
const float deltaVert    = 1.0;
const byte servoPinVert  = 10;

const int trackID = 1;


struct servoType {
  Servo servo;
  int angle;
  float fAngle;
  float delta;
  int maxServoAngle;
  void init(byte aPin, int startAngle, int aDelta, int MaxServoAngle) {
    servo.attach(aPin);
    angle = startAngle;
    fAngle = startAngle;
    delta = aDelta;
    maxServoAngle = MaxServoAngle;
    servo.write(angle);
  }
  void movePlus() {
    move(1);
  }
  void moveMinus() {
    move(-1);
  }
  void move(int dir) {
    fAngle += dir * delta;
    angle = constrain(fAngle, 0, maxServoAngle);
    servo.write(angle);
    if (fAngle < 0.0) { fAngle = 0.0; }
    if (fAngle > maxServoAngle) { fAngle = maxServoAngle; }
  }
};

HUSKYLENS huskylens;
servoType horizontal;
servoType vertical;

void setup() {
  horizontal.init(servoPinHor, startAngleHor, deltaHor, maxServoHor);   
  vertical.init(servoPinVert, startAngleVert, deltaVert, maxServoVert);  
  Serial.begin(115200);
  pinMode(laserpin, OUTPUT);
  digitalWrite(laserpin, LOW);
  
  xservo.attach(servoxpin);
  yservo.attach(servoypin);
  Wire.begin();
  while (!huskylens.begin(Wire)) {
    Serial.println(F("HUSKYLENS not connected!"));
    delay(100);
  }
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);
}

void loop() {
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    else if(!huskylens.available()) Serial.println(F("Nothing Found!"));
    else
    {
      while (huskylens.available())
        {
            HUSKYLENSResult result = huskylens.read();
            printResult(result);
            if (result.ID == trackID) {
              handlePan(result.xCenter);
              handleTilt(result.yCenter);
              delay(50);  // Add a delay to allow the servo to move to the new position
              
              
            }
        }
        
        
    }
}





void handlePan(int xCenter) {
  byte mode = 0;
  if (xCenter > minPixelHor && xCenter < lowPixelHor) { mode = 1; }
  if (xCenter > highPixelHor) { mode = 2; }
  switch (mode) {
    case 0:  // No change with x_center below minPixelHor or between lowPixelHor and highPixelHor
      break;
    case 1:  // Increase servo angle
      horizontal.movePlus();
      break;
    case 2:  // Decrease servo angle
      horizontal.moveMinus();
      break;
  }
}

void handleTilt(int yCenter) {
  byte mode = 0;
  if (yCenter > minPixelVert && yCenter < lowPixelVert) { mode = 1; }
  if (yCenter > highPixelVert) { mode = 2; }
  switch (mode) {
    case 0:  
      break;
    case 1:  
      vertical.movePlus();
      break;
    case 2:  
      vertical.moveMinus();
      break;
  }
}

void printResult(HUSKYLENSResult &Result) {

  Serial.print("Object tracked at X: ");
  Serial.print(Result.xCenter);
  Serial.print(", Y: ");
  Serial.print(Result.yCenter);
  Serial.print(", Width: ");
  Serial.print(Result.width);
  Serial.print(", Height: ");
  Serial.print(Result.height);
  Serial.print(", Tracked ID: ");
  Serial.println(Result.ID);

}
