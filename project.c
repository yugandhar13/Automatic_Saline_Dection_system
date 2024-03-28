#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define buzzer A3

#include "DHT.h"
#define DHTTYPE DHT11
#define dht_dpin 4
DHT dht(dht_dpin, DHTTYPE);

#include "HX711.h"
#define calibration_factor -7050.0  //This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN 6
#define LOADCELL_SCK_PIN 5
HX711 scale;

namespace
{
const char* USER_AGENT = "Saline_monitoring";                    // Assgin the user agent
const char* VERSION = "1.0";                                // Assign the version
const char* METHOD = "POST";                                // Set the method
const char* TOKEN = "BBUS-q3CZdvNnix0PmEaEIFJgBXn57hndMI";  // Assign your Ubidots TOKEN
const char* DEVICE_LABEL = "Saline_monitoring";                  // Assign the device label
const char* VARIABLE_LABEL1 = "Weight";                 // Assign the variable label
const char* VARIABLE_LABEL2 = "Temperature";                 // Assign the variable label
const char* VARIABLE_LABEL3 = "Humidity";                 // Assign the variable label
}
char str_sensor1[10];
char str_sensor2[10];
char str_sensor3[10];

float present = 0, previous = 0;
int weight = 0, j = 1;
void setup()
{
  Serial.begin(115200);
  dht.begin();
  lcd.begin(16, 2); // initialize the lcd
  lcd.setCursor(0, 0);
  lcd.print("    Saline       ");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring System");
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(1000);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);  //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();
  lcd.clear(); // initialize the lcd
  lcd.setCursor(0, 0);
  lcd.print("Tare Calibration");
  lcd.setCursor(0, 1);
  lcd.print("Completed.");
  //Assuming there is no weight on the scale at start up, reset the scale to 0
  delay(4000);
  lcd.clear(); // initialize the lcd
  lcd.setCursor(0, 0);
  lcd.print("Pls Hang saline");
  lcd.setCursor(0, 1);
  lcd.print("bottle to loadcell");
  delay(5000);
}

void loop()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Measuring sensors");
  delay(500);
  for (int i = 0; i < 20; i++)
  {
    present = scale.get_units();
    previous = present + previous;
  }
  previous = previous / 100;
  previous = previous / 14.48;
  previous = previous * 1000;
  weight = previous;
  previous = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weight: ");
  lcd.print(weight);
  lcd.print(" g");
  delay(1000);
  if (weight <= 30)
  {
    digitalWrite(buzzer, HIGH);
  }
  else
  {
    digitalWrite(buzzer, LOW);
  }
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.setCursor(0, 1);
  lcd.print(t);
  lcd.print(" *C");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity:");
  lcd.setCursor(0, 1);
  lcd.print(h);
  lcd.print(" %");
  delay(1000);

  //***********************************************************************
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sending data IOT");
  char* command1 = (char*)malloc(sizeof(char) * 128);
  float sensor1 = weight;
  dtostrf(sensor1, 4, 2, str_sensor1);
  sprintf(command1, "init#");
  sprintf(command1, "%s%s/%s|%s|%s|", command1, USER_AGENT, VERSION, METHOD, TOKEN);
  sprintf(command1, "%s%s=>", command1, DEVICE_LABEL);
  sprintf(command1, "%s%s:%s", command1, VARIABLE_LABEL1, str_sensor1);
  sprintf(command1, "%s|end#final", command1);
  Serial.print(command1);
  free(command1);
  delay(2000);

  char* command2 = (char*)malloc(sizeof(char) * 128);
  float sensor2 = t;
  dtostrf(sensor2, 4, 2, str_sensor2);
  sprintf(command2, "init#");
  sprintf(command2, "%s%s/%s|%s|%s|", command2, USER_AGENT, VERSION, METHOD, TOKEN);
  sprintf(command2, "%s%s=>", command2, DEVICE_LABEL);
  sprintf(command2, "%s%s:%s", command2, VARIABLE_LABEL2, str_sensor2);
  sprintf(command2, "%s|end#final", command2);
  Serial.print(command2);
  free(command2);
  delay(2000);

  char* command3 = (char*)malloc(sizeof(char) * 128);
  float sensor3 = h;
  dtostrf(sensor3, 4, 2, str_sensor3);
  sprintf(command3, "init#");
  sprintf(command3, "%s%s/%s|%s|%s|", command3, USER_AGENT, VERSION, METHOD, TOKEN);
  sprintf(command3, "%s%s=>", command3, DEVICE_LABEL);
  sprintf(command3, "%s%s:%s", command3, VARIABLE_LABEL3, str_sensor3);
  sprintf(command3, "%s|end#final", command3);
  Serial.print(command3);
  free(command3);
  delay(2000);
  //*******************************************************************************
  lcd.setCursor(0, 0);
  lcd.print("Data Sent IOT");
  delay(500);
}
