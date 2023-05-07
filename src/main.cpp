#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

void readTempAndHumid();
void searchWifi();
void startWifi();

#define DHTPIN 19
#define DHTTYPE DHT22
#define LDRPIN 4
#define BUZZPIN 0
#define PH_PIN 14
#define WATER_SEN 32


#define REL_FOOD 12
#define REL_WATER 14
#define REL_FAN 27

#define PH_SENSOR 33

#define BLYNK_TEMPLATE_ID           "TMPL65DgpX-C_"
#define BLYNK_TEMPLATE_NAME         "PoultryIOT"
#define BLYNK_AUTH_TOKEN            "h5HVLffKAiYupLjLWi_tZ08WQhrVz6yN"


DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

/*

  Kung iibahin yung wifi
  ssid = yung name ng wifi
  pass = password nung wifi

*/
const char* ssid = "Maria";
const char* pass = "Zenaida102763";

float temp, humid;
int waterLevel; 
boolean openDoorFood; 

void setup() {
  Serial.begin(9600);
  pinMode(LDRPIN, INPUT);
  pinMode(BUZZPIN, OUTPUT);
  pinMode(REL_FOOD, OUTPUT);
  pinMode(REL_WATER, OUTPUT);
  pinMode(REL_FAN, OUTPUT);
  pinMode(WATER_SEN, INPUT_PULLUP);


  // HIGH yung off and LOW naman yung on
  digitalWrite(REL_FAN, HIGH);
  digitalWrite(REL_FOOD, HIGH);
  digitalWrite(REL_WATER, HIGH);

  lcd.begin();
  lcd.backlight();
  dht.begin();

  // Init wifi
  //startWifi();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Init blynk");
  delay(1000);

  timer.setInterval(2000L, readTempAndHumid);

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Tangina");
  lcd.setCursor(6, 1);
  lcd.print("nyo");

}

void loop() {
  Blynk.run();
  timer.run();
  lcd.clear();

  /*
  if(temp >= 35){
    digitalWrite(REL_FAN, LOW);
  }else {
    digitalWrite(REL_FAN, HIGH);
  }

  int ldrValue = digitalRead(LDRPIN);

  if(ldrValue == 1){
    // Lalagay ko dito yung led light
    Serial.print("");
  } else {
    // pagiisipan pa. 
  }

  int phVoltage = analogRead(33);
  Serial.print("PH LEVEL VOLTAGE: ");
  Serial.println(phVoltage);


  // Kapag 0 output walang laman, 1 may laman
  Serial.print("Water Level: ");
  Serial.println(digitalRead(WATER_SEN));

  waterLevel = digitalRead(WATER_SEN);

  if (waterLevel == 0){
    // buzzer
    


    for(int i = 0; i < 32; i++){
    delay(500);
    lcd.setCursor(0, i);
    lcd.print("Warning: Low water level detected.");
    }
  } else {
    Serial.println("Water pump is moving");
  }
*/
}

void readTempAndHumid(){
  
  temp = dht.readTemperature();
  humid = dht.readHumidity();


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print("Humid: ");
  lcd.print(humid);

  Blynk.virtualWrite(V6, temp);
  Blynk.virtualWrite(V1, humid);

  Serial.print("Temperature: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humid);

  delay(500);
}

void readPHLevel() {

}

void openFood(){
  if(openDoorFood == true){
    
  }
}

void startWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void searchWifi(){
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
      Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);
}

