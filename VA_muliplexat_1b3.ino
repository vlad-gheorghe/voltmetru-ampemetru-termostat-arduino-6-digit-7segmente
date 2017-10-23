/*
 base sketch: https://cdn.sparkfun.com/datasheets/Components/LED/_7Seg_Example.pde
  4 digit 7 segment display: http://www.sparkfun.com/products/9483 
 7 segments + 4 digits + 1 colon = 12 pins required for full control 
modified connexion by Nicu FLORICA (niq_ro) from http://nicuflorica.blogspot.com
http://arduinotehniq.blogspot.com
http://www.tehnic.go.ro
http://www.arduinotehniq.com/
ver.0 - Volt and ammeter with 2x3 digits, 22.10.2017 (just simulate value)
ver.1 - put voltage, current and temperature measurement 
*/

#define digit1 11 //PWM pin for control digit 1 (left side)
#define digit2 10 //PWM pin for control digit 2
#define digit3 9  //PWM pin for control digit 3
#define digit4 6  //PWM pin for control digit 4
#define digit5 5  //PWM pin for control digit 5
#define digit6 3  //PWM pin for control digit 6 (right side)

#define segA 2 // pin for control "a" segment
#define segB A5 // pin for control "b" segment (19 = A5)
#define segC 4 // pin for control "c" segment
#define segD A4 // pin for control "d" segment (18 = A4)
#define segE 12 // pin for control e" segment
#define segF 7 // pin for control "f" segment
#define segG 8 // pin for control "g" segment
#define segDP 13 // pin for control decimal point

#define DIODA A2
#define pintensiune A1
#define pincurent A0
#define releu A3
/*
https://www.hackster.io/microst/thermometer-diode-based-524613
Thermometer based on 1n4148 silicon diode used as temperature sensor.The thermometer is based on the diode characteristic that the increase of the temperature its forward voltage (VF) is lowered by 2,2mV / ° C.
Fixing the value of Vf = VF0 at ambient temperature t0, the temperature value
t is calculated with the following formula:
t= t0 - [vf(t)- vf0]* K
with K = 1 / 2,2mV
The value of Vf (t) = dtemp -vf0 is obtained by averaging values of 1024 by acquiring as many vf values
*/


int DISPLAY_BRIGHTNESS = 500;
//Display brightness
//Each digit is on for a certain amount of microseconds
//Then it is off until we have reached a total of 20ms for the function call
//Let's assume each digit is on for 1000us
//Each digit is on for 1ms, there are 4 digits, so the display is off for 16ms.
//That's a ratio of 1ms to 16ms or 6.25% on time (PWM).
//Let's define a variable called brightness that varies from:
//5000 blindingly bright (15.7mA current draw per digit)
//2000 shockingly bright (11.4mA current draw per digit)
//1000 pretty bright (5.9mA)
//500 normal (3mA)
//200 dim but readable (1.4mA)
//50 dim but readable (0.56mA)
//5 dim but readable (0.31mA)
//1 dim but readable in dark (0.28mA)
/*
//for common catode
#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW
#define SEGMENT_ON  LOW
#define SEGMENT_OFF HIGH
*/

// for common anode
#define DIGIT_ON  LOW
#define DIGIT_OFF  HIGH
#define SEGMENT_ON  HIGH
#define SEGMENT_OFF LOW


int maxbright = 1023; // 1023+7
int lene = 0;
int cicluri = 100; // if maxbright ~1000, cicluri = 125, if maxbright ~500 cicluri = 250;

float tensiune = 0;
float curent = 0;
int x = 0;

const int t0 = 20.3;
const float vf0 = 573.44;
double dtemp, dtemp_avg, te;
double voltageref = 1090;  // ideal value
float corectie1 = 1;  // correction factor for reading when is use multiplexing method and other causes
float corectie2 = 1.04;  // correction factor for reading voltage when is use multiplexing method and other causes
float corectie3 = 1.0; // correction factor for current
float t1;  
int measurements = 10;
int tmax = 50;
int tmin = 40;

float R1 = 1.0;
float R2 = 68.;
float shunt = 0.33;

unsigned long timp = 0;
unsigned long cattimp = 1000;

void setup() {
 Serial.begin(9600);
 Serial.println("volt- & ammeter by niq_ro");
 
 analogReference(INTERNAL);  // http://tronixstuff.com/2013/12/12/arduino-tutorials-chapter-22-aref-pin/
 pinMode(DIODA, INPUT_PULLUP);            // set the  pin IN with npull up to bias the diode
 
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);
  pinMode(segDP, OUTPUT);

  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);
  pinMode(digit5, OUTPUT);
  pinMode(digit6, OUTPUT);

  pinMode(releu, OUTPUT); 
  digitalWrite(releu, LOW); 

if (DISPLAY_BRIGHTNESS > 1020) DISPLAY_BRIGHTNESS = 1000;
}

