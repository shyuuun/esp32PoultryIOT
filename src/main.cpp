#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include "Blynk_Commands.h"

void readTempAndHumid();
void readWaterLevel();
void alertSound();

#define DHTPIN 19
#define DHTTYPE DHT22
#define LDRPIN 4
#define BUZZPIN 0
#define PH_PIN 14
#define WATER_SEN 32
#define WATER_BUZZ 23
#define REL_ACTUATOR 26
#define REL_WATER_MOTOR 14
#define REL_EXHAUST 27
#define REL_LIGHT 25

#define PH_SENSOR 33

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

float temp, humid;
int waterLevel;

void setup()
{
  Serial.begin(9600);
  pinMode(LDRPIN, INPUT);
  pinMode(BUZZPIN, OUTPUT);
  pinMode(REL_ACTUATOR, OUTPUT);
  pinMode(REL_WATER_MOTOR, OUTPUT);
  pinMode(REL_EXHAUST, OUTPUT);
  pinMode(REL_LIGHT, OUTPUT);
  pinMode(WATER_BUZZ, OUTPUT);
  pinMode(WATER_SEN, INPUT_PULLUP);
  // Init wifi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  lcd.init();
  lcd.backlight();
  dht.begin();

  Serial.println("Welcome to IOT Poultry");

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("IOT");
  lcd.setCursor(6, 1);
  lcd.print("Poultry");

  delay(2500);

  // LOW yung off and HIGH naman yung on
  digitalWrite(REL_EXHAUST, LOW);
  digitalWrite(REL_ACTUATOR, LOW);
  digitalWrite(REL_WATER_MOTOR, LOW);
  digitalWrite(REL_LIGHT, LOW);

  timer.setInterval(2000L, readTempAndHumid);
  timer.setInterval(2000L, readWaterLevel);


}

void loop()
{
  Blynk.run();
  timer.run();

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

void readTempAndHumid()
{

  temp = dht.readTemperature();
  humid = dht.readHumidity();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print("Humid: ");
  lcd.print(humid);

  Blynk.virtualWrite(VIR_TEMPERATURE, temp);
  Blynk.virtualWrite(VIR_HUMIDITY, humid);

  Serial.print("Temperature: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humid);


  delay(500);
}

void readWaterLevel(){
  waterLevel = digitalRead(WATER_SEN);
  if(waterLevel == 1){
    Serial.println("Tank currently has water.");
    Blynk.virtualWrite(VIR_WATERSEN, "Present");
  } else {
    Serial.println("Tank water level is low. Please fill up the tank.");
    Blynk.virtualWrite(VIR_WATERSEN, "Not Present");

    alertSound();

    Blynk.virtualWrite(VIR_WATER_MOTOR, LOW);
  }
  delay(500);
}

void alertSound() {
    tone(WATER_BUZZ, 500); // play a 500Hz tone
    delay(2000); // wait for 1 second
    noTone(WATER_BUZZ); // stop the tone
    delay(2000);
    tone(WATER_BUZZ, 500); // play a 500Hz tone
    delay(1000); // wait for another second
    noTone(WATER_BUZZ); // stop the tone
}

BLYNK_WRITE(VIR_ACTUATOR)
{
  if (param.asInt() == 1)
  {
    Serial.println("BLYNK: REL_ACTUATOR turned on");
    digitalWrite(REL_ACTUATOR, HIGH);
    delay(5000);
    Serial.println("BLYNK: REL_ACTUATOR turned off");
    digitalWrite(REL_ACTUATOR, OFF);
  }

}

BLYNK_WRITE(VIR_EXHAUST){
  if (param.asInt() == 1){
    Serial.println("BLYNK: REL_EXHAUST turned on");
    digitalWrite(REL_EXHAUST, HIGH);
  } else {
    Serial.println("BLYNK: REL_EXHAUST turned off");
    digitalWrite(REL_EXHAUST, LOW);
  }
}

BLYNK_WRITE(VIR_WATER_MOTOR){
  if (param.asInt() == 1){
    if(waterLevel != 1){
      Serial.println("BLYNK: Tank water level is low. Please fill up the tank.");
      // maglalagay ng buzzer dito and lcd screen msg
    } else {
      Serial.println("BLYNK: REL_WATER_MOTOR turned on");
      digitalWrite(REL_WATER_MOTOR, HIGH);
    }
  } else {
    Serial.println("BLYNK: REL_WATER_MOTOR turned off");
    digitalWrite(REL_WATER_MOTOR, LOW);
  }
}

BLYNK_WRITE(VIR_LIGHT){
  if(param.asInt() == 1){
    Serial.println("BLYNK: Incandescent Light has turned on");
    digitalWrite(REL_LIGHT, HIGH);
  } else {
    Serial.println("BLYNK: Incandescent Light has turned off");
    digitalWrite(REL_LIGHT, LOW);
  }
}