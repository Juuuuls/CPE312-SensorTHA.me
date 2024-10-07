#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"
#include <DHT.h>
#include <MQ135.h>

#define DHTPIN D2                                                           // Connect to Pin D2 in ESP8266
#define DHTTYPE DHT11                                                       // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE);

String sheetHumid = "";
String sheetTemp = "";
String sheetairquality="";

int air_quality = A0;
// variables for getting the average per 10 mins
const long duration = 60000; // 10 mins in milliseconds
unsigned long previousMillis = 0;
float m = 0; // gets the total value of air quality we recorded
float h = 0; // gets the total value of humidity we recorded
float t = 0; // gets the total value of temperature we recorded
float m_total = 0;
float h_total = 0;
float t_total = 0;
float m_ave = 0;
float h_ave = 0;
float t_ave = 0;
int n_count = 0; // number of records we recorded in our sensor

const char* ssid = "legen...";                //replace with our wifi ssid // CONVERGE_MGM // Juls // to change
const char* password = "waitforitdary";         //replace with your wifi password // magnait_3695926 // 00000000 // to change

const char* host = "script.google.com";
const char *GScriptId = "AKfycbwrkzplIaeLqSsUQjJxow04qrEFGHUPuQ-eLPT7S53B6feyph0_U10I8C-rxE8igacb"; // Replace with your own google script id // to change
const int httpsPort = 443; //the https port is same

// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
const char* fingerprint = "";

//const uint8_t fingerprint[20] = {};

String url = String("/macros/s/") + GScriptId + "/exec?value=Temperature";  // Write Teperature to Google Spreadsheet at cell A1
// Fetch Google Calendar events for 1 week ahead
String url2 = String("/macros/s/") + GScriptId + "/exec?cal";  // Write to Cell A continuosly

//replace with sheet name not with spreadsheet file name taken from google
String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"air_quality_data_sheet\", \
                       \"values\": "; // to change
String payload = "";

HTTPSRedirect* client = nullptr;

// used to store the values of free stack and heap before the HTTPSRedirect object is instantiated
// so that they can be written to Google sheets upon instantiation

void setup() {
  delay(1000);
  Serial.begin(115200);
  dht.begin();     //initialise DHT11

  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("==================================");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  Serial.print("Connecting to ");
  Serial.println(host);          //try to connect with "script.google.com"

  // Try to connect for a maximum of 5 times then exit
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
// Finish setup() function in 1s since it will fire watchdog timer and will reset the chip.
//So avoid too many requests in setup()

  Serial.println("\nWrite into cell 'A1'");
  Serial.println("------>");
  // fetch spreadsheet data
  client->GET(url, host);
  
  Serial.println("\nGET: Fetch Google Calendar Data:");
  Serial.println("------>");
  // fetch spreadsheet data
  client->GET(url2, host);

 Serial.println("\nStart Sending Sensor Data to Google Spreadsheet");

  
  // delete HTTPSRedirect object
  delete client;
  client = nullptr;
}

void loop() {
  unsigned long currentMillis = millis();
  MQ135 gasSensor = MQ135(air_quality);
  m = gasSensor.getRZero();
  h = dht.readHumidity();                                              // Reading temperature or humidity takes about 250 milliseconds!
  t = dht.readTemperature();
  m_total += m;
  h_total += h;
  t_total += t;
  // Read temperature as Celsius (the default)
  if (isnan(h) || isnan(t) || isnan(m)) {                                                // Check if any reads failed and exit early (to try again).
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
 
  Serial.print("Humidity: ");  Serial.print(h);
  sheetHumid = String(h) + String(" %");                                         //convert integer humidity to string humidity
  Serial.print("%  Temperature: ");  Serial.print(t);  Serial.println("°C ");
  sheetTemp = String(t) + String(" °C");
  Serial.print("Air Quality: "); Serial.println(m);
  sheetairquality = String(m) + String(" PPM");
  
  n_count++; // gets the number of recordings we will need it when getting the average
  Serial.println(currentMillis);

  delay(60000);
  static int error_count = 0;
  static int connect_count = 0;
  const unsigned int MAX_CONNECT = 20;
  static bool flag = false;
  if (currentMillis-previousMillis >= duration){
    Serial.println("the recording has gone 10 minutes");
    Serial.println("the recording has gone 10 minutes");
    Serial.println("Getting the average");

    m_ave = (m_total/n_count);
    h_ave = (h_total/n_count);
    t_ave = (t_total/n_count);

    Serial.println("Average air quality");
    Serial.println(m_ave);
    Serial.println("Average humidity");
    Serial.println(h_ave);
    Serial.println("Average Temperature");
    Serial.println(t_ave);

    payload = payload_base + "\"" + t_ave + "," + h_ave + ","   + m_ave + "\"}";

    if (!flag) {
      client = new HTTPSRedirect(httpsPort);
      client->setInsecure();
      flag = true;
      client->setPrintResponseBody(true);
      client->setContentTypeHeader("application/json");
    }

    if (client != nullptr) {
      if (!client->connected()) {
        client->connect(host, httpsPort);
        client->POST(url2, host, payload, false);
        Serial.print("Sent : ");  Serial.println("Temp, Humid and Air Quality");
      }
    }
    else {
      DPRINTLN("Error creating client object!");
      error_count = 5;
    }

    if (connect_count > MAX_CONNECT) {
      connect_count = 0;
      flag = false;
      delete client;
      return;
    }

    //  Serial.println("GET Data from cell 'A1':");
    //  if (client->GET(url3, host)) {
    //    ++connect_count;
    //  }
    //  else {
    //    ++error_count;
    //    DPRINT("Error-count while connecting: ");
    //    DPRINTLN(error_count);
    //  }

    Serial.println("POST or SEND Sensor data to Google Spreadsheet:");
    if (client->POST(url2, host, payload)) {
      ;
    }
    else {
      ++error_count;
      DPRINT("Error-count while connecting: ");
      DPRINTLN(error_count);
    }

    if (error_count > 3) {
      Serial.println("Halting processor...");
      delete client;
      client = nullptr;
      Serial.printf("Final free heap: %u\n", ESP.getFreeHeap());
      Serial.printf("Final stack: %u\n", ESP.getFreeContStack());
      Serial.flush();
      ESP.deepSleep(0);
    }
    delay(3000);    // keep delay of minimum 2 seconds as dht allow reading after 2 seconds interval and also for google sheet

     // reset the values for the next recording of the average
    m_total = 0;
    h_total = 0;
    t_total = 0;
    n_count = 0;
    // extends the duration for the next reading
  } // records the device per minute 
}