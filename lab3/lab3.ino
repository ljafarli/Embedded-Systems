#include <LiquidCrystal.h> //Library to control an LCD display
#include <Wire.h>   
#include <uRTCLib.h> //Library to control a real-time clock

uRTCLib rtc(0x68);  //Creates an RTC object called rtc at I²C (communication protocol used for talking to devices like sensors)

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; //Defines the Arduino pins connected to the LCD.
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  //Creates lcd object to control the 16x2 LCD. Its a function

unsigned long seconds = 0;  //current second from the RTC.
unsigned long lastSecond = 0;  //stores the last second value when a button was pressed.
static bool buttonLastState = false; //keeps track of the toggle state of the game.
int buttonPin = A0;  //analog pin A0 is used for the button.

int greenLed = 9;  //pins controlling LEDs
int redLed = 8;

//Detect a clean button press without false triggers.
bool buttonPressed(){
  static bool last = HIGH;  //last read state of button.
  static unsigned long tLastChange = 0;  //time of last state change
  static bool armed = true;  //ensures button only triggers once per press


  bool now = digitalRead(buttonPin);  //Read current button state

///if the button state changed since the last time we checked.
  if (now != last) {
    tLastChange = millis();
    last = now;
  }

  if ((millis() - tLastChange) > 30) {   // debounce
    if (last == LOW && armed) {
      armed = false;  //Once we detect this press, we don’t want to detect it again until the button is released.
      return true;
    }
    if (last == HIGH) armed = true;
  }
  return false;
}


void setup() {

Serial.begin(9600);

lcd.begin(16, 2);
lcd.print("10 SECONDS GAME");

pinMode(greenLed, OUTPUT);
pinMode(redLed, OUTPUT);

pinMode(buttonPin, INPUT);
Wire.begin();
URTCLIB_WIRE.begin();

}

void loop() {
rtc.refresh();  //updates RTC’s internal values.
bool pressed = buttonPressed();   //checks if button was pressed.
seconds = rtc.second();  //gets current seconds from RTC.

if (pressed) {
    buttonLastState = !buttonLastState;  //Toggles the game state between running and paused each time the button is pressed.
  }

//Serial.println(buttonLastState);

  if (!buttonLastState){
  
    if (seconds > 10){
      rtc.set(0, 0, 0, 0, 0, 0, 26);  //If yes, resets the RTC to 00:00:00 (year 26, day 0).
      seconds  = 0;
    }
      lastSecond = seconds;  //Stores current seconds in lastSecond.
  }



  if(buttonLastState){
        rtc.set(lastSecond,rtc.minute(), rtc.hour(),  //Sets the RTC seconds to lastSecond so the timer pauses the second counter.
            rtc.dayOfWeek(), rtc.day(),
            rtc.month(), rtc.year());  //Keeps the minute/hour/day/etc. unchanged.
        seconds = lastSecond;

//chechks win or lose
        if (seconds == 10) {
          Serial.println("Should be green");
          digitalWrite(greenLed, HIGH);
          digitalWrite(redLed, LOW);
        } else if (seconds != 10){
          Serial.println("Should be red");
          digitalWrite(redLed, HIGH);
          digitalWrite(greenLed, LOW);
        } else {
          Serial.println("nothing");
          digitalWrite(greenLed, LOW);
          digitalWrite(redLed, LOW);
        }


  }

  lcd.setCursor(0, 1);  //Moves cursor to second row (row 1, column 0).
  lcd.print("    ");   //Clears previous number (4 spaces).
  lcd.setCursor(0, 1);
  lcd.print(seconds);  ///Prints the current seconds value.

  
}
