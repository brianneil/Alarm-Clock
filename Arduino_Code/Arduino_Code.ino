/* This code will run an alarm clock with a settable
    alarm time and clock time
    The alarm works by playing an MP3 from an MP3 shield
    and is meant to have a code adjustable snooze

    Uses code from: https://www.arduino.cc/en/Tutorial/HelloWorld
*/

//Includes for the display + MP3 player
#include <LiquidCrystal.h>
#include <SPI.h>           // SPI library
#include <SdFat.h>         // SDFat Library
#include <SdFatUtil.h>     // SDFat Util Library
#include <SFEMP3Shield.h>  // Mp3 Shield Library

#define TICK 1000  //1000 milliseconds (1 second)
#define SNOOZETIME 2  //This will allow for X minutes of snoozing
#define ALARMSWITCHPIN 4
#define SNOOZEBUTTONPIN 3
#define LEFT 0
#define RIGHT 1

//Inspired by the characters from http://forum.arduino.cc/index.php?topic=8882.0
byte Colon[8] = 
{
 B00000,
 B00000,
 B01110,
 B01110,
 B01110,
 B00000,
 B00000,
 B00000
};
byte UpperThird[8] =
{
 B11111,
 B11111,
 B11111,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000
};
byte LowerThird[8] =
{
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B11111,
 B11111,
 B11111
};

