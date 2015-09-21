// adapted sketch by niq_ro from http://arduinotehniq.blogspot.com
// and http://nicuflorica.blogspot.ro
// base version 1.0 in 6.11.2014, Craiova - Romanaia
// actul version 3.0 in 22.05.2015
// source for LEDControl: http://embedded-lab.com/blog/?p=6862
#include "LedControl.h" 
/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(12,11,10,1);

// ideea from: http://arduinoprojects.ru/2014/08/%D0%BF%D1%80%D0%BE%D1%81%D1%82%D0%BE%D0%B9-%D1%82%D0%B5%D1%80%D0%BC%D0%BE%D1%81%D1%82%D0%B0%D1%82-%D0%BD%D0%B0-arduino-%D0%B8-%D1%86%D0%B8%D1%84%D1%80%D0%BE%D0%B2%D0%BE%D0%BC-%D1%82%D0%B5%D1%80%D0%BC/
// for DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// declaration for type of value
float t, h, t1;

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
// original sketck from http://learn.adafruit.com/ds1307-real-time-clock-breakout-board-kit/
// add part with SQW=1Hz from http://tronixstuff.wordpress.com/2010/10/20/tutorial-arduino-and-the-i2c-bus/

// Dawn & Dusk controller. http://andydoz.blogspot.ro/2014_08_01_archive.html
// 16th August 2014 - (C) A.G.Doswell 2014

#include <Wire.h>
#include "RTClib.h" // from https://github.com/adafruit/RTClib
#include <Encoder.h> // from http://www.pjrc.com/teensy/td_libs_Encoder.html

RTC_DS1307 RTC; // Tells the RTC library that we're using a DS1307 RTC
Encoder knob(2, 3); //encoder connected to pins 2 and 3 (and ground)

//the variables provide the holding values for the set clock routine
int setyeartemp; 
int setmonthtemp;
int setdaytemp;
int sethourstemp;
int setminstemp;
int setsecs = 0;
int maxday; // maximum number of days in the given month
int TimeOut = 10;
int TimeOutCounter;

// These variables are for the push button routine
int buttonstate = 0; //flag to see if the button has been pressed, used internal on the subroutine only
int pushlengthset = 2000; // value for a long push in mS
int pushlength = pushlengthset; // set default pushlength
int pushstart = 0;// sets default push value for the button going low
int pushstop = 0;// sets the default value for when the button goes back high

int knobval; // value for the rotation of the knob
boolean buttonflag = false; // default value for the button flag



