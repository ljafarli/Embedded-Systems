int yellow = 2;
int red = 11;
int blue = 6;
int green = 4;
int xPin = A0;
int yPin = A1;
int sw = 8;

int centerMax = 530;
int centerMin = 490;

void setup() {
  
   Serial.begin(9600);
   pinMode(xPin, INPUT);
   pinMode(yPin, INPUT);
   
}

void loop() {
  
  int xVal = analogRead(xPin);
  int yVal = analogRead(yPin);
  
  Serial.print(xVal);
  Serial.print(" | ");
  Serial.println(yVal);
  
  if ((xVal < centerMin) && (centerMin < yVal && yVal < centerMax)){
    digitalWrite(green, HIGH);
    digitalWrite(red, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(yellow, LOW);
    }
    
  else if ((xVal > centerMax) && (centerMin < yVal && yVal < centerMax)){
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
    digitalWrite(blue, HIGH);
    digitalWrite(yellow, LOW);
    }
    
  else if ((yVal < centerMin)&& (centerMin < xVal && xVal < centerMax)){
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(yellow, HIGH);
    }
    
  else if ((yVal > centerMax)&& (centerMin < xVal && xVal < centerMax) ){
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
    digitalWrite(blue, LOW);
    digitalWrite(yellow, LOW);
    }
    
  else {
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(yellow, LOW);
    }
}
