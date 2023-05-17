#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Servo.h>
#include "Blynk_Config.h"

void readTempAndHumid();
void alertSound();
void phsen();
void turnOff();
void readWaterOne();
void readWaterTwo();
void displaySen();

#define DHTPIN 19
#define DHTTYPE DHT22
#define LDRPIN 4
#define WATER_SEN_1 32
#define WATER_SEN_2 33
#define BUZZER_PIN 23
#define REL_ACTUATOR 16
#define REL_WATER_MOTOR 17
#define REL_EXHAUST 18
#define REL_LIGHT 25
#define PH_SENSOR 35
#define SERVO_PIN 13

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo; 
BlynkTimer timer;

int relayPins[] = {REL_ACTUATOR, REL_WATER_MOTOR, REL_EXHAUST, REL_LIGHT};

unsigned int temp, humid;
int waterLevel1, waterLevel2 ;
float calibration_value = 21.34 - 0.2; // 20.24 - 0.7;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10];
int ph_act;

int buttonTimer;

void setup()
{
  Serial.begin(115200);
  delay(50);
  lcd.init();
  lcd.backlight();
  dht.begin();
  servo.attach(SERVO_PIN);
  Serial.println("...connecting");

  // Init wifi
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid1, pass1);

  delay(2500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Success");

  Serial.println("Success");

  pinMode(LDRPIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(WATER_SEN_1, INPUT_PULLUP);
  pinMode(WATER_SEN_2, INPUT_PULLUP);

  for (int i : relayPins){
    pinMode(i, OUTPUT);
    digitalWrite(i , LOW);
  }

  Serial.println("Welcome to IOT Poultry");
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("IOT");
  lcd.setCursor(6, 1);
  lcd.print("Poultry");

  timer.setInterval(1000L, readTempAndHumid);
  timer.setInterval(1000L, phsen);
  timer.setInterval(1000L, readWaterOne);
  timer.setInterval(1000L, readWaterTwo);
  timer.setInterval(1000L, displaySen);

}

void loop()
{
  Blynk.run();
  timer.run();

}


// Display all sensor values to the LCD screen

void displaySen(){


  delay(1500);

    if (waterLevel1 == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water 1: ");
    lcd.print("Present");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water 1: ");
    lcd.print("Not Present");
    alertSound();
  }

    delay(1500);

      if (waterLevel2 == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water 2: ");
    lcd.print("Present");
  } else {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Water 2: ");
    lcd.print("Not Present");
    alertSound();
  }

  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("pH Level: ");
  lcd.print(ph_act);

  delay(1500);
}

void readTempAndHumid()
{
  delayMicroseconds(50);
  temp = dht.readTemperature();
  humid = dht.readHumidity();

  if (isnan(humid) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humid);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humid: ");
  lcd.print(humid);
  lcd.print("%");



  Blynk.virtualWrite(VIR_TEMPERATURE, temp);
  Blynk.virtualWrite(VIR_HUMIDITY, humid);


}

void readWaterOne(){
  waterLevel1 = digitalRead(WATER_SEN_1);
  // For water pump
  Serial.print("Water 1: ");
  Serial.println(waterLevel1);


  Blynk.virtualWrite(VIR_WATERSEN1, waterLevel1);

}

void readWaterTwo(){
  waterLevel2 = digitalRead(WATER_SEN_2);
  // For water pump
  Serial.print("Water 2: ");
  Serial.println(waterLevel2);


  Blynk.virtualWrite(VIR_WATERSEN2, waterLevel2);

}

void phsen()
{
  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(PH_SENSOR);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 3.3 / 4096.0 / 6;
  // Serial.print("Voltage: ");
  // Serial.println(volt);
  ph_act = -5.70 * volt + calibration_value;

  Serial.print("pH Val: ");
  Serial.println(ph_act);



  Blynk.virtualWrite(VIR_PH, ph_act);

}

void setServos(int degrees) {
    for(int i = 0; i < 5; ++i) {
        servo.write((degrees + (35 * i)) % 180);
    }
}



void alertSound(){
  tone(BUZZER_PIN, 500); // play a 500Hz tone
  delay(2000);           // wait for 1 second
  noTone(BUZZER_PIN);    // stop the tone
  delay(2000);
  tone(BUZZER_PIN, 500); // play a 500Hz tone
  delay(1000);           // wait for another second
  noTone(BUZZER_PIN);    // stop the tone
}



BLYNK_WRITE(VIR_EXHAUST)
{
  if (param.asInt() == 1)
  {
    Serial.println("BLYNK: REL_EXHAUST turned on");
    digitalWrite(REL_EXHAUST, HIGH);

  }
  else
  {
    Serial.println("BLYNK: REL_EXHAUST turned off");
    digitalWrite(REL_EXHAUST, LOW);
  }
}

BLYNK_WRITE(VIR_SERVO){
  if(param.asInt() == 1){
    Serial.println("BLYNK: SERVO_MOTOR turned on");
      /*
      for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
        setServos(posDegrees);
        Serial.println(posDegrees);
        delay(20);
    }
    */
    servo.write(180);
  } else {
    Serial.println("BLYNK: SERVO_MOTOR turned off");
    servo.write(0);
      /*
      for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
        setServos(posDegrees);
        Serial.println(posDegrees);
        delay(20);
    }
      */
  }
}

/*
BLYNK_WRITE(VIR_ACTUATOR) {

  static int last_time = millis();
  int current_time = millis();

  if(current_time - last_time >= 1000){
    if (param.asInt() == 1) {
      Serial.println("BLYNK: REL_ACTUATOR turned on");
      digitalWrite(REL_ACTUATOR, HIGH);
      Serial.println("Code: 1");
    } else {
      Serial.println("BLYNK: REL_ACTUATOR turned off");
      digitalWrite(REL_ACTUATOR, LOW);
      Serial.println("Code: 2");
    }
    last_time = current_time;
  }

  if (param.asInt() == 1) {
    Serial.println("BLYNK: REL_ACTUATOR turned on");
    digitalWrite(REL_ACTUATOR, HIGH);
    Serial.println("Code: 3");
  } else {
    Serial.println("BLYNK: REL_ACTUATOR turned off");
    digitalWrite(REL_ACTUATOR, LOW);
    Serial.println("Code: 4s");
  }
}
*/

BLYNK_CONNECTED{
  
}

BLYNK_WRITE(VIR_ACTUATOR) {
  if(param.asInt() == 1){ 
    digitalWrite(REL_ACTUATOR, HIGH);
    // This will create a timer within 1.5 seconds. 
    // If 1.5 seconds elapse, the timer will be deleted to avoid causing an error exception
    buttonTimer = timer.setInterval(1500L, turnOff);
    Serial.print("Button Timer: ");
    Serial.println(buttonTimer);
    Blynk.virtualWrite(VIR_ACTUATOR, LOW);
  } else {
    digitalWrite(REL_ACTUATOR, LOW);
  }
}

void turnOff(){
  if(buttonTimer != -1){
      timer.deleteTimer(buttonTimer);
  }
  digitalWrite(REL_ACTUATOR, LOW);

}


BLYNK_WRITE(VIR_WATER_MOTOR)
{
  if (param.asInt() == 1)
  {
    if (waterLevel1 != 1)
    {
      Serial.println("BLYNK: Tank water level is low. Please fill up the tank.");
      // maglalagay ng buzzer dito and lcd screen msg
      alertSound();
    }
    else
    {
      Serial.println("BLYNK: REL_WATER_MOTOR turned on");
      digitalWrite(REL_WATER_MOTOR, HIGH);
    }
  }
  else
  {
    Serial.println("BLYNK: REL_WATER_MOTOR turned off");
    digitalWrite(REL_WATER_MOTOR, LOW);
  }
}

BLYNK_WRITE(VIR_LIGHT)
{
  if (param.asInt() == 1)
  {
    Serial.println("BLYNK: Incandescent Light has turned on");
    digitalWrite(REL_LIGHT, HIGH);
  }
  else
  {
    Serial.println("BLYNK: Incandescent Light has turned off");
    digitalWrite(REL_LIGHT, LOW);
  }
}