void setup()
{
// Initialize MAX7219 device
lc.shutdown(0,false); // Enable display 
lc.setIntensity(0,8); // Set brightness level (0 is min, 15 is max) 
lc.clearDisplay(0); // Clear display register 
 
// Initialize DS18B20 sensor
sensors.begin(); 

    Wire.begin();
    RTC.begin();
  if (! RTC.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
 
// part code from http://tronixstuff.wordpress.com/
Wire.beginTransmission(0x68);
Wire.write(0x07); // move pointer to SQW address
//Wire.write(0x10); // sends 0x10 (hex) 00010000 (binary) to control register - turns on square wave
Wire.write(0x13); // sends 0x13 (hex) 00010011 (binary) to control register - turns on square wave at 32kHz
Wire.endTransmission();
// end part code from http://tronixstuff.wordpress.com/

pinMode(A0,INPUT);//push button on encoder connected to A0 (and GND)
digitalWrite(A0,HIGH); //Pull A0 high
}

void loop()
{

sensors.requestTemperatures(); // запрос на получение температуры
t=(sensors.getTempCByIndex(0)); 
t = 10*t; // for see value after comma

lc.clearDisplay(0); // Clear display register 
// temperatue on display
for(int j=0; j<300; j++){
temperatura (t);
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    delay (10);
    if (pushlength <pushlengthset) {
      ShortPush ();   
    }
       //This runs the setclock routine if the knob is pushed for a long time
       if (pushlength >pushlengthset) {
       lc.clearDisplay(0); // Clear display register 
         DateTime now = RTC.now();
         setyeartemp=now.year(),DEC;
         setmonthtemp=now.month(),DEC;
         setdaytemp=now.day(),DEC;
         sethourstemp=now.hour(),DEC;
         setminstemp=now.minute(),DEC;
         setclock();
         pushlength = pushlengthset;
       };
}

lc.clearDisplay(0); // Clear display register 

// time on display
for(int j=0; j<8; j++){
DateTime now = RTC.now();
int ora0 = now.hour();
int minut0 = now.minute();
int second0 = now.second();
ora (ora0, minut0, second0, j);
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    delay (10);
    if (pushlength <pushlengthset) {
      ShortPush ();   
    }
       //This runs the setclock routine if the knob is pushed for a long time
       if (pushlength >pushlengthset) {
         lc.clearDisplay(0); // Clear display register 
         DateTime now = RTC.now();
         setyeartemp=now.year(),DEC;
         setmonthtemp=now.month(),DEC;
         setdaytemp=now.day(),DEC;
         sethourstemp=now.hour(),DEC;
         setminstemp=now.minute(),DEC;
         setclock();
         pushlength = pushlengthset;
       };
delay (500);
}

}

void temperatura (int temper)
{
int st = int(temper/100); // determin cifra sutelor
temper = temper - 100*st; // restul numarului de la zeci si unitati 
int zt = int(temper/10); // determin cifra zecilor
int ut = temper - 10*zt; // determin cifra unitatilor
  
lc.setDigit(0,3,st, false); // afisez un 5 pe coloana 7
lc.setDigit(0,2,zt, true); // afisez un 0 pe coloana 1 si virgula
lc.setDigit(0,1,ut, false); // afisez un 5 pe coloana 7
//lc.setRow(0,5,B1100011); // afisez un semn de grad pe coloana 5
lc.setRow(0,0,B1001110); // afisez un C pe coloana 4
}


void ora (int ora1, int minut1, int second1, int k) 
{
// hour
int zo = int(ora1/10); // determin cifra zecilor
int uo = ora1 - 10*zo; // determin cifra unitatilor
if (zo >= 1) lc.setDigit(0,3,zo, false); // afisez zecile de ore pe coloana 7 (in stanga)
//lc.setDigit(0,2,uo, false); // afisez unitatile de ore pe coloana 6
//lc.setDigit(0,2,uo, true); // afisez unitatile de ore pe coloana 6

int k1 = k/2;
if (k == 2*k1 ) lc.setDigit(0,2,uo, false);
else lc.setDigit(0,2,uo, true);

// minute
int zm = int(minut1/10); // determin cifra zecilor
int um = minut1 - 10*zm; // determin cifra unitatilor
lc.setDigit(0,1,zm, false); // afisez zecile de minute pe coloana 4
lc.setDigit(0,0,um, false); // afisez unitatile de minute pe coloana 3
}

void data (int ziua1, int luna1, int anul1) 
{
// ziua (day)
int zz = int(ziua1/10); // determin cifra zecilor
int uz = ziua1 - 10*zz; // determin cifra unitatilor
lc.clearDisplay(0); // Clear display register
if (zz >= 1) lc.setDigit(0,3,zz, false); // afisez zecile de ore pe coloana 7 (in stanga)
lc.setDigit(0,2,uz, true); // afisez unitatile de ore pe coloana 6

// luna (mounth)
int zl = int(luna1/10); // determin cifra zecilor
int ul = luna1 - 10*zl; // determin cifra unitatilor
lc.setDigit(0,1,zl, false); // afisez zecile de minute pe coloana 5
lc.setDigit(0,0,ul, true); // afisez unitatile de minute pe coloana 4
//lc.setRow(0,2,B0000001);  // afisez o liniuta pe coloana 2

delay(1000);
lc.clearDisplay(0); // Clear display register
// anul (year)
int ma = int(anul1/1000); // determin cifra miilor
int rsa = anul1 - 1000*ma; // determin restul sutelor
int sa = int(rsa/100); // determin cifra sutelor
int rza = rsa - 100*sa; // determin restul zecilor
int za = int(rza/10); // determin cifra zecilor
int ua = rza - 10*za; // determin cifra anilor

lc.setDigit(0,3,ma, false); // afisez zecile de secunde pe coloana 3
lc.setDigit(0,2,sa, false); // afisez unitatile de minute pe coloana 2  
lc.setDigit(0,1,za, false); // afisez zecile de secunde pe coloana 1
lc.setDigit(0,0,ua, false); // afisez unitatile de minute pe coloana 0 (dreapta) 

delay(1000);
}

//sets the clock
void setclock (){
   setyear ();
lc.clearDisplay(0); // Clear display register 
   setmonth ();
lc.clearDisplay(0); // Clear display register 
   setday ();
lc.clearDisplay(0); // Clear display register 
   sethours ();
lc.clearDisplay(0); // Clear display register 
   setmins ();
lc.clearDisplay(0); // Clear display register 
   
   RTC.adjust(DateTime(setyeartemp,setmonthtemp,setdaytemp,sethourstemp,setminstemp,setsecs));
   delay (1000);
}

// subroutine to return the length of the button push.
int getpushlength () {
  buttonstate = digitalRead(A0);  
       if(buttonstate == LOW && buttonflag==false) {     
              pushstart = millis();
              buttonflag = true;
          };
          
       if (buttonstate == HIGH && buttonflag==true) {
         pushstop = millis ();
         pushlength = pushstop - pushstart;
         buttonflag = false;
       };
       return pushlength;
}
// The following subroutines set the individual clock parameters
int setyear () {
lc.setRow(0,3,B00111011);  // afisez litera "y" pe coloane 7 (in stanga de tot)
lc.setRow(0,2,B11101111);  // afisez litera "e" pe coloane 6
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setyeartemp;
    }
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) { //bit of software de-bounce
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setyeartemp=setyeartemp + knobval;
    if (setyeartemp < 2014) { //Year can't be older than currently, it's not a time machine.
      setyeartemp = 2014;
    }
int zua = setyeartemp - 2000;  // aflu ultimele 2 cifre din an
int za = zua / 10;  // aflu cifra zecilor din an
int ua = zua - 10*za; // aflu cifra unitatilor din an
lc.setDigit(0,1,za, false); // afisez cifra zecilor din an pe coloana 1
lc.setDigit(0,0,ua, false); // afisez cifra unitatilor din an pe coloana 0 (dreapta de tot)
    setyear();
}

  
int setmonth () {
lc.setRow(0,3,B00110111);  // afisez litera "H" (in loc de "m") pe coloane 7 (in stanga de tot)
lc.setRow(0,2,B10011101);  // afisez litera "o" pe coloane 6
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setmonthtemp;
    }
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setmonthtemp=setmonthtemp + knobval;
    if (setmonthtemp < 1) {// month must be between 1 and 12
      setmonthtemp = 1;
    }
    if (setmonthtemp > 12) {
      setmonthtemp=12;
    }
