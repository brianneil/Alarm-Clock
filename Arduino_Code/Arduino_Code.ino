/* This code will run an alarm clock with a settable
 *  alarm time and clock time
 *  The alarm works by playing an MP3 from an MP3 shield
 *  and is meant to have a code adjustable snooze
 *  
 *  Uses code from: https://www.arduino.cc/en/Tutorial/HelloWorld
 */

#include <LiquidCrystal.h>

#define TICK 1000;

LiquidCrystal lcd(12,11,5,4,3,2); //Establishes the LCD with proper pins

//time variables
int hours;
int minutes;
int seconds;
bool AM;
unsigned long targetMillis = 0;
String timeString;

void setup() {
  //Set up the lcd number of columns and rows.
  lcd.begin(16,2);
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
    timeString = "";  //Clear the string so we can build it
    timeString += hours;
    timeString += ":";
    if (minutes < 10) {
      timeString += "0";
    }
    timeString += minutes;
    timeString += ":";
    if (seconds < 10) {
      timeString += "0";
    }
    timeString += seconds;
    if (AM) {
      timeString += " AM";
    } else {
      timeString += " PM";
    }
    lcd.clear();
    lcd.home();
    lcd.print(timeString);
  }

  
  
//  lcd.print(millis() / 1000);
}
