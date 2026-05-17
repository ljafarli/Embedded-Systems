Arduino Code:

#include <SPI.h>  //enabling spi communication
#include <MFRC522.h>  //rfid library
#include <Keypad.h>  //keypad handlind library
#include <IRremote.hpp>  //ir reciever library

#define SS_PIN 10  //ss=sda
#define RST_PIN 9

#define IR_PIN 2

#define RED_LED 6
#define GREEN_LED 7

MFRC522 rfid(SS_PIN, RST_PIN);  //creates rfid reader object

// ================= KEYPAD =================

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 4, 5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);  //creates keypad object

// ================= PASSWORD =================

String password = "";  //stores keypad password
String enteredIR = "";  //stores remote entered digits

bool locked = false;
bool unlocked = false;

// ================= IR CODES =================

unsigned long IR_0 = 0xE916FF00;
unsigned long IR_1 = 0xF30CFF00;
unsigned long IR_2 = 0xE718FF00;
unsigned long IR_3 = 0xA15EFF00;
unsigned long IR_4 = 0xF708FF00;
unsigned long IR_5 = 0xE31CFF00;
unsigned long IR_6 = 0xA55AFF00;
unsigned long IR_7 = 0xBD42FF00;
unsigned long IR_8 = 0xAD52FF00;
unsigned long IR_9 = 0xB54AFF00;

void setup() {

  Serial.begin(9600);

  SPI.begin(); //starts spi bus
  rfid.PCD_Init();  //initializes rfid reader

  IrReceiver.begin(IR_PIN);  //starts ir receiver

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  setWaitingState();  //initial led state

  Serial.println("SYSTEM READY");
}

void loop() {

  // ================= WAITING STATE =================

  if (!locked) {  //system waits for keypad input

    char key = keypad.getKey();  //reads keypad button

    if (key) {

      if (isDigit(key)) {  //accepts only numbers

        password += key;  //adds digits to password

        Serial.print("Keypad Input: ");
        Serial.println(password);

        if (password.length() == 4) {

          locked = true;  //4 digit entered system locks
          unlocked = false;

          Serial.println("SYSTEM LOCKED");

          setLockedState();
        }
      }
    }
  }

  // ================= LOCKED STATE =================

  else if (locked && !unlocked) {  //ssystem waits for ir unlock

    if (IrReceiver.decode()) {  //checks for remote signal

      unsigned long value = IrReceiver.decodedIRData.decodedRawData;

      char digit = decodeIR(value);  //vonvert hex to digit

      if (digit != 'X') {

        enteredIR += digit;

        Serial.print("IR Input: ");
        Serial.println(enteredIR);

        if (enteredIR.length() == 4) {

          if (enteredIR == password) {

            unlocked = true;

            Serial.println("SYSTEM UNLOCKED");

            setUnlockedState();
          }

          enteredIR = "";
        }
      }

      IrReceiver.resume();
    }
  }

  // ================= UNLOCKED STATE =================

  else if (unlocked) {

    if (!rfid.PICC_IsNewCardPresent()) //checks rfid  near
      return;

    if (!rfid.PICC_ReadCardSerial()) //read card uid
      return;

    String uid = "";

    for (byte i = 0; i < rfid.uid.size; i++) {

      uid += String(rfid.uid.uidByte[i], HEX); //Converts bytes into hex string
    }

    uid.toUpperCase();  //43EB1D3

    Serial.print("TAG:"); 
    Serial.println(uid);

    flashLEDs();

    rfid.PICC_HaltA();
  }
}

// ================= FUNCTIONS =================

void setWaitingState() {

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
}

void setLockedState() {

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
}

void setUnlockedState() {

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
}

void flashLEDs() {

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);

  delay(300);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  delay(300);

  setUnlockedState();
}

char decodeIR(unsigned long code) {

  if (code == IR_0) return '0';
  if (code == IR_1) return '1';
  if (code == IR_2) return '2';
  if (code == IR_3) return '3';
  if (code == IR_4) return '4';
  if (code == IR_5) return '5';
  if (code == IR_6) return '6';
  if (code == IR_7) return '7';
  if (code == IR_8) return '8';
  if (code == IR_9) return '9';

  return 'X';
}
