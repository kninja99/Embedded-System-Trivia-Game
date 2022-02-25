#include <LiquidCrystal.h>


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to

#define joyX A1
#define joyY A0
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int sw = 6;
const int a0 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// joystick var
int xValue;
int yValue;



typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
    
} task;

const unsigned short tasksNum = 2;
task tasks[tasksNum];

// ------------------- Joy Stick Task --------------------------
enum JoyStick_states {joyStart, checkDirection, left, right};
int TickFct_JoyStick(int state)
{ 
  //Read thing
    switch(state){ // State transitions
      case joyStart:
        xValue = analogRead(joyX);
        //yValue = analogRead(joyY);
        xValue= map(xValue, 0, 1023, 0 , 3);
        state = checkDirection;
        break;
      case checkDirection:
        if(xValue > 1)
        {
          state =left;
        }
        else if(xValue < 1)
        {
          state = right;
        }
        else {
          state = checkDirection;
        }
        break;
      case left:
        Serial.println("joystick is moved left");
        state = checkDirection;
        break;
      case right:
        Serial.println("joystick is moved right");
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
        //Serial.println(xValue);
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
        state = released;
        break;
      case released:
        if(digitalRead(a0))
        {
          state = pressed;
        }
        else if(!digitalRead(a0)){
          state = released;
        }
        break;
      case pressed:
        if(digitalRead(a0))
        {
          state = pressed;
        }
        else if(!digitalRead(a0)){
          Serial.println("btn was pressed");
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
        break;
      case pressed:
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
  
  // pin setup
  pinMode(13, INPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.blink();
  lcd.write("testing");
  Serial.begin(9600);
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
