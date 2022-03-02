#include <LiquidCrystal.h>
#include <Wire.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
#define EEPROM_I2C_ADDRESS 0x50
#define joyX A1
#define joyY A0
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int sw = 6;
const int a0 = 13;
const int buzzpin = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// joystick var
int xValue;
int yValue;
// Shared Vars
int joystickMovement = 0;
bool select = false;

// EEPROM Helper Functions
// reference video https://www.youtube.com/watch?v=ShqvATqXA7g&t=421s&ab_channel=DroneBotWorkshop

// for reading from the EEPROM
void writeEEPROM(int address, byte val, int i2c_address) {
  Wire.beginTransmission(i2c_address);

  Wire.write((int)(address >> 8)); // Most significant bit
  Wire.write((int)(address & 0xFF)); // Least significant bit

  //sending data
  Wire.write(val);

  Wire.endTransmission();

  //Add delay write
  delay(5);
}

// for writing to eeprom
byte readEEPROM(int address, int i2c_address) {
  byte rcvData = 0xFF;
  
  Wire.beginTransmission(i2c_address);
  
  Wire.write((int)(address >> 8)); // Most significant bit
  Wire.write((int)(address & 0xFF)); // Least significant bit

  Wire.endTransmission();

  Wire.requestFrom(i2c_address, 1);

  rcvData = Wire.read();

  return rcvData;
}

// ----- Start of Tasks ----

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
    
} task;

const unsigned short tasksNum = 3;
task tasks[tasksNum];

// ------------------- Joy Stick Task --------------------------
enum JoyStick_states {joyStart, checkDirection, left, right};
int TickFct_JoyStick(int state)
{ 
  //Read thing
    switch(state){ // State transitions
      case joyStart:
        joystickMovement = 0;
        state = checkDirection;
        break;
      case checkDirection:
        if(xValue > 1)
        {
          joystickMovement = -1;
          state =left;
        }
        else if(xValue < 1)
        {
          joystickMovement = 1;
          state = right;
        }
        else if(xValue == 1)
        {
          joystickMovement = 0;
        }
        else {
          state = checkDirection;
        }
        break;
      case left:
        //Serial.println("joystick is moved left");
        state = checkDirection;
        break;
      case right:
        //Serial.println("joystick is moved right");
        state = checkDirection;
        break;
      default:
        state = joyStart;
        break;
    }
    switch(state){ // State Action
      case joyStart:
        break;
      case checkDirection:
        xValue = analogRead(joyX);
        //yValue = analogRead(joyY);
        xValue= map(xValue, 0, 1023, 0 , 3);
        break;
      case left:
        break;
      case right:
        break;
      default:
        break;
    }
  return state;
  }
  
// ------------ Btn Task ------------------
  enum Btn_states {btnStart, released, pressed};
  
  int TickFct_btn(int state)
  {
    // transitions
    switch(state)
    {
      case btnStart:
        select = false;
        state = released;
        break;
      case released:
        if(digitalRead(a0))
        {
          state = pressed;
        }
        else if(!digitalRead(a0)){
          select = false;
          state = released;
        }
        break;
      case pressed:
        if(digitalRead(a0))
        {
          state = pressed;
        }
        else if(!digitalRead(a0)){
          select = true;
          state = released;
        }
        break;
      default:
        state = btnStart;
    }
    // ACTIONS
    switch(state)
    {
      case btnStart:
        break;
      case released:
        if(select)
        {
          Serial.println("select btn pressed");
        }
        break;
      case pressed:
        break;
      default:
        break;
    }
    return state;
  }
// -------- Buzzer task (wirered right and working)---------------
 enum Bzr_states {bzrStart , bzrOn, bzrOff};

 int TickFct_bzr(int state) {
  // transitions
  switch(state){
    case bzrStart:
      state = bzrOn;
      break;
    case bzrOn:
      state = bzrOff;
      break;
    case bzrOff:
      state = bzrOn;
      break;
    default:
      break;
  }
//actions
  switch(state){
    case bzrStart:
      break;
    case bzrOn:
      //digitalWrite(buzzpin, HIGH);
      //Serial.println(digitalRead(buzzpin));
      break;
    case bzrOff:
      //digitalWrite(buzzpin, LOW);
      break;
    default:
      break;
  }

  return state;
 }

void setup() {
  // Task scheduler
  unsigned char i = 0;
  tasks[i].state = joyStart;
  tasks[i].period = 100;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &TickFct_JoyStick;
  i++;
  tasks[i].state = btnStart;
  tasks[i].period = 100;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &TickFct_btn;
  i++;
  tasks[i].state = bzrStart;
  tasks[i].period = 100;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &TickFct_bzr;

  //EEPROM Setup
  Wire.begin();

  // pin setup
  pinMode(13, INPUT);
  pinMode(buzzpin, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.blink();
  lcd.write("testing");
  Serial.begin(9600);
  // samples of how to use EEPROM
  //writeEEPROM(100, 35, EEPROM_I2C_ADDRESS);
  //Serial.println(readEEPROM(100, EEPROM_I2C_ADDRESS));
  
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
     if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
        tasks[i].state = tasks[i].TickFct(tasks[i].state);
        tasks[i].elapsedTime = millis(); // Last time this task was ran
      
     }
   }
  delay(100); // GCD.
}
