// LCD
#include <LiquidCrystal.h>
//Realtimes
#include <virtuabotixRTC.h>
// Connect NodeMCU
#include <SoftwareSerial.h>

#include <Keypad.h>
#include <Wire.h>
#include <Password.h>  
#include <EEPROM.h>

//LCD
const int rs = 35, en = 34, d4 = 33, d5 = 32, d6 = 31, d7 = 30;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Realtimes
const int clk = 5, dat = 6, rst = 7;
virtuabotixRTC myRTC(clk, dat, rst); 

//Relay
#define RELAY_1 38    //Free
#define RELAY_2 39    //Free
#define RELAY_3 40    //Free
#define RELAY_4 41    //Water solenoid valve
// Relay Status
String Status_Relay_1, Status_Relay_2, Status_Relay_3, Status_Relay_4;

// Keypad 4x4
char keypressed;
String keyhistory;
const byte ROWS = 4;
const byte COLS = 4;
char key;
char keys[ROWS][COLS] = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
byte rowPins[ROWS] = {46, 47, 48, 49};
byte colPins[COLS] = {50, 51, 52, 53};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


// Timer
int Ran_relay_1, Run_Fan, Run_Mist, Run_Water; 


// Globle Setting
boolean OFF =  HIGH;
boolean ON = LOW;
int MODE = 2;
unsigned long show_time;

void setup() {
  Serial.begin(9600);
  Set_Time();
  Set_LCD();
  Set_Relay();
//  Set_Control();

}

void loop() {
  Control();
  
  if (keyhistory == ""){
    Show_Value();
  }

  //Get pressed key
  keypressed = keypad.getKey();
  if(keypressed != NO_KEY){
    keyhistory += keypressed;
  }

  // Clear Key and Enter Normal Operation
  if (keypressed == '0'){
    keyhistory = "";
    keypressed = NO_KEY;
  }

  // Enter Manual Control
  if (keyhistory == "A"){ 
    Control_Manual();
  }

  // Main MENU
  if (keypressed == '*'){  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("     Setup");
    lcd.setCursor(0,1);
    lcd.print("5: DateTime     ");
    delay(200);
  } else
  
  //Menu Setup DateTime ------------------------
  if (keyhistory == "*5"){
    Setup_DateTime();
  }

}

// SET FUNCTIONS....................................................................
void Set_Time() {
 //Set the current date, and time in the following format:
 //seconds, minutes, hours, day of the week, day of the month, month, year
//  myRTC.setDS1302Time(15, 22, 21, 7, 14, 1, 2018);
 //but remember to "comment/remove" this function once you're done
 //The setup is done only one time and the module will continue counting it automatically
}

void Set_LCD(){
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("     WELCOME    ");
  lcd.setCursor(0,1);
  lcd.print("   STRAWBERRY   ");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("System Boot ");
  delay(200);
  lcd.print(".");
  delay(600);
  lcd.print(".");
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.clear();
}

void Set_Relay(){
  pinMode(RELAY_1,OUTPUT);
  pinMode(RELAY_2,OUTPUT);
  pinMode(RELAY_3,OUTPUT);
  pinMode(RELAY_4,OUTPUT);
  digitalWrite(RELAY_1, OFF);
  digitalWrite(RELAY_2, OFF);
  digitalWrite(RELAY_3, OFF);
  digitalWrite(RELAY_4, OFF);
  Status_Relay_1 = "OFF";
  Status_Relay_2 = "OFF";
  Status_Relay_3 = "OFF";
  Status_Relay_4 = "OFF";
}

// MENU SETUP....................................................................

