#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int soundPin = A0;
const int ledPin = 8;

const int threshold = 600;

volatile int soundValue = 0;   // shared with ISR
volatile bool newData = false; // flag for main loop

void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  lcd.begin(16,2);
  lcd.print("Sound Monitor");

  // -------- TIMER1 CONFIG (100 ms interrupt) --------
  noInterrupts();

  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1B |= (1 << WGM12);   // CTC mode

  // Prescaler = 64 → 16MHz/64 = 250kHz
  // 100 ms → 250000 * 0.1 = 25000 counts
  OCR1A = 24999;

  TCCR1B |= (1 << CS11) | (1 << CS10); // prescaler 64

  TIMSK1 |= (1 << OCIE1A); // enable interrupt

  interrupts();
}

void loop()
{
  // Only display (no heavy work)
  if(newData)
  {
    newData = false;

    lcd.setCursor(0,1);
    lcd.print("Lvl:");
    lcd.print(soundValue);
    lcd.print("    ");

    Serial.print("SOUND:");
    Serial.println(soundValue);
  }
}

// -------- TIMER INTERRUPT --------
ISR(TIMER1_COMPA_vect)
{
  soundValue = analogRead(soundPin);

  if(soundValue > threshold)
  {
    digitalWrite(ledPin, HIGH);
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }

  newData = true;
}
