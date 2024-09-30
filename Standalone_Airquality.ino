#include<LiquidCrystal_I2C.h>
#include <Wire.h> 

LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  // put your setup code here, to run once:
pinMode(A0,INPUT);
lcd.init();
lcd.backlight();
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int mq_value=analogRead(A0);
  Serial.print(mq_value);
  lcd.setCursor(0,0);
  lcd.print("Air quality: ");
  lcd.print(mq_value);
}