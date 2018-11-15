#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
SoftwareSerial myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.

void setup() {
  // put your setup code here, to run once:
  myVR.begin(9600);
  Serial.begin(115200);
  delay(2000);
  Serial.println("Setting");
  myVR.write(0xAA);     // AA
  Serial.println("0xAA ");
  myVR.write(0x04 + 4);  // record 갯수
   Serial.println("0x08");
  myVR.write(0x15);     // 15
   Serial.println("0x15");
  myVR.write(0x1F);     // 5개 레코드 BITmap
   Serial.println("0x1F");
  myVR.print('\0');     // record 0 ~ 4
   Serial.println("0x00");
  myVR.write(0x01);
   Serial.println("0x01");
  myVR.write(0x02);
   Serial.println("0x02");
  myVR.write(0x03);
   Serial.println("0x03");
  myVR.write(0x04);
   Serial.println("0x04");
  myVR.write(0x0A);     // 종료
   Serial.println("0x0A");
}

void loop() {
  // put your main code here, to run repeatedly:
  char input;
  if(myVR.available())
  {
    input = myVR.read();
    Serial.print(input);
    Serial.print(' ');
  }

}