byte UpperAndBottom[8] =
{
 B11111,
 B11111,
 B11111,
 B00000,
 B00000,
 B00000,
 B11111,
 B11111
};
byte UpperBitBottomThick[8] =
{
 B11111,
 B00000,
 B00000,
 B00000,
 B00000,
 B11111,
 B11111,
 B11111
};
byte UpperBit[8] = 
{
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte BottomThickOnly[8] = 
{
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
};
byte AlarmBell[8] =
{
  B00100,
  B01110,
  B01110,
  B01110,
  B11111,
  B00000,
  B00100,
  B00000,
};

LiquidCrystal lcd(A5, A4, A3, A2, A1, A0); //Establishes the LCD with proper pins

//time variables
typedef struct {
  int hours;
  int minutes;
  int seconds;
  bool AM;
} TheTime;

typedef struct {
  int hour;
  int minute;
  bool AM;
} Alarm;

enum AlarmStates {
  Off,
  Armed,
  Alarming,
  Snoozing,
};

//Instances
Alarm currentAlarm;
TheTime currentTime;
AlarmStates alarmState;
SdFat sd; // Create object to handle SD functions
SFEMP3Shield MP3player; // Create Mp3 library object

//Globals
unsigned long targetMillis = 0;
int timeSnoozing;
bool alarmSwitch;
bool snoozeButton;
int masterVolume = 65;
const uint16_t monoMode = 0;  // Mono setting 0 = stereo. 3 is max (no idea what max mono mode means though)

void setup() {
  //Store the custom blocks on the LCD display
  lcd.createChar(0,Colon);
  lcd.createChar(1,UpperThird);
  lcd.createChar(2,LowerThird);
  lcd.createChar(3,UpperAndBottom);
  lcd.createChar(4,UpperBitBottomThick);
  lcd.createChar(5,UpperBit);
  lcd.createChar(6,BottomThickOnly);
  lcd.createChar(7,AlarmBell);

  pinMode(ALARMSWITCHPIN, INPUT_PULLUP);
  pinMode(SNOOZEBUTTONPIN, INPUT_PULLUP);
  
  //Set up the lcd number of columns and rows.
  lcd.begin(16, 2);
  Serial.begin(9600); //For debugging

  //Initializes time
  currentTime.hours = 7;
  currentTime.minutes = 45;
  currentTime.seconds = 30;
  currentTime.AM = true;
  PrintTime();

  //Initializes an alarm
  currentAlarm.hour = 7;
  currentAlarm.minute = 46;
  currentAlarm.AM = true;

  //Initializes alarm state
  alarmState = Off;

  //Initialize MP3 Player
  initSD();
  initMP3Player();

  targetMillis = TICK;  //Increment the target millisecond counter
}

void loop() {
  if ((long)( millis() - targetMillis ) >= 0) { //A second has elapsed
    currentTime.seconds += 1;
    if (currentTime.seconds >= 60) {  //End of a minute
      UpdateMinute();
    }
    targetMillis += TICK; 
  }

  //Check the pins (both are inverted because grounding is them being fired)
  alarmSwitch = !digitalRead(ALARMSWITCHPIN);
  snoozeButton = !digitalRead(SNOOZEBUTTONPIN);

  //Check to see the alarm status
  CheckAlarm();
}

void UpdateMinute() {
  currentTime.seconds = 0;
  currentTime.minutes++;
  if (alarmState == Snoozing) {
    timeSnoozing++;
  }
  if (currentTime.minutes >= 60) { //End of an hour
    currentTime.minutes = 0;
    currentTime.hours++;
    if (currentTime.hours == 12) {  //we just hit midnight or noon
      currentTime.AM = !currentTime.AM; //flip the boolean for AM
      }
    }
  if (currentTime.hours > 12) { //12 hour loop is over
    currentTime.hours = 1;
  }
  PrintTime();  //Puts the new minute up on the display
}

void PrintTime() {
  //Prep display
  lcd.clear();
  lcd.home();
  int cursorX = 0;
  
  //Prep numbers to print
  int hoursTens = currentTime.hours/10;
  int hoursOnes = currentTime.hours%10;
  int minuteTens = currentTime.minutes/10;
  int minuteOnes = currentTime.minutes%10;

  if (hoursTens > 0) {  //If the tens place in hrs has a value, print it and move the cursor. Otherwise just move the cursor
    PrintBigDigit(hoursTens, cursorX);
  }
  cursorX += 3; //Only move over 3 here since the 1 takes up two places.
  PrintBigDigit(hoursOnes, cursorX);
  cursorX += 3; //Only move 3 since we'll put the colon right next to the ones hour digit.
  PrintColon(cursorX);
  cursorX += 1;
  PrintBigDigit(minuteTens, cursorX);
  cursorX += 4;
  PrintBigDigit(minuteOnes, cursorX);
  cursorX += 3;
  PrintAMPM(cursorX, currentTime.AM);
  cursorX += 1;
  PrintBell(cursorX);
}

void PrintColon(int x) {
 lcd.setCursor(x, 0); //Go to the correct X position, top half
 lcd.write(byte(0));
 lcd.setCursor(x, 1); //Go to the bottom row
 lcd.write(byte(0));
}

void PrintBell(int x) {
  lcd.setCursor(x,1); //Go to the bottom half, we'll use upper half for AM/PM
  lcd.write(7);
}

void PrintAMPM(int x, bool ItIsAM) {
  lcd.setCursor(x, 0); //Top half
  if (ItIsAM) {
    lcd.write("AM");
  } else {
    lcd.write("PM");
  }
}

void CheckAlarm() {
   switch(alarmState) {
    case Off:
      if(alarmSwitch) { //Alarm is turned on
        alarmState = Armed;
      }
      break;
     case Armed:
      if(!alarmSwitch) { //Alarm is turned off
        alarmState = Off;
      }
      if(AlarmTime()) {
        PlayAlarm();
        alarmState = Alarming;
      }
      break;
     case Alarming:
      if(snoozeButton) { //User hit the snooze button
        PauseAlarm();
        alarmState = Snoozing;
        timeSnoozing = 0; //Reset the snooze timer. It gets updated in the minute updater.
      }
      if(!alarmSwitch) { //Alarm is turned off
        alarmState = Off;
      }
      break;
     case Snoozing:
      if(timeSnoozing >= SNOOZETIME) {
        ResumeAlarm();
        alarmState = Alarming;
      }
      if(!alarmSwitch) { //Alarm is turned off
        alarmState = Off;
      }
      break;
   }
}

bool AlarmTime() {
  bool TimeForAlarm = false;
  if (currentTime.hours == currentAlarm.hour && currentTime.minutes == currentAlarm.minute && currentTime.AM == currentAlarm.AM) { //time matches
    TimeForAlarm = true;
  }
  return TimeForAlarm;
}

void PlayAlarm() {
  //play the music
  uint8_t result = MP3player.playTrack(1);
  
  //For now, also put an A in the corner so we have some visual
  lcd.setCursor(0,0);
  lcd.write("A");
}

void PauseAlarm() {
  //Pause the music (reset the volume?)
  MP3player.pauseMusic();
  //For now also put an S in the corner
  lcd.setCursor(0,0);
  lcd.write("S");
}

void ResumeAlarm() {
  MP3player.resumeMusic();
  //For now, also put an A in the corner so we have some visual
  lcd.setCursor(0,0);
  lcd.write("A");
}

void PrintBigDigit(int digit, int x) {
  switch(digit) {
      case 0:
        Custom0(x);
        break;     
      case 1:
        Custom1(x);
        break;
      case 2:
        Custom2(x);
        break;
      case 3:
        Custom3(x);
        break;
      case 4:
        Custom4(x);
        break;
      case 5:
        Custom5(x);
        break;
      case 6:
        Custom6(x);
        break;
      case 7:
        Custom7(x);
        break;
      case 8:
        Custom8(x);
        break;
      case 9:
        Custom9(x);
        break;
    }
}

//Functions to display the large font numbers
void Custom0(int x)
{ // uses segments to build the number 0
 lcd.setCursor(x, 0); // set cursor to column 0, line 0 (first row)
 lcd.write(255);  // Next 3 lines are the 3 columns of the upper half of the number
 lcd.write(1);
 lcd.write(255);
 lcd.setCursor(x, 1); // set cursor to colum 0, line 1 (second row)
 lcd.write(255);  // Next 3 lines are the columns of the bottom half
 lcd.write(2);
 lcd.write(255);
}

void Custom1(int x)
{
 lcd.setCursor(x,0);
 lcd.write(1);
 lcd.write(255);
 lcd.setCursor(x+1,1);  //The +1 will bump over 1 column
 lcd.write(255);
}

void Custom2(int x)
{
 lcd.setCursor(x,0);
 lcd.write(3);
 lcd.write(3);
 lcd.write(255);
 lcd.setCursor(x, 1);
 lcd.write(255);
 lcd.write(4);
 lcd.write(4);
}

void Custom3(int x)
{
 lcd.setCursor(x,0);
 lcd.write(3);
 lcd.write(3);
 lcd.write(255);
 lcd.setCursor(x, 1);
 lcd.write(4);
 lcd.write(4);
 lcd.write(255); 
}

void Custom4(int x)
{
 lcd.setCursor(x,0);
 lcd.write(255);
 lcd.write(6);
 lcd.write(255);
 lcd.setCursor(x+2, 1);
 lcd.write(255);
}

void Custom5(int x)
{
 lcd.setCursor(x,0);
 lcd.write(255);
 lcd.write(3);
 lcd.write(3);
 lcd.setCursor(x, 1);
 lcd.write(4);
 lcd.write(4);
 lcd.write(255);
}

void Custom6(int x)
{
 lcd.setCursor(x,0);
 lcd.write(255);
 lcd.write(3);
 lcd.write(3);
 lcd.setCursor(x, 1);
 lcd.write(255);
 lcd.write(4);
 lcd.write(255);
}

void Custom7(int x)
{
 lcd.setCursor(x,0);
 lcd.write(1);
 lcd.write(1);
 lcd.write(255);
 lcd.setCursor(x+2, 1);
 lcd.write(255);
}

void Custom8(int x)
{
 lcd.setCursor(x,0);
 lcd.write(255);
 lcd.write(3);
 lcd.write(255);
 lcd.setCursor(x, 1);
 lcd.write(255);
 lcd.write(4);
 lcd.write(255);
}

void Custom9(int x)
{
 lcd.setCursor(x,0);
 lcd.write(255);
 lcd.write(3);
 lcd.write(255);
 lcd.setCursor(x, 1);
 lcd.write(5);
 lcd.write(5);
 lcd.write(255);
}

void initMP3Player() {
  uint8_t result = MP3player.begin(); // init the mp3 player shield
  if (result != 0) // check result, see readme for error codes.
  {
    Serial.println(result); //To do: handle errors here in some way?
  }
  union twobyte volume;  //creates a variable that can has a byte for the left and right ear volumes
  volume.byte[LEFT] = masterVolume;        //Sets some starting values. 0 is full loudness, 255 is full quiet
  volume.byte[RIGHT] = masterVolume;
  MP3player.setVolume(volume.byte[LEFT], volume.byte[RIGHT]); //Pushes the new volumes onto the player
  MP3player.setMonoMode(monoMode);  //Pushes mono settings
}

void initSD() {   //Code taken from the sparkfun example, not sure how it works.
  //Initialize the SdCard.
  if (!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
  // depending upon your SdCard environment, SPI_HAVE_SPEED may work better.
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");
}