int zl = setmonthtemp / 10;  // aflu cifra zecilor din luna
int ul = setmonthtemp - 10*zl; // aflu cifra unitatilor din luna
lc.setDigit(0,1,zl, false); // afisez cifra zecilor din luna pe coloana 1
lc.setDigit(0,0,ul, false); // afisez cifra unitatilor din luna pe coloana 0 (dreapta de tot)
     setmonth();
}

int setday () {
  if (setmonthtemp == 4 || setmonthtemp == 5 || setmonthtemp == 9 || setmonthtemp == 11) { //30 days hath September, April June and November
    maxday = 30;
  }
  else {
  maxday = 31; //... all the others have 31
  }
  if (setmonthtemp ==2 && setyeartemp % 4 ==0) { //... Except February alone, and that has 28 days clear, and 29 in a leap year.
    maxday = 29;
  }
  if (setmonthtemp ==2 && setyeartemp % 4 !=0) {
    maxday = 28;
  }
lc.setRow(0,3,B00111101);  // afisez litera "d" pe coloane 7
lc.setRow(0,2,B11111101);  // afisez litera "a" pe coloane 6
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setdaytemp;
    }
     knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setdaytemp=setdaytemp+ knobval;
    if (setdaytemp < 1) {
      setdaytemp = 1;
    }
    if (setdaytemp > maxday) {
      setdaytemp = maxday;
    }
