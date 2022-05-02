#include <GParser.h>
#include <parseUtils.h>
#include <GyverEncoder.h>
#include <GyverNTC.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

int temp = 0;

unsigned long last_time;
unsigned long last_time_lcd;

byte OPEN[] = {B01110,B10001,B10001,B10000,B11111,B11011,B11011,B01110};
byte CLOSE[] = {B00000,B01110,B10001,B10001,B11111,B11011,B11011,B01110};
byte START[] = {B00000,B00000,B10101,B01110,B11011,B01110,B10101,B00000};
byte STOP[] = {B00000,B00000,B11111,B10001,B10101,B10001,B11111,B00000};

//======ПИНЫ======
#define MP1 11
#define MP2 10
#define MP3 9
#define MP4 6

#define TP1 A0
#define TP2 A1
#define TP3 A2
#define TP4 A3

#define CLC 4
#define DT 2
#define SW 3
//======ПИНЫ======

Encoder enc(DT, CLC, SW);

GyverNTC NTC1(TP1, 10000, 3950);
GyverNTC NTC2(TP2, 10000, 3950);
GyverNTC NTC3(TP3, 10000, 3950);
GyverNTC NTC4(TP4, 10000, 3950);

bool MO1 = false;
bool MO2 = false;
bool MO3 = false;
bool MO4 = false;

int planka = 30;

int T1 = 0;
int T2 = 0;
int T3 = 0;
int T4 = 0;

bool ET = false;
bool OM = false;

void setup() {
  pinMode(TP1, INPUT);
  pinMode(TP2, INPUT);
  pinMode(TP3, INPUT);
  pinMode(TP4, INPUT);
  
  pinMode(MP1, OUTPUT);
  pinMode(MP2, OUTPUT);
  pinMode(MP3, OUTPUT);
  pinMode(MP4, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.createChar(1,OPEN);
  lcd.createChar(2,CLOSE);
  lcd.createChar(3,START);
  lcd.createChar(4,STOP);

  enc.setDirection(true);
}

void loop() {
  if (millis() - last_time > 500) {
      last_time = millis();
      TI();
      TC();
      OPENM();
    }
  if (millis() - last_time_lcd > 100) {
      last_time_lcd = millis();
      LCDT();
    }
  TENC();
}

void TI() {
    T1 = NTC1.getTempAverage();
    T2 = NTC2.getTempAverage();
    T3 = NTC3.getTempAverage();
    T4 = NTC4.getTempAverage();
}

void TC() {
  if (OM){
    if (T1 >= planka) { MO1 = false; } else { MO1 = true; }
    if (T2 >= planka) { MO2 = false; } else { MO2 = true; }
    if (T3 >= planka) { MO3 = false; } else { MO3 = true; }
    if (T4 >= planka) { MO4 = false; } else { MO4 = true; }
  }
  else {
     MO1 = false;
     MO2 = false;
     MO3 = false;
     MO4 = false;
    }
}

void LCDT() {
    lcd.setCursor(0,0);
    String T;
    lcd.print("T = ");
    if(T1 > -50){T = T1;}else{T = "NO";}
    lcd.print(T);lcd.print("|");
    if(T2 > -50){T = T2;}else{T = "NO";} 
    lcd.print(T);lcd.print("|");
    if(T3 > -50){T = T3;}else{T = "NO";} 
    lcd.print(T);lcd.print("|");
    if(T4 > -50){T = T4;}else{T = "NO";} 
    lcd.print(T);lcd.print("|    ");

    lcd.setCursor(0,1);
    lcd.print("P = ");lcd.print(planka);
    lcd.print(" "); if (ET) { lcd.write(1); } else { lcd.write(2); }
    lcd.print(" "); if (OM) { lcd.write(3); } else { lcd.write(4); }
}

void OPENM() {
    if (MO1) { digitalWrite(MP1, HIGH); } else { digitalWrite(MP1, LOW); }
    if (MO2) { digitalWrite(MP2, HIGH); } else { digitalWrite(MP2, LOW); }
    if (MO3) { digitalWrite(MP3, HIGH); } else { digitalWrite(MP3, LOW); }
    if (MO4) { digitalWrite(MP4, HIGH); } else { digitalWrite(MP4, LOW); }
    }
void TENC() {
    enc.tick();
     if (enc.isRight() && ET && planka +1 <= 120){ planka += 1; }
     if (enc.isLeft() && ET && planka -1 >= 20){ planka -= 1; }
     if (enc.isClick()){ if (ET) { ET = false; } else { ET = true; }  }
     if (enc.isDouble()){ if (OM) { OM = false; } else { OM = true; }  }
    }