void loop() {

dtemp_avg = 0;
tensiune = 0;
curent = 0;
  for (int i = 0; i < measurements; i++) {
    float vf = analogRead(DIODA) * (voltageref / 1023.00);
    //Serial.println(vf);
    dtemp = (vf - vf0) * 0.4545454;
    dtemp_avg = dtemp_avg + dtemp;
 //  delay(1);
tensiune = tensiune + analogRead(pintensiune);
curent = curent + analogRead(pincurent);
  }
  te = t0 - dtemp_avg / measurements;
  te = te * corectie1 + 0.5;
  tensiune = tensiune / measurements;
  curent = curent / measurements;
Serial.print("temp = ");
Serial.println(te);
if (te > tmax)
{
  digitalWrite(releu, HIGH);
Serial.println("ventilator pornit");
}
if (te < tmin)
{
  digitalWrite(releu, LOW);
Serial.println("ventilator oprit");
}  
//tensiune = analogRead(pintensiune);
Serial.print("treapta tensiune = ");
Serial.print(tensiune);
Serial.println("/1024 ");
tensiune = corectie2 * tensiune * (voltageref / 1023.00);
if (tensiune < 0) tensiune = 0;
//curent = analogRead(pincurent);
curent = corectie3 * curent * (voltageref / 1023.00);
Serial.print("treapta curent = ");
Serial.print(curent);
Serial.println("/1024 ");
if (curent < 0) curent = 0;

tensiune = tensiune * (R2+R1)/R1;
tensiune = tensiune - curent;
tensiune = tensiune /100;
Serial.print("Tensiune = ");
Serial.print(tensiune/10.);
Serial.print("V    ");
curent = curent / shunt;
curent = curent /10;
Serial.print("Curent = ");
Serial.print(curent/100.);
Serial.println("A "); 
//tensiune = random (0,999);
//curent = random (0,999);

for (int i = 0; i < cicluri; i++) {
displayNumber1(tensiune);
if (curent == 0.) displayNumber3(te);
else
displayNumber2(curent);
}

} // end main program


//Given a number, turns on those segments
//If number == 10, then turn off number
void lightNumber(int numberToDisplay) {
  switch (numberToDisplay){

  case 0:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 1:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 2:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 3:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 4:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 5:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 6:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 7:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 8:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 9:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  // all segment are ON
  case 10:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

    // degree symbol made by niq_ro
  case 11:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  // C letter made by niq_ro
  case 12:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    break;
  
  // H letter made by niq_ro
  case 13:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;
 
  // L letter made by niq_ro
  case 14:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  // A letter made by niq_ro
  case 15:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;
  
    // F letter made by niq_ro
  case 16:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;
    
  // n letter made by niq_ro
  case 17:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

    // o letter made by niq_ro
  case 18:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;
 
 // "Y" letter made by niq_ro
  case 19:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  // "r" letter made by niq_ro
  case 20:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  // "d" letter made by niq_ro
  case 21:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;
 }
}


// display number 
void displayNumber1(int toDisplay) {
  for(int digit = 3 ; digit > 0 ; digit--) {
    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      digitalWrite(segDP, SEGMENT_OFF);
  //    if (mii == 0) lightNumber(10);
  //    else
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
   case 2:
      digitalWrite(digit2, DIGIT_ON);
      digitalWrite(segDP, SEGMENT_ON);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
      digitalWrite(segDP, SEGMENT_OFF);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
    }
     //Turn off all segments
    lightNumber(10); 
    digitalWrite(segDP, SEGMENT_OFF);
    lene = maxbright - DISPLAY_BRIGHTNESS;
    delayMicroseconds(lene); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
    digitalWrite(digit5, DIGIT_OFF);
    digitalWrite(digit6, DIGIT_OFF);   
    digitalWrite(segDP, SEGMENT_OFF);
}
} 

void displayNumber2(int toDisplay) {
  for(int digit = 6 ; digit > 3 ; digit--) {
    //Turn on a digit for a short amount of time
    switch(digit) {
    case 4:
      digitalWrite(digit4, DIGIT_ON);
      digitalWrite(segDP, SEGMENT_ON);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      digitalWrite(segDP, SEGMENT_OFF);
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
    case 5:
      digitalWrite(digit5, DIGIT_ON);
      digitalWrite(segDP, SEGMENT_OFF);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      digitalWrite(segDP, SEGMENT_OFF);
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break; 
     case 6:
      digitalWrite(digit6, DIGIT_ON);
      digitalWrite(segDP, SEGMENT_OFF);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      digitalWrite(segDP, SEGMENT_OFF);
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
    }
     //Turn off all segments
    lightNumber(10); 
    digitalWrite(segDP, SEGMENT_OFF);
    lene = maxbright - DISPLAY_BRIGHTNESS;
    delayMicroseconds(lene); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
    digitalWrite(digit5, DIGIT_OFF);
    digitalWrite(digit6, DIGIT_OFF);   
    digitalWrite(segDP, SEGMENT_OFF);
}
} 

void displayNumber3(int toDisplay) {
  for(int digit = 6 ; digit > 3 ; digit--) {
    //Turn on a digit for a short amount of time
    switch(digit) {
    case 4:
      digitalWrite(digit4, DIGIT_ON);
      digitalWrite(segDP, SEGMENT_OFF);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      digitalWrite(segDP, SEGMENT_OFF);
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
    case 5:
      digitalWrite(digit5, DIGIT_ON);
      digitalWrite(segDP, SEGMENT_ON);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      digitalWrite(segDP, SEGMENT_OFF);
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break; 
     case 6:
      digitalWrite(digit6, DIGIT_ON);
      digitalWrite(segDP, SEGMENT_OFF);
      lightNumber(12);
      digitalWrite(segDP, SEGMENT_OFF);
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
    }
     //Turn off all segments
    lightNumber(10); 
    digitalWrite(segDP, SEGMENT_OFF);
    lene = maxbright - DISPLAY_BRIGHTNESS;
    delayMicroseconds(lene); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
    digitalWrite(digit5, DIGIT_OFF);
    digitalWrite(digit6, DIGIT_OFF);   
    digitalWrite(segDP, SEGMENT_OFF);
}
} 


