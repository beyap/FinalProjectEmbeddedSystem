/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>
#include <LiquidCrystal.h>

Servo myservo1;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo
const int buttonPower = A2;     // the number of the pushbutton pin
const int buttonLeft = 13;     // the number of the pushbutton pin
const int buttonRight = 12;     // the number of the pushbutton pin
const int pingPin = 7; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor
const int ledGreen = A1;
const int ledRed = A0;
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int reading;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 11, en = 8, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int power = 0; // variable to store the status of the game (on == 1, off == 0)
int gameIsOver = 0; // variable to store whether the player has lost (lost == 1, playing == 0)
int pos = 0;    // variable to store the servo position
int buttonStateLeft = 0;         // variable for reading the pushbutton status
int buttonStateRight = 0;         // variable for reading the pushbutton status
int buttonStatePower = 0;         // variable for reading the pushbutton status
int points = 0;

void setup() {
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(9600); // Starting Serial Terminal
  // initialize the pushbutton pin as an input:
  pinMode(buttonLeft, INPUT);
  pinMode(buttonRight, INPUT);
  pinMode(buttonPower, INPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  myservo1.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo2.attach(10);  // attaches the servo on pin 10 to the servo object
  myservo1.write(90);
  myservo2.write(90);

}

void loop() {
  
  // Setup Ultrasonic Sensor
   long duration, inches, cm;
   pinMode(pingPin, OUTPUT);
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   pinMode(echoPin, INPUT);
   duration = pulseIn(echoPin, HIGH);
   inches = microsecondsToInches(duration);
   cm = microsecondsToCentimeters(duration);
   Serial.print(inches);
   Serial.print("in, ");
   Serial.print(cm);
   Serial.print("cm");
   Serial.println();
   delay(100);
  
  buttonStateLeft = digitalRead(buttonLeft);
  buttonStateRight = digitalRead(buttonRight);
  reading = digitalRead(buttonPower);

  debounce();

  buttonStatePower = buttonState;

  if (buttonStatePower == HIGH) {
    if (power == 0) { // game is turned on
      points = 0;
      power = 1;
      gameIsOver = 0;
      digitalWrite(ledGreen, HIGH);
    }
    else { // game is turned off
      power = 0;
      points = 0;
      digitalWrite(ledGreen, LOW);
    }
  }

  if (power == 1) {
    if (gameIsOver == 1) {
      digitalWrite(ledGreen, LOW);
    }
    else {
      digitalWrite(ledGreen, HIGH);
      points = points + 1;
      // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
      if (buttonStateLeft == HIGH) {
        leftFlipper();
      } 
      if (buttonStateRight == HIGH) {
        rightFlipper();
      }
      // check to see if the ball has fallen below the flippers
      if (inches <= 2) {
        gameOver();
      }
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Points: ");
        lcd.setCursor(8, 0);
        lcd.print(points);
      }
    }
  }

  else {
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, LOW);
    lcd.clear();
  }
  

}

void leftFlipper() {
  for (pos = 30; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo1.write(180);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 5ms for the servo to reach the position
  }
  for (pos = 0; pos <= 30; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo1.write(0);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 5ms for the servo to reach the position
  }
  myservo1.write(90);
}

void rightFlipper() {
  for (pos = 0; pos <= 30; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo2.write(0);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 5ms for the servo to reach the position
  }
  for (pos = 30; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo2.write(180);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 5ms for the servo to reach the position
  }
  myservo2.write(90);
}

void gameOver() {
  // Print a message to the LCD.
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Game Over!");
  gameIsOver = 1;
  lcd.setCursor(0, 1);
  lcd.print("Points: ");
  lcd.setCursor(8, 1);
  lcd.print(points);
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, HIGH);
}

long microsecondsToInches(long microseconds) {
   return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}

void debounce() {
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
