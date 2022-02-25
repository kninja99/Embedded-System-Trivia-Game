#include <LiquidCrystal.h>


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int a0 = 6, a1 = 7, a2 = 8, a3 = 9 , a4= 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
    
} task;

const unsigned short tasksNum = 1;
task tasks[tasksNum];

enum test_states {testStart};
int TickFct_test(int state)
{ 
  //Read thing
    switch(state){ // State transitions
      case testStart:
        break;
      default:
        state = testStart;
        break;
    }
    switch(state){ // State Action
      case testStart:
        break;
      default:
        break;
    }
  return state;
  }


 

void setup() {
  // Task scheduler
  unsigned char i = 0;
  tasks[i].state = testStart;
  tasks[i].period = 100;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &TickFct_test;
  // pin setup
  //pinMode(a0, INPUT);
 
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.blink();
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
