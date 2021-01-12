/*
   IRremoteESP8266: IRrecvDemo - demonstrates receiving IR codes with IRrecv
   This is very simple teaching code to show you how to use the library.
   If you are trying to decode your Infra-Red remote(s) for later replay,
   use the IRrecvDumpV2.ino (or later) example code instead of this.
   An IR detector/demodulator must be connected to the input kRecvPin.
   Copyright 2009 Ken Shirriff, http://arcfn.com
   Example circuit diagram:
    https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-receiving
   Changes:
     Version 0.2 June, 2017
       Changed GPIO pin to the same as other examples.
       Used our own method for printing a uint64_t.
       Changed the baud rate to 115200.
     Version 0.1 Sept, 2015
       Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009
*/
int codeLed [] = {0xF720DF, 0xF7A05F, 0xF7609F, 0xF7E01F, 0xF740BF, 0xF7C03F, 0xF700FF, 0xF7807F}; // RED, GREEN, BLUE, WHITE, OFF, ON, +, -

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include <IRsend.h>
#include <ir_Trotec.h>

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRTrotecESP ac(kIrLed);  // Set the GPIO to be used for sending messages.
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
const uint16_t kRecvPin = 14;

IRrecv irrecv(kRecvPin);

decode_results results;

void setup() {
  ac.begin();
  Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
}

void loop() {
  /*
    irsend.sendNEC(0xF7C03F);
    irsend.sendNEC(0xF720DF);
    delay(2000);
    irsend.sendNEC(0xF740BF);
    delay(2000);
    for (int i = 4; i <= 5; i ++) {
    irsend.sendNEC(codeLed[i]);
    delay(10);
    }
    for (int i = 1; i >= 0; i--) {
    irsend.sendNEC(codeLed[i]);
    delay(1);
    }*/
  /*
    0xF720DF // RED
    0xF7A05F // GREEN
    0xF7E01F // WHITE
    0xF7609F // BLUE
    0xF740BF // OFF
    0xF7C03F // ON
    0xF700FF // +
    0xF7807F // -
  */


  if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
  delay(1000);

}
