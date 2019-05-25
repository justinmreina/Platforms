/************************************************************************************************************************************/
/** @file     esp32_gpio.c
 *  @brief    x
 *  @details  x
 *
 *  @target     ESP32
 *  @board      HUZZAH32 / 
 *
 *  @author     Justin Reina, Firmware Engineer, Misc. Company
 *  @created    6/19/17
 *  @last rev   6/19/17
 *
 *  @ref   https://learn.sparkfun.com/tutorials/esp32-thing-hookup-guide#arduino-example-blink
 * 
 *  @rslt  1 Hz square wave on IO21 with sreial output @ 115200 Baud
 * 
 *  @ref   https://www.arduino.cc/en/Tutorial/StringToIntExample (Sprintf)
 *  @ref   https://github.com/espressif/esptool/issues/198 (Failed to Connect Resolve)
 * 
 *  @section   Pinout (IO21 - pin#33)
 *    [0] - JP1.1 (GND - JP1.13)    <- [PASS]
 *    [1] - JP1.1 (GND - JP1.13)    <- [PASS]
 *    [2]
 *    [3]
 *
 *  @section  Legal Disclaimer
 *      All contents of this source file and/or any other Misc. Product related source files are the explicit property of
 *      Misc. Company. Do not distribute. Do not copy.
 */
/************************************************************************************************************************************/
#include "D:/Documents/Arduino/hardware/espressif/esp32/tools/sdk/include/driver/driver/gpio.h"

int loopCt = 0;

//#IO21, pin #33
int pinGpioNum = 21;


void setup() {

  //init pin
  pinMode(pinGpioNum, OUTPUT);

  //Serial (for demo)
  Serial.begin(115200);
  
  return;
}


void loop() {
  
    //On
    digitalWrite(pinGpioNum, HIGH);
    delay(500);

    //Off
    digitalWrite(pinGpioNum, LOW);
    delay(500);

    //Update Serial Report
    Serial.print("Hello, world, loopCt:");
    Serial.println(loopCt++);

    return;
}


