/*
  anschluss relais/optokoppler:
  COM2 = 2 ("relaypin")
  vcc=3.3V
  GND=GND
  
  kontakt=diode an = kontakt mitte auf aussen(bei "2")
  
  http://physicalcomputing.at/page/11
  taster/teiler = analog 1
  R 1k 5V auf a1
  R_NTC GND auf a1
  
  
  LCD = d4 bis 9 sowie 10helligkeit
  Taster = a0
  
  nix= 1023
  select=722/721
  left= 482/481
  down=307/308
  up= 131/132
  right=0
  
  
    
  rausführen also
  GND ("doppelt")
  3.3V
  d3 (relais)
  a1
  
 */
 
 
 /********************************************************
 * PID RelayOutput Example
 * Same as basic example, except that this time, the output
 * is going to a digital pin which (we presume) is controlling
 * a relay.  the pid is designed to Output an analog value,
 * but the relay can only be On/Off.
 *
 *   to connect them together we use "time proportioning
 * control"  it's essentially a really slow version of PWM.
 * first we decide on a window size (5000mS say.) we then 
 * set the pid to adjust its output between 0 and that window
 * size.  lastly, we add some logic that translates the PID
 * output into "Relay On Time" with the remainder of the 
 * window being "Relay Off Time"
 ********************************************************/

#include <PID_v1.h>
#include <LiquidCrystal.h>
#include <math.h> 

#define RelayPin 2

//PID: Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,2,5,1, DIRECT);

//LCD init
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Parameter NTC-Sensor
int SensorPin = 1; // Für den Senoreingang wird Analog 0 gewählt 
float sensorWert = 0; // Variable, die den Sensor Wert annimmt 
float u1 =0; // Spannung u1 am Spannungsteiler 
float u2 = 0; // Spannung u2 am Spannungsteiler 
float i = 0; // Strom in A 
float Rntc = 0; // Widerstand des Thermistors zum Zeitpunkt der Messung 
float T = 0; // Variable für gemessene Temperatur 
float B = 3982; // Wert aus Datenblatt des Thermistors 
float Tn = 298.16; // Nenntemperatur in K 
float R25 = 2250; // Nennwiderstand in Ohm bei Nenntemperatur (aus Datenblatt) 

int taster = 1023;


int WindowSize = 5000;
unsigned long windowStartTime;

void setup()
{
  // LCD Groese festlegen
  lcd.begin(16, 2);
  // LCD Helligkeit einstellen
  analogWrite (10, 60);
  
  
  
  
  windowStartTime = millis();
  
  
  // Sollwert setzen
  //initialize the variables we're linked to
  Setpoint = 25;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  
  pinMode(RelayPin, OUTPUT); 
  
  
}


void loop()
{


// NTC lesen und Temp errechnen
sensorWert = analogRead(SensorPin); // Wert am Sensor wird gelesen 
u2 = (sensorWert * 5)/1024; // Spannung am Sensor wird berechnet 
u1 = 5-u2; // Spannung am Vorwiderstand wird berechnet 
i = u1/1000; // Strom wird berechnet 
Rntc = u2/i; // Widerstand des Thermistors zum Zeitpunkt der Messung 
T = (B*Tn)/(B+(log(Rntc/R25)*Tn)); // Berechnung der Temperatur 
T = T-273.15; // Umrechnung von K in °C 
  
  
  
  Input = T;
  
  myPID.Compute();

  /************************************************
   * turn the output pin on/off based on pid output
   ************************************************/
  unsigned long now = millis();
  if(now - windowStartTime>WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if(Output > now - windowStartTime) digitalWrite(RelayPin,HIGH);
  else digitalWrite(RelayPin,LOW);

 //taster einlesen
 taster = analogRead(0);
 
 int pressed = map(taster, 0, 1023, 8, 0);

 
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print(T);
 lcd.print("|");
 
 lcd.print(Setpoint);
 
 lcd.setCursor(0, 1);
 lcd.print(Output);

 lcd.print("|");
 lcd.print(sensorWert);
 
 lcd.print("|");
 lcd.print(pressed);



 switch( pressed )
 {
  case  5: //left -2grad
  Setpoint = Setpoint -2;
  break;
case  7: //up +0,1grad
  Setpoint = Setpoint +0.1;
  break;
case  6: //down 
  Setpoint = Setpoint -0.1;
  break;
case  8: //right
  Setpoint = Setpoint +2;
  break;
// select = case 3

 }
 delay(500);
 
}



 
/* 
 

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(200);               // wait for a second
}

*/



