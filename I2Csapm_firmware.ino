/*  I2Csapm firmware
 *   I2C slave arduino pro micro
 *   firmware for Arduino pro micro 3.3V ATmega328p 8MHz I2C slave
 */

#include <Wire.h>
#define I2CADDR 2

#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'S'},
  {'4', '5', '6', 'G'},
  {'7', '8', '9', 'L'},
  {'*', '0', '#', 'P'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

 byte ordre = 0;

 const int led = 13;
 const int swtin = 12;

 int rot;
 int sw1;
 int ledout;
 int analin[6];

 int pinA = 3;  // Connected to CLK on KY-040
 int pinB = 4;  // Connected to DT on KY-040
 int encoderPosCount = 0; 
 int pinALast;  
 int aVal;
 boolean bCW;


void setup() {
  pinMode(led,OUTPUT);
  pinMode(swtin,INPUT_PULLUP);

  pinMode (pinA,INPUT);
  pinMode (pinB,INPUT);
  /* Read Pin A
  Whatever state it's in will reflect the last position   
  */
  pinALast = digitalRead(pinA);   

  Wire.begin(I2CADDR);
  Wire.onReceive(escolta);
  Wire.onRequest(respon);
}

void loop() {
  if (ledout==1) {
    digitalWrite(led,HIGH);
    }
    else {
      digitalWrite(led,LOW);
    }
  if (digitalRead(swtin)==1) {
    sw1=1;
    }
    else {
     sw1=0;
    }
    analin[0]=analogRead(A0);
    analin[1]=analogRead(A1);
    analin[2]=analogRead(A2);
    analin[3]=analogRead(A3);
    analin[4]=analogRead(A6);
    analin[5]=analogRead(A7);
    
    aVal = digitalRead(pinA);
    if (aVal != pinALast) { // Means the knob is rotating
     // if the knob is rotating, we need to determine direction
     // We do that by reading pin B.
     if (digitalRead(pinB) != aVal) {  // Means pin A Changed first - We're Rotating Clockwise
       encoderPosCount ++;
       bCW = true;
     }
     else {       // Otherwise B changed first and we're moving CCW
       bCW = false;
       encoderPosCount--;
     }
     if (bCW){
       rot=1;
     }
     else {
       rot=-1;
     }     
   } 
   else {
       rot=0;
   }
   pinALast = aVal;
}

void escolta(int ordrelen) {
  ordre = Wire.read();
  switch(ordre) {
    case 1:
      ledout=1;
      break;
    case 2:
      ledout=0;
      break;
  }

}

void respon() {
  int resposta;
  switch(ordre) {
    case 0:
      resposta = analin[0];
      break;
    case 1:
      resposta = analin[1];
      break;
    case 2:
      resposta = analin[2];
      break;
    case 3:
      resposta = analin[3];
      break;
    case 4:
      resposta = analin[4];
      break;
    case 5:
      resposta = analin[5];
      break;
    case 6:
      resposta = sw1;
      break;
    case 7:
      resposta = rot;
      break;
    case 8:
      resposta = encoderPosCount;
      break;
    case 9:
      resposta = keypad.getKey();
      break;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
      resposta = 0;
      break;
    default:
      resposta = -1;
      break;
  }
  byte buffer[2];
  buffer[0]=resposta >> 8;
  buffer[1]=resposta & 0xff;
  Wire.write(buffer,2);
  ordre=0;
}

