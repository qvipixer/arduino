//Pin connected to Strobe (pin 1) of 4094
int strobePin = A3;
//Pin connected to Data (pin 2) of 4094
int dataPin = A4;
//Pin connected to Clock (pin 3) of 4094
int clockPin = A5;
//Pin connected to OE (pin 15) of 4094
int oePin = A2;

/* Decimal representation of binary numbers for each
  digit in the 7 segment display from 0 to 9

*/
byte segChar[] = {4, 8, 16, 32,
                  64, 128, 256, 512,
                  1024, 2048, 4096, 8192,
                  16384, 32768, 65536, 131072
                 };
int t = 25;

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(oePin, OUTPUT);
  digitalWrite(oePin, HIGH);
  Serial.begin(115200);
}

void loop() {

  /* for (int j = 0; j < 99999999; j += 16) {
     byte qw = 4; // int 4 blue, 8 green
     Serial.println(j);
     //устанавливаем LOW на latchPin пока не окончена передача байта
     digitalWrite(strobePin, LOW);
     shiftOut(dataPin, clockPin, LSBFIRST, j);
     digitalWrite(strobePin, HIGH);
     delay(100);
    }
  */

  for (int i = 0; i < 13; i++) {
    //set strobe pin low to begin storing bits
    digitalWrite(strobePin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, segChar[i]);
    //set strobe pin high to stop storing bits
    digitalWrite(strobePin, HIGH);
    delay(t);
  }
  for (int i = 13; i > 0; i--) {
    //set strobe pin low to begin storing bits
    digitalWrite(strobePin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, segChar[i]);
    //set strobe pin high to stop storing bits
    digitalWrite(strobePin, HIGH);
    delay(t);
  }
}
