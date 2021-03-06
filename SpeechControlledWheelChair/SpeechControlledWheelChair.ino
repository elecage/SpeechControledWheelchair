

// HCSR04, Adafruit_MCP4725, MStimer2 라이브러리를 추가해야 함
#include <MsTimer2.h>
#include <Wire.h>
#include <HCSR04.h>
#include <Adafruit_MCP4725.h>
#include "SoftwareSerial.h"
#include "VoiceRecognitionV3.h"

#define JOY_X_IN  A1
#define JOY_Y_IN  A0
#define FSR       A2

#define X_ADDR 0x61
#define Y_ADDR 0x60

#define GO    0   // 앞으로
#define BACK  1   // 뒤로
#define LEFT  2   // 좌회전
#define RIGHT 3   // 우회전
#define STOP  4   // 정지

#define DAC_RESOLUTION    (8)

#define YOFFSET (-210)
Adafruit_MCP4725 dac;
VR myVR(2, 3);

unsigned short joyXIn = 0;
unsigned short joyYIn = 0;

uint8_t records[7];
uint8_t buf[64];

//UltraSonicDistanceSensor forwardSensor(12, 13); 
//UltraSonicDistanceSensor bottomSensor(10, 11); 
UltraSonicDistanceSensor forwardSensor(10, 11); 
UltraSonicDistanceSensor bottomSensor(12, 13); 

bool fallAlert = false;
bool obstacleAlert = false;
bool joystickIn = false;
bool prevJoystickIn = false;
int forwardDistance = 0;
int bottomDistance = 0;

int currentState = STOP;

void getUltrasound()
{
  forwardDistance = forwardSensor.measureDistanceCm();
//  bottomDistance = bottomSensor.measureDistanceCm();

  if(forwardDistance < 80 && forwardDistance >= 0) // 전방 80cm 이내에 장애물이 있으면
  {
    obstacleAlert = true;
    Serial.print(forwardDistance);
    Serial.println("\t: ObstacleAlert");
  } else
  {
    obstacleAlert = false;
  }
  
  if(bottomDistance > 150) // 추락 위험이 있으면
  {
    fallAlert = true;
    Serial.print(bottomDistance);
    Serial.println("\t: Fall Alert");
  } else {
    fallAlert = false;
  }
  
}
void setup() {
  // put your setup code here, to run once:
  myVR.begin(9600);
  Serial.begin(115200);
  dac.begin(X_ADDR);
  dac.setVoltage(2048, false);    // X축 입력을 중앙으로
  dac.begin(Y_ADDR);
  dac.setVoltage(2048, false);    // Y축 입력을 중앙값으로
  Serial.println("Start wheelchair");
  MsTimer2::set(200, getUltrasound); 
  MsTimer2::start();             // 초음파를 사용하려면 앞의 //를 없앨것
  
}

void moveGo()
{
  dac.begin(X_ADDR);
  dac.setVoltage(2048 + 750, false);
  dac.begin(Y_ADDR);
  dac.setVoltage(2048-200, false);
//  MsTimer2::start();             // 초음파를 사용하려면 앞의 //를 없앨것

}
void moveBack()
{
  dac.begin(X_ADDR);
  dac.setVoltage(2048 - 500, false);
  dac.begin(Y_ADDR);
  dac.setVoltage(2048 -150 , false);
}
void moveLeft()
{
    dac.begin(X_ADDR);
  dac.setVoltage(2048, false);
  dac.begin(Y_ADDR);
  dac.setVoltage(2048 - 400, false);
}
void moveRight()
{
    dac.begin(X_ADDR);
  dac.setVoltage(2048, false);
  dac.begin(Y_ADDR);
  dac.setVoltage(2048 + 400, false);
}
void stopWheelchair()
{
    dac.begin(X_ADDR);
  dac.setVoltage(2048 , false);
  dac.begin(Y_ADDR);
  dac.setVoltage(2048 , false);
 // MsTimer2::stop();             // 초음파를 사용하려면 앞의 //를 없앨것

}

void loop() {
  // put your main code here, to run repeatedly:

  uint32_t dac_value;       // 출력 값 : 0 ~ 4096
  int ret;                  // 음성인식 반환값
  byte cmd;                 // 음성인식 결과값
  joyXIn = analogRead(JOY_X_IN);  // 조이스틱 X축 읽기
  joyYIn = analogRead(JOY_Y_IN);  // 조이스틱 Y축 읽기

  if(joyXIn >= 495 &&joyXIn <= 530 && joyYIn >= 495 && joyYIn <= 530) // 만약 조이스틱을 움직이지 않으면
  {
    joystickIn = false;     // 조이스틱 입력 없음
    
    if(prevJoystickIn == true)
    {
      Serial.println("Stop Wheelchair by Joystick input");
        stopWheelchair();
    }
  }
  else {                  // 조이스틱을 움직이면
    joystickIn = true;   // 조이스틱 입력 있음
    if(prevJoystickIn == false)
    {
      Serial.println("MoveWheelchair by Joystick Input");
    }

  }
  
 
  if(joystickIn == true) {      // 조이스틱 입력이 있으면
 //   Serial.print(joyXIn);
  //  Serial.print("     ");
  //  Serial.println(joyYIn);
 //   dac_value = map(joyXIn, 0, 1023, 819, 3276);  // 조이스틱 입력은 10비트이며 출력은 12비트이므로 숫자 변환
    dac_value = map(joyXIn, 0, 1023, 0, 4095);  // 조이스틱 입력은 10비트이며 출력은 12비트이므로 숫자 변환

    dac.begin(X_ADDR);                            // X 축 DAC 활성화
    dac.setVoltage(dac_value, false);             // X축 전압 출력
    
 //   dac_value = map(joyYIn, 0, 1023, 819, 3276);  // 조이스틱 입력은 10비트이며 출력은 12비트이므로 숫자 변환
    dac_value = map(joyYIn, 0, 1023, 0, 4095);  // 조이스틱 입력은 10비트이며 출력은 12비트이므로 숫자 변환

    dac.begin(Y_ADDR);                            // Y 축 DAC 활성화
    dac.setVoltage(dac_value, false);             // Y축 전압 출력
  }
  
   prevJoystickIn = joystickIn;
  if(joystickIn == false)
  {
    ret = myVR.recognize(buf, 50);
    
    if(ret > 0) {
      Serial.print("Voice : ");
      switch(buf[1]){
        case GO:
       
          moveGo();
          currentState = GO;
          Serial.println("GO");
          break;
        case BACK:
          moveBack();
          currentState = BACK;
          Serial.println("BACK");
          break;
        case LEFT:
          moveLeft();
          currentState = LEFT;
          Serial.println("LEFT");
          break;
        case RIGHT:
          moveRight();
          currentState = RIGHT;
          Serial.println("RIGHT");
          break;
        case STOP:
          stopWheelchair();
          currentState = STOP;
          Serial.println("STOP");
          break;
      }
    }
  }
  if(fallAlert == true) // 추락 위험이 있을 때
  {
   // stopWheelchair();
   // moveBack();
   // delay(1000);
   // stopWheelchair();
    Serial.println("Fall Alert"); 
  }
  if(obstacleAlert == true) // 장애물이 있을 때
  {
    if(currentState == GO)
    {
      stopWheelchair();
   //   delay(1000);
   //   moveGo();
      Serial.println("Obstacle Alert");
    }
  }
  
}
