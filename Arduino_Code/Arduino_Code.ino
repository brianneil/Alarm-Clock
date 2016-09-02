/* This code will run an alarm clock with a settable
    alarm time and clock time
    The alarm works by playing an MP3 from an MP3 shield
    and is meant to have a code adjustable snooze

    Uses code from: https://www.arduino.cc/en/Tutorial/HelloWorld
*/

#include <LiquidCrystal.h>

#define TICK 1000;

//Stole these custom large font characters from http://forum.arduino.cc/index.php?topic=8882.0
byte LT[8] = 
{B00111, B01111, B11111, B11111, B11111, B11111, B11111, B11111};
byte UB[8] =
{B11111, B11111, B11111, B00000, B00000, B00000, B00000, B00000};
byte RT[8] =
{B11100, B11110, B11111,B11111,B11111,B11111,B11111,B11111};
byte LL[8] =
{B11111,B11111,B11111,B11111,B11111,B11111,B01111,B00111};
byte LB[8] =
{B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111};
byte LR[8] =
{B11111,B11111,B11111,B11111,B11111,B11111,B11110,B11100};
byte MB[8] =
{B11111,B11111,B11111,B00000,B00000,B00000,B11111,B11111};
byte block[8] =
{B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //Establishes the LCD with proper pins

//time variables
int hours;
int minutes;
int seconds;
bool AM;

int cursorXPosition = 0;
int numberToPrint = 0;

int alarmHour;
int alarmMinute;
bool alarmAM;

bool alarmOn;

unsigned long targetMillis = 0;
String timeString;

void setup() {
  //Store the custom blocks on the LCD display
  lcd.createChar(0,LT);
  lcd.createChar(1,UB);
  lcd.createChar(2,RT);
  lcd.createChar(3,LL);
  lcd.createChar(4,LB);
  lcd.createChar(5,LR);
  lcd.createChar(6,MB);
  lcd.createChar(7,block);
  //Set up the lcd number of columns and rows.
  lcd.begin(16, 2);
  Serial.begin(9600);   //It would be good to pull this out later if I don't need it.

  //initializes time to midnight
  hours = 12;
  minutes = 00;
  seconds = 00;
  AM = true;

  targetMillis = TICK;  //Increment the target millisecond counter
  timeString = "";
}

void loop() {
  if ((long)( millis() - targetMillis ) >= 0) { //A second has elapsed
    seconds += 1;
    if (seconds >= 60) {  //End of a minute
      seconds = 0;
      minutes += 1;
    }
    if (minutes >= 60) { //End of an hour
      minutes = 0;
      hours += 1;
      if (hours == 12) {  //we just hit midnight or noon
        AM = !AM; //flip the boolean for AM
      }
    }
    if (hours > 12) { //12 hour loop is over
      hours = 1;
    }
    targetMillis += TICK;
//    lcd.clear();
//    lcd.home();

    printBigDigit(numberToPrint, cursorXPosition);
    numberToPrint++;
    cursorXPosition += 4;
    
//    int hoursTens = hours/10;
//    int hoursOnes = hours%10;
//    if(hoursTens > 0) {
//      Serial.print("!");
//      printBigDigit(hoursTens);
//    }
//    //printBigDigit(hoursOnes);
    

    
//    timeString = "";  //Clear the string so we can build it
//    timeString += hours;
//    timeString += ":";
//    if (minutes < 10) {
//      timeString += "0";
//    }
//    timeString += minutes;
//    timeString += ":";
//    if (seconds < 10) {
//      timeString += "0";
//    }
//    timeString += seconds;
//    if (AM) {
//      timeString += " AM";
//    } else {
//      timeString += " PM";
//    }
//    lcd.clear();
//    lcd.home();
//    lcd.print(timeString);
  }
}

void printBigDigit(int digit, int cursorX) {
  switch(digit) {
      case 0:
        custom0(cursorX);
        break;     
      case 1:
        custom1(cursorX);
        break;
      case 2:
        custom2(cursorX);
        break;
      case 3:
        custom3(cursorX);
        break;
      case 4:
        custom4(cursorX);
        break;
      case 5:
        custom5(cursorX);
        break;
      case 6:
        custom6(cursorX);
        break;
      case 7:
        custom7(cursorX);
        break;
      case 8:
        custom8(cursorX);
        break;
      case 9:
        custom9(cursorX);
        break;
    }
}

//Functions to display the large font numbers
void custom0(int cursor)
{ // uses segments to build the number 0
 lcd.setCursor(cursor,0); // set cursor to column 0, line 0 (first row)
 lcd.write(byte(0));  // call each segment to create
 lcd.write(1);  // top half of the number
 lcd.write(2);
 lcd.setCursor(cursor, 1); // set cursor to colum 0, line 1 (second row)
 lcd.write(3);  // call each segment to create
 lcd.write(4);  // bottom half of the number
 lcd.write(5);
}

void custom1(int cursor)
{
 lcd.setCursor(cursor,0);
 lcd.write(1);
 lcd.write(2);
 lcd.setCursor(cursor,1);
 lcd.write(4);
 lcd.write(7);
 lcd.write(4);
}

void custom2(int cursor)
{
 lcd.setCursor(cursor,0);
 lcd.write(6);
 lcd.write(6);
 lcd.write(2);
 lcd.setCursor(cursor, 1);
 lcd.write(3);
 lcd.write(4);
 lcd.write(4);
}

void custom3(int cursor)
{
 lcd.setCursor(cursor,0);
 lcd.write(6);
 lcd.write(6);
 lcd.write(2);
 lcd.setCursor(cursor, 1);
 lcd.write(4);
 lcd.write(4);
 lcd.write(5); 
}

void custom4(int cursor)
{
 lcd.setCursor(cursor,0);
 lcd.write(3);
 lcd.write(4);
 lcd.write(7);
 lcd.setCursor(cursor + 2, 1);
 lcd.write(7);
}

void custom5(int cursor)
{
 lcd.setCursor(cursor,0);
 lcd.write(3);
 lcd.write(6);
 lcd.write(6);
 lcd.setCursor(cursor, 1);
 lcd.write(4);
 lcd.write(4);
 lcd.write(5);
}

void custom6(int cursor)
{
 lcd.setCursor(cursor,0);
 lcd.write(byte(0));
 lcd.write(6);
 lcd.write(6);
 lcd.setCursor(cursor, 1);
 lcd.write(3);
 lcd.write(4);
 lcd.write(5);
}

void custom7(int cursor)
{
 lcd.setCursor(cursor,0);
 lcd.write(1);
 lcd.write(1);
 lcd.write(2);
 lcd.setCursor(cursor + 2, 1);
 lcd.write(7);
}

void custom8(int cursor)
{
 lcd.setCursor(cursor,0);
 lcd.write(byte(0));
 lcd.write(6);
 lcd.write(2);
 lcd.setCursor(cursor, 1);
 lcd.write(3);
 lcd.write(4);
 lcd.write(5);
}

void custom9(int cursor)
{
 lcd.setCursor(cursor,0);
 lcd.write(byte(0));
 lcd.write(6);
 lcd.write(2);
 lcd.setCursor(cursor + 2, 1);
 lcd.write(7);
}

