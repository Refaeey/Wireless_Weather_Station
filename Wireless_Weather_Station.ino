#include "DHT.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>


#define BLYNK_TEMPLATE_ID "TMPL24dQOZU3H"
#define BLYNK_TEMPLATE_NAME "Weather Station"
#define BLYNK_AUTH_TOKEN "_TqgSYTxpT0KawsS_0STPKzDZpI9Bg0X"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h> 

char ssid[] = "Refaeey";
char pass[] = "0123456789";

#define DHTPIN 2   
#define AO_PIN 36 
#define DHTTYPE DHT11  
int vane_diameter = 150; // Anamometer vane diameter (set to the value for your cup-to-cup in mm)         <----------------------------------------
int vane_circ;     // Calculate vane circumference in metres
int EfficiencyFactor = 2.5 ;    //Set an anamometer factor to account for inefficiency (value is a guess) <----------------------------------------
int Windspeed = 0;
int RPM;
float TriggerCounter;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

void IRAM_ATTR sens() {
  TriggerCounter++;
  }

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  attachInterrupt(digitalPinToInterrupt(12 ), sens, RISING);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to Wifi...");
  
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  lcd.init();                    
  lcd.backlight();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);
  // Reading humidity
  float Humidity = dht.readHumidity();
  // Reading temperature
  float Temperature = dht.readTemperature();
  // Read gasValue
  int gasValue = analogRead(AO_PIN);


  // Check if any reads failed and exit early (to try again).
  if (isnan(Humidity) || isnan(Temperature) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(Humidity);
  Serial.println(F("%"));

  Serial.print(F("Temperature: "));
  Serial.print(Temperature);
  Serial.println(F(" Celsius"));

  Serial.print(F("Gas methane value: "));
  Serial.println(gasValue);

  Anemometer_Calc();

  Blynk.virtualWrite(V1,Humidity);
  Blynk.virtualWrite(V2,Temperature);
  Blynk.virtualWrite(V0,gasValue);
  Blynk.virtualWrite(V4,Windspeed);



  lcd.setCursor(0,0);
  lcd.print("Temp");
  lcd.setCursor(5,0);
  lcd.print("Hum");
  lcd.setCursor(9,0);
  lcd.print("GL");
  lcd.setCursor(13,0);
  lcd.print("WSp");

  lcd.setCursor(5,1);
  lcd.print((float) Humidity);
  lcd.print(" %"); 

   lcd.setCursor(0,1);
  lcd.print((float) Temperature);
  lcd.print(" C"); 

  lcd.setCursor(8,1);
  lcd.print((int) gasValue);
  lcd.print(" ppm");

  lcd.setCursor(13,1);
  lcd.print((float) Windspeed);
  lcd.print(" m/s");
}

void Anemometer_Calc(){
  int   wings= 20;  // no of wings of rotating object, for disc object use 1 with white tape on one side
  float TriggerNew = TriggerCounter/wings;  //here we used fan which has 3 wings
  float vane_circ = 0.09*3.1415 ;
  float RPM = TriggerNew * 60;
  float Windspeed ;
  Windspeed = RPM*vane_circ*0.1 ;
  Serial.print(Windspeed);
  Serial.println("m/s. ");
  

  Blynk.virtualWrite(V4, Windspeed);
  TriggerCounter=0;
  
  }

