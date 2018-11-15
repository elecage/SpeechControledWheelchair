int x = 0;
int y = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  x = analogRead(A0);
  Serial.print("X : ");
  Serial.print(x);
  Serial.print("\tY : ");
  y = analogRead(A1);
  Serial.println(y);
  delay(100);
}
