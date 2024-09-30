#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>

// Initialize the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define DHT11 sensor parameters
#define DHTPIN 7     // Pin connected to the DHT11 sensor
#define DHTTYPE DHT11   // DHT 11 sensor type
DHT dht(DHTPIN, DHTTYPE); // Initialize the DHT sensor

void setup() {
  // Initialize pin for MQ sensor (air quality)
  pinMode(A0, INPUT);
  
  // Initialize LCD and DHT11
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  // Start serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read air quality value from MQ sensor
  int m = analogRead(A0);
  
  // Read temperature and humidity from DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Display air quality on the first line of the LCD
  lcd.setCursor(0, 0);            // Set cursor to the first line
  lcd.print("Air quality: ");
  lcd.print(m);             // Print air quality value
  
  // Display temperature and humidity on the second line of the LCD
  lcd.setCursor(0, 1);            // Set cursor to the second line
  lcd.print("T:");                // Display temperature label
  lcd.print(t);                   // Display temperature value
  lcd.print("C H:");              // Display Celsius and humidity label
  lcd.print(h);                   // Display humidity value
  lcd.print("%");

  // Print to the Serial Monitor for debugging
  Serial.print("Air quality: ");
  Serial.print(m);
  Serial.print(" | Humidity: ");
  Serial.print(h);
  Serial.print(" % | Temp: ");
  Serial.print(t);
  Serial.println(" °C");

  delay(1000); // Delay of 1 second before the next reading
}