void Setup_DateTime(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Setup Date:");
  lcd.setCursor(0,1);
  lcd.print("00/00/0000");
  lcd.setCursor(0,1);
  lcd.blink();

  char keypassed1 = Get_OnlyDigitKey(); 
  lcd.print(keypassed1);
  lcd.setCursor(1, 1);

  char keypassed2 = Get_OnlyDigitKey(); 
  lcd.print(keypassed2);
  lcd.setCursor(3, 1);
  
  char keypassed3 = Get_OnlyDigitKey();
  lcd.print(keypassed3);
  lcd.setCursor(4, 1);
  
  char keypassed4 = Get_OnlyDigitKey(); 
  lcd.print(keypassed4);
  lcd.setCursor(6, 1);
  
  char keypassed5 = Get_OnlyDigitKey(); 
  lcd.print(keypassed5);
  lcd.setCursor(7, 1);
    
  char keypassed6 = Get_OnlyDigitKey(); 
  lcd.print(keypassed6);
  lcd.setCursor(8, 1);
    
  char keypassed7 = Get_OnlyDigitKey(); 
  lcd.print(keypassed7);
  lcd.setCursor(9, 1);
    
  char keypassed8 = Get_OnlyDigitKey(); 
  lcd.print(keypassed8);
  
  lcd.noBlink();

  int d1 = (keypassed1-48)*10;
  int d2 = keypassed2-48;
  int dd = d1+d2;
    
  int m1 = (keypassed3-48)*10;
  int m2 = keypassed4-48;
  int mm = m1+m2;

  int y1 = (keypassed5-48)*1000;
  int y2 = (keypassed6-48)*100;
  int y3 = (keypassed7-48)*10;
  int y4 = keypassed8-48;
  int yyyy = y1+y2+y3+y4;
    
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setup Time:");
  lcd.setCursor(0, 1);
  lcd.print("00:00:00");
  lcd.setCursor(0,1);
  lcd.blink();

  char keypassed9 = Get_OnlyDigitKey(); 
  lcd.print(keypassed9);
  lcd.setCursor(1, 1);
    
  char keypassed10 = Get_OnlyDigitKey(); 
  lcd.print(keypassed10);
  lcd.setCursor(3, 1);
  
  char keypassed11 = Get_OnlyDigitKey(); 
  lcd.print(keypassed11);
  lcd.setCursor(4, 1);
  
  char keypassed12 = Get_OnlyDigitKey(); 
  lcd.print(keypassed12);
  lcd.setCursor(6, 1);
  
  char keypassed13 = Get_OnlyDigitKey(); 
  lcd.print(keypassed13);
  lcd.setCursor(7, 1);
  
  char keypassed14 = Get_OnlyDigitKey(); 
  lcd.print(keypassed14);
  
  lcd.noBlink();

  int h1 = (keypassed9-48)*10;
  int h2 = keypassed10-48;
  int hh = h1+h2;
    
  int n1 = (keypassed11-48)*10;
  int n2 = keypassed12-48;
  int mn = n1+n2;

  int s1 = (keypassed13-48)*10;
  int s2 = keypassed14-48;
  int ss = s1+s2;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("#: Save");
  lcd.setCursor(0,1);
  lcd.print("*: Cancel");
  char keypassed = keypad.waitForKey();

  lcd.setCursor(0,1);
  if (keypassed == '#'){
    myRTC.setDS1302Time(ss, mn, hh, 1, dd, mm, yyyy); // (seconds, minutes, hours, day of the week, day of the month, month, year)
    // day of the week is no used in our application then, default set it to 1 (or any number 1-7)
    lcd.print("      Saved");
  } else {
    lcd.print("      Cancel");
  }
  
  delay(200);
  
  // Exit Setting Up
  keypassed = NO_KEY;
  keyhistory = "*";

}

// GET FUNCTIONS....................................................................
void Get_Parm() {
  
}

char Get_OnlyDigitKey(){
  char waitkey;
  while(!isDigit(waitkey)){
    waitkey = keypad.waitForKey();
  }
  return waitkey;
}

// PUT FUNCTIONS....................................................................

// SHOW FUNCTIONS....................................................................
void Show_clock() {
  if ( millis() - show_time >1000 ) {
    show_time = millis();
    myRTC.updateTime();
    lcd.clear();
    lcd.setCursor(0,0);
    if(myRTC.dayofmonth<10){
      lcd.print("0");
    }
    lcd.print(myRTC.dayofmonth);
    lcd.print("/");
    if(myRTC.month<10){
      lcd.print("0");
    }
    lcd.print(myRTC.month);
    lcd.print("/");
    lcd.print(myRTC.year);
    lcd.setCursor(0,1);

    if(myRTC.hours<10){
      lcd.print("0");
    }
    lcd.print(myRTC.hours);
    lcd.print(":");
    if(myRTC.minutes<10){
      lcd.print("0");
    }
    lcd.print(myRTC.minutes);
    lcd.print(":");
    if(myRTC.seconds<10){
      lcd.print("0");
    }
    lcd.print(myRTC.seconds);
  }
  
}

void Show_Value() {
 Show_clock();
  
}

// CONTROL FUNCTIONS....................................................................
  
void Control(){
  
  if (MODE == 2){
    Control_Timer();
  } else if (MODE == 1) {
    Control_Auto();
  }
  
}
void Control_Auto() {
  
}

void Control_Timer() {

  short int th1 = 6; 
  short int tm1 = 0; 
  short int th2 = 17; 
  short int tm2 = 0; 

  int rumtime = 5; // minute
  
  myRTC.updateTime();
  
  int h = myRTC.hours;
  int m = myRTC.minutes;
  // set time hotnam
  if ((h == th1 &&  m == tm1) || (h == th2 &&  m == tm2)) {
    digitalWrite(RELAY_4, ON);
    Status_Relay_4 = "ON";
    
  }

  if (Status_Relay_4 == "ON") {
    if ((h == th1 &&  m > tm1+5) || (h == th2 &&  m > tm2+2)) {
      digitalWrite(RELAY_4, OFF);
      Status_Relay_4 = "OFF";
    }
    
  }
  
}

void Control_Manual() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  Maual Control");
  lcd.setCursor(0,1);
  lcd.print("D: Water = ");
  
  while(keyhistory == "A") {
    
    lcd.setCursor(11,1);
    lcd.print(Status_Relay_4);
    lcd.print("   ");
    keypressed = keypad.getKey();

    if(keypressed=='D'){
//      Control Water Vale
      if (Status_Relay_4 == "OFF"){
        digitalWrite(RELAY_4, ON);
        Status_Relay_4 = "ON";
      } else{
        digitalWrite(RELAY_4, OFF);
        Status_Relay_4 = "OFF";
      }

    } else 
    
    if (keypressed == '0' || keypressed == '*'){
      keyhistory = "";
    }
    delay(200);
  }
  delay(200);
}
