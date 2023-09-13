#include <Arduino.h>
#include <BleGamepad.h>

#define numOfButtons 12
#define numOfHatSwitches 2

//36,39,34,35 pot pins

const int X = 34;  // Potentiometer is connected to GPIO 34 (Analog ADC1_CH6)
const int Y = 35;
const int rX = 33;
const int rY = 32;
const int numberOfPotSamples = 5;      // Number of pot samples to take (to smooth the values)
const int delayBetweenSamples = 4;     // Delay in milliseconds between pot samples
const int delayBetweenHIDReports = 5;  // Additional delay in milliseconds between HID reports


const int potPins[] = { X, Y, rX, rY };  // X, Y, rX, rY

//int potpins[4]=[X,Y,rX,rY];

byte previousButtonStates[numOfButtons];
byte currentButtonStates[numOfButtons];
byte buttonPins[numOfButtons] = { 13, 12, 14, 27, 36, 39, 19, 18, 5, 23, 4, 6 };
byte physicalButtons[numOfButtons] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };


BleGamepad bleGamepad("pavans joystick", "pavan", 100);
BleGamepadConfiguration bleGamepadConfig;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleGamepadConfig.setAutoReport(false);
  bleGamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD);  // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
  bleGamepadConfig.setButtonCount(numOfButtons);
  bleGamepadConfig.setHatSwitchCount(numOfHatSwitches);
  bleGamepadConfig.setVid(0xe502);
  bleGamepadConfig.setPid(0xabcd);
  //bleGamepadConfig.setWhichAxes(true,true,false,false,false,false,false,false);





  // Some non-Windows operating systems and web based gamepad testers don't like min axis set below 0, so 0 is set by default
  //bleGamepadConfig.setAxesMin(0x8001); // -32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal
  bleGamepadConfig.setAxesMin(0x0000);  // 0 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal
  bleGamepadConfig.setAxesMax(0x7FFF);  // 32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal
  bleGamepad.begin(&bleGamepadConfig);  // Simulation controls, special buttons and hats 2/3/4 are disabled by default

  // changing bleGamepadConfig after the begin function has no effect, unless you call the begin function again
}

void loop() {
  if (bleGamepad.isConnected()) {
    int potValues[numberOfPotSamples];  // Array to store pot readings
    int potAvgValue[4];                 // Variable to store calculated pot reading average

    // Populate readings
    for (int j = 0; j < 4; j++) {
      int potValue = 0;
      for (int i = 0; i < numberOfPotSamples; i++) {
        potValues[i] = analogRead(potPins[j]);
        potValue += potValues[i];
        delay(delayBetweenSamples);
      }

      // Calculate the average
      potAvgValue[j] = potValue / numberOfPotSamples;
    }
    // Map analog reading from 0 ~ 4095 to 32737 ~ 0 for use as an axis reading
    int adjustedx = map(potAvgValue[0], 0, 4095, 32737, 0);
    int adjustedy = map(potAvgValue[1], 0, 4095, 32737, 0);
    int adjustedrX = map(potAvgValue[2], 0, 4095, 32737, 0);
    int adjustedrY = map(potAvgValue[3], 0, 4095, 32737, 0);

    // Update X axis and auto-send report
    //bleGamepad.setX(adjustedx);
    // bleGamepad.setX(adjustedx);
    // bleGamepad.setY(adjustedy);
    // bleGamepad.setZ(adjustedrX);
    // //bleGamepad.setRX(adjustedrX);

    // //bleGamepad.setRY(adjustedrX);

    // bleGamepad.setZ(adjustedrY);
    // //bleGamepad.setRZ(adjustedrY);
    //bleGamepad.set
    bleGamepad.setAxes(adjustedx, adjustedy, adjustedrX, adjustedrY, false, false, false, false);



    read_buttons();
    delay(delayBetweenHIDReports);

    Serial.print(adjustedx);
    Serial.print(" ");
    Serial.print(adjustedy);
    Serial.print(" ");
    Serial.print(adjustedrX);
    Serial.print(" ");
    Serial.println(adjustedrY);

    bleGamepad.sendReport();
    //delay(100);
  }
}

void read_buttons() {
  for (byte currentIndex = 0; currentIndex < numOfButtons; currentIndex++) {
    currentButtonStates[currentIndex] = digitalRead(buttonPins[currentIndex]);

    if (currentButtonStates[currentIndex] != previousButtonStates[currentIndex]) {
      if (currentButtonStates[currentIndex] == LOW) {
        bleGamepad.press(physicalButtons[currentIndex]);
      } else {
        bleGamepad.release(physicalButtons[currentIndex]);
      }
    }
  }

  if (currentButtonStates != previousButtonStates) {
    for (byte currentIndex = 0; currentIndex < numOfButtons; currentIndex++) {
      previousButtonStates[currentIndex] = currentButtonStates[currentIndex];
    }

    bleGamepad.sendReport();
  }

  for (byte currentIndex = 0; currentIndex < numOfButtons; currentIndex++) {
    Serial.print(currentButtonStates[currentIndex]);
    Serial.print(" ");
  }


  // delay(20);
}