int zz = setdaytemp / 10;  // aflu cifra zecilor din zi
int uz = setdaytemp - 10*zz; // aflu cifra unitatilor din zi
lc.setDigit(0,1,zz, false); // afisez cifra zecilor din zi pe coloana 1
lc.setDigit(0,0,uz, false); // afisez cifra unitatilor din zi pe coloana 0 (dreapta de tot)
    setday();
}

int sethours () {
lc.setRow(0,3,B00010111);  // afisez litera "h" pe coloane 7
lc.setRow(0,2,B10011101);  // afisez litera "o" pe coloane 6
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return sethourstemp;
    }
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    sethourstemp=sethourstemp + knobval;
    if (sethourstemp < 1) {
      sethourstemp = 1;
    }
    if (sethourstemp > 23) {
      sethourstemp=23;
    }
int zo = sethourstemp / 10;  // aflu cifra zecilor de ore
int uo = sethourstemp - 10*zo; // aflu cifra unitatilor de ore
lc.setDigit(0,1,zo, false); // afisez cifra zecilor de ora pe coloana 1
lc.setDigit(0,0,uo, false); // afisez cifra unitatilor de ora pe coloana 0 (dreapta de tot)
    sethours();
}

int setmins () {
lc.setRow(0,3,B00110111);  // afisez litera "H" (in loc de "m") pe coloane 7 (in stanga de tot)
lc.setRow(0,2,B10010000);  // afisez litera "i" pe coloane 6
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setminstemp;
    }
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setminstemp=setminstemp + knobval;
    if (setminstemp < 0) {
      setminstemp = 0;
    }
    if (setminstemp > 59) {
      setminstemp=59;
    }
int zm = setminstemp / 10;  // aflu cifra zecilor de minute
int um = setminstemp - 10*zm; // aflu cifra unitatilor de minute
lc.setDigit(0,1,zm, false); // afisez cifra zecilor de minute pe coloana 1
lc.setDigit(0,0,um, false); // afisez cifra unitatilor de minute pe coloana 0 (dreapta de tot)
    setmins();
}


void ShortPush () {
  //This displays show something
lc.clearDisplay(0); // Clear display register
//for (long Counter = 0; Counter < 10 ; Counter ++) { //returns to the main loop if it's been run 604 times 
/*                                                 
//  lcd.setCursor (2,0);
//  lcd.print ("Hi, niq_ro!");
lc.setRow(0,6,B1100010);  // afisez litera "n" pe coloane 6
lc.setRow(0,5,B0100000);  // afisez litera "i" pe coloane 5
lc.setRow(0,4,B1110011);  // afisez litera "q" pe coloane 4
lc.setRow(0,3,B0000001);  // afisez semnul "-" pe coloane 3
lc.setRow(0,2,B1000010);  // afisez litera "r" pe coloane 2
lc.setRow(0,1,B1100011);  // afisez litera "o" pe coloane 1
delay(200);
*/

lc.clearDisplay(0); // Clear display register 
for(int j=0; j<2 ; j++){
DateTime now = RTC.now();
int ziua0 = now.day();
int luna0 = now.month();
int anul0 = now.year();
data (ziua0, luna0, anul0);

    pushlength = pushlengthset;
    pushlength = getpushlength ();
    delay (10);
    if (pushlength <pushlengthset) {
      ShortPush ();   
    }
       //This runs the setclock routine if the knob is pushed for a long time
 delay (750);
}

lc.clearDisplay(0); // Clear display register
}
