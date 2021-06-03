#include "DHT.h"
#include <LiquidCrystal.h>

 
#define DHTPIN A2 // pino que estamos conectando
#define DHTTYPE DHT11 // DHT 11

// Declaration and initialization of the input pins
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal lcd(7, 8, 10, 11, 12, 13);

int fan_tachometer_pin = 2;
int fan_vcc_pin = 5;


volatile uint32_t rev; // Revolution Count
unsigned long measureTime = 0;


int vcc;
int BASE_VCC = 210;
float MAX_VOLTAGE = 5.0;

int t;
int diff;
int IDEAL_TEMP = 26;

void setup() { 
  pinMode(fan_vcc_pin, OUTPUT); 
  pinMode(fan_tachometer_pin, INPUT); 

  attachInterrupt(0, addRevolution, FALLING);

  dht.begin();
  lcd.begin(16, 2);
}

void loop(){

  int t, h;
  read_dht(dht, &t, &h);
  diff = t - IDEAL_TEMP;

  // FAN

  vcc = BASE_VCC + 10*diff;
  if(vcc > 255) {
    vcc = 255;
  }
  
  analogWrite(fan_vcc_pin, vcc);

  // TACHOMETER

  noInterrupts();
  uint32_t rpm = rev * 60000 / (millis() - measureTime);
  rev = 0;
  measureTime = millis();
  interrupts();

  // LCD

  char temp_str[2]; 
  itoa(t, temp_str, 10);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  
  lcd.print("Temp:");
  lcd.print(temp_str);
  lcd.print("C");

  char hum_str[2]; 
  itoa(h, hum_str, 10);

  lcd.print(" Umi:");
  lcd.print(hum_str);
  lcd.print("%");

  
  lcd.setCursor(0, 1);

  float voltage = (vcc / 255.0) * MAX_VOLTAGE;
  
  lcd.print("  ");
  lcd.print(String(voltage));
  lcd.print("V");

  char tach_str[3]; 
  itoa(rpm, tach_str, 10);

  lcd.print(" ");
  lcd.print(tach_str);
  lcd.print("rpm");
  
  delay(1000);
}


/////////////////////////////////////////////////////////////////////////////////////

 
void read_dht(DHT dht, int* t_ptr, int* h_ptr){
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  *t_ptr = (int) t;
  *h_ptr = (int) h;
}


///////////////////////////////////////////////////////////////////////////////////


void addRevolution() {
  rev++;
}
