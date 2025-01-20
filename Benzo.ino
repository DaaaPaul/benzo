/*
 * Benzo
 * An everyday microscope created around the Arduino
 * Made by Paul Peng
 * Last Modified 1/20/2024
 */

#include <Servo.h>
#define button1 2 // the input pin that is LOW when button1 is pressed (pull-up resistor)
#define button2 4 // the input pin that is LOW when button2 is pressed (pull-up resistor)
#define lens1On 63 // the "on" position of lens1
#define lens1Off 160 // the "off" position of lens1
#define lens2On 16 // the "on" position of lens2
#define lens2Off 93 // the "off" position of lens2
#define joystickX A0 // the input pin for the x position of the joystick (don't need y)

// create servo objects for the 2 lens servos
Servo lens1Servo;
Servo lens2Servo;

// create tracking flags for button logic
bool button1PrevOff = true;
bool lens1State = false;
unsigned long button1LastPressTime = 0;
bool button2PrevOff = true;
bool lens2State = false;
unsigned long button2LastPressTime = 0;

// create platform movement servos and position tracking variables
Servo northServo;
Servo southServo;
int northSouthServoPos = 93; // the position for both servos

/*
 * bool buttonPressed(int button)
 * int button - the pin to read for button signal
 * returns bool - true the button was pressed, assumes pull-up resistor mechanics
 * checks the input to a pin, signaling if a button is pressed
 */
bool buttonPressed(int button) {
  return digitalRead(button) == LOW;
}

/*
 * bool overMinimumInterval(unsigned long lastTime, unsigned long minimumInterval)
 * unsigned long lastTime - the previous time the component was activated
 * unsigned long minimumInterval - the minimum interval of time that should pass before the component can activate again
 * returns bool - true if the activation was over the minimum interval
 * checks if a component should be allowed to activate, based on the previous time it activated
 */
bool overMinimumInterval(unsigned long lastTime, unsigned long minimumInterval) {
  unsigned long time = millis(); // the time in milliseconds since the program began running

  return time - lastTime >= minimumInterval;
}

void setup() {
  pinMode(button1, INPUT_PULLUP); // make sure to set buttons to INPUT_PULLUP since they are using pull-up resistors
  pinMode(button2, INPUT_PULLUP);
  
  lens1Servo.attach(9);
  lens2Servo.attach(10);
  lens1Servo.write(lens1Off); // set lenses to the off position as the default beginning state
  lens2Servo.write(lens2Off);

  northServo.attach(3); 
  southServo.attach(5);
  northServo.write(northSouthServoPos); // write platform movement servos to the vertical position
  southServo.write(northSouthServoPos + 8); // the south side servo's vertical position is offset by 8 compared to the north side servo, their real positions are the same here
}

void loop() {
  unsigned long time = millis(); // time since begin variable for later use
  
  // if button1 was pressed 
  if(buttonPressed(button1) && button1PrevOff && overMinimumInterval(button1LastPressTime, 50)) {
    // update trackers accordingly
    button1LastPressTime = time;
    button1PrevOff = false;

    // negate lens1State and write the servo position accordingly
    lens1State = !lens1State;
    (lens1State) ? lens1Servo.write(lens1On) : lens1Servo.write(lens1Off);
  }
  if(!buttonPressed(button1)) button1PrevOff = true; // make sure to update button1PrevOff accordingly

  // if button2 was pressed
  if(buttonPressed(button2) && button2PrevOff && overMinimumInterval(button2LastPressTime, 50)) {
    // update trackers accordingly
    button2LastPressTime = time;
    button2PrevOff = false;

    // negate lens2State and write the servo position accordingly
    lens2State = !lens2State;
    (lens2State) ? lens2Servo.write(lens2On) : lens2Servo.write(lens2Off);
  }
  if(!buttonPressed(button2)) button2PrevOff = true; // make sure to update button2PrevOff accordingly

  if(analogRead(joystickX) > 540 || analogRead(joystickX) < 460) {
    int joystickXServoPos = (analogRead(joystickX) - 512)/5.7;
    int desiredServoPos = northSouthServoPos - joystickXServoPos;
    desiredServoPos = constrain(desiredServoPos, 0, 180);

    while(northSouthServoPos != desiredServoPos) {
      if(northSouthServoPos < desiredServoPos) northSouthServoPos++;
      else northSouthServoPos--;

      northServo.write(northSouthServoPos);
      southServo.write(northSouthServoPos + 8);
      delay(7);
    }
  }
}