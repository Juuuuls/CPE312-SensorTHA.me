#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>
#include <MQ135.h>

// Initialize the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define DHT11 sensor parameters
#define DHTPIN 7     // Pin connected to the DHT11 sensor
#define DHTTYPE DHT11   // DHT 11 sensor type
DHT dht(DHTPIN, DHTTYPE); // Initialize the DHT sensor

const long duration = 600000; // 10 mins in milliseconds
float m = 0; // gets the total value of air quality we recorded
float h = 0; // gets the total value of humidity we recorded
float t = 0; // gets the total value of temperature we recorded
float m_ave = 0;
float h_ave = 0;
float t_ave = 0;
int n_count = 0; // number of records we recorded in our sensor

void setup() {
  // Initialize LCD and DHT11
  lcd.init();
  lcd.backlight();
  dht.begin();
  // Start serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read air quality value from MQ sensor
  MQ135 gasSensor = MQ135(A0);
  m += gasSensor.getRZero();
  
  // Read temperature and humidity from DHT11
  h += dht.readHumidity();
  t += dht.readTemperature();
  
  if (millis() == duration ){
    Serial.println("the recording has gone 10 minutes");
    Serial.println("Getting the average");

    m_ave = (m/n_count);
    h_ave = (h/n_count);
    t_ave = (t/n_count);

    Serial.println("Average air quality");
    Serial.println(m_ave);
    Serial.println("Average humidity");
    Serial.prinln(h_ave);
    Serial.println("Average Temperature");
    Serial.println(t_ave);

    // put the code of esp 8266 here


    // reset the values for the next recording of the average
    m = 0;
    h = 0;
    t = 0;
    n_count = 0;

    // extends the duration for another 10 minutes
    duration += 600000;
    
  }
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
  n_count++;
  
  delay(6000); // Delay of 1 minute second before the next reading

}
