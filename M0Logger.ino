#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "Adafruit_MPRLS.h"
#include "Adafruit_MAX31855.h"

#define TEST 0 //set to 1 to wait for serial console before starting to log data
//Definitions from dependencies
//MPRLS
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin

//ADXL377
#define sampleSize 10

//MAX31855 
#define MAXDO   12
#define MAXCS   11
#define MAXCLK  10

// Example creating a thermocouple instance with hardware SPI
// on a given CS pin.
//#define MAXCS   10
//Adafruit_MAX31855 thermocouple(MAXCS);

// Example creating a thermocouple instance with hardware SPI
// on SPI1 using specified CS pin.
//#define MAXCS   10
//Adafruit_MAX31855 thermocouple(MAXCS, SPI1);

// Set the pins used
#define cardSelect 4
#define analogPin1 A4
#define analogaccxPin A1
#define analogaccyPin A2
#define analogacczPin A3 
#define digitalHall 5


//Global vars
Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);
File logfile;
uint8_t i=0;
unsigned long currentTime;

//Helper functions
//Accelerometer oversampling, 1ms delayincluded per call
int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading/sampleSize;
}

// blink out an error code
void error(uint8_t errno) {
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
}
//Ram checker
extern "C" char *sbrk(int i);

int RamAvailable () {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

//Stuff
class logObject{
  public:
    double thermocoupleTemp;
    double thermocoupleInternalTemp;
    int analogTMPTemp;
    bool digitalHallEffect;
    int hallhits = 0;
    int RPM = 0;
    float mprlsPressure;
    int analogaccx;
    int analogaccy;
    int analogaccz;
    unsigned long logTimer;
    logObject(){};
    void update(){
      thermocoupleTemp = thermocouple.readCelsius();
      thermocoupleInternalTemp = thermocouple.readInternal();
      analogTMPTemp = analogRead(analogPin1);
      digitalHallEffect = (digitalRead(digitalHall) == LOW) ? true : false;
      hallhits = hallhits + ((digitalRead(digitalHall) == LOW) ? 1  : 0);
      mprlsPressure = mpr.readPressure();
      analogaccx = ReadAxis(analogaccxPin);
      analogaccy = ReadAxis(analogaccyPin);
      analogaccz = ReadAxis(analogacczPin);
    };
    void print(File l){
      Serial.println("Printing to serial first!");
      Serial.print("Time:");
      Serial.print(currentTime - logTimer);
      Serial.print(",");
      Serial.print("Hall state:");
      Serial.print(digitalHallEffect);
      Serial.print(",");
      Serial.print("TMP temp:");
      Serial.print(analogTMPTemp);
      Serial.print(",");
      Serial.print("Thermocouple Internal Temp:");
      Serial.print(thermocoupleInternalTemp);
      Serial.print(",");
      Serial.print("Thermocouple Temp:");
      Serial.print(thermocoupleTemp);
      Serial.print(",");
      Serial.print("MPRLS pressure hPa:");
      Serial.print(mprlsPressure);
      Serial.print(",");
      Serial.print("Accelerometer values: ");
      Serial.print("x: ");
      Serial.print(analogaccx);
      Serial.print(";");
      Serial.print("y: ");
      Serial.print(analogaccy);
      Serial.print(";");
      Serial.print("z: ");
      Serial.print(analogaccz);
      Serial.print(",");
      Serial.println("End of line");
      l.print("Time:");
      l.print(currentTime - logTimer);
      l.print(",");
      l.print("Hall state:");
      l.print(digitalHallEffect);
      l.print(",");
      l.print("TMP temp:");
      l.print(analogTMPTemp);
      l.print(",");
      l.print("Thermocouple Internal Temp:");
      l.print(thermocoupleInternalTemp);
      l.print(",");
      l.print("Thermocouple Temp:");
      l.print(thermocoupleTemp);
      l.print(",");
      l.print("MPRLS pressure hPa:");
      l.print(mprlsPressure);
      l.print(",");
      l.print("Accelerometer values: ");
      l.print("x: ");
      l.print(analogaccx);
      l.print(";");
      l.print("y: ");
      l.print(analogaccy);
      l.print(";");
      l.print("z: ");
      l.print(analogaccz);
      l.print(",");
      l.println("End of line");
      l.flush();
    };
};

logObject Logger;



// This line is not needed if you have Adafruit SAMD board package 1.6.2+
//   #define Serial SerialUSB

void setup() {
  pinMode(digitalHall, INPUT);
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  if(TEST){while(!Serial) delay(1);};
  currentTime = millis();
  Logger.logTimer = currentTime;
  Serial.println("Initializing MPRLS Simple Test");
  if (! mpr.begin()) {
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found MPRLS sensor");
  Logger.mprlsPressure = mpr.readPressure();
  Serial.print("Initial pressure hPa:");
  Serial.println(Logger.mprlsPressure);
  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) { //this shouldn't conflict with SPI bus because it's all spoofing it, but we'll see
    Serial.println("ERROR.");
    while (1) delay(10);
  }
  Serial.println("DONE.");
  Logger.thermocoupleInternalTemp = thermocouple.readInternal();
  Serial.print("Initial Internal Temp = ");
  Serial.println(Logger.thermocoupleInternalTemp);
  Logger.thermocoupleTemp = thermocouple.readCelsius();
  if (isnan(Logger.thermocoupleTemp)) {
    Serial.println("Something wrong with thermocouple!");
  } else {
    Serial.print("Initial Thermocouple Temp C = ");
    Serial.println(Logger.thermocoupleTemp);
    }
   //Serial.print("F = ");
   //Serial.println(thermocouple.readFahrenheit());
  Serial.println("Attempting to assign initial acc values");
  Logger.analogaccx = ReadAxis(analogaccxPin);
  Logger.analogaccy = ReadAxis(analogaccyPin);
  Logger.analogaccz = ReadAxis(analogacczPin);
  Serial.println("Initial acc values: ");
  Serial.print("X: ");
  Serial.print(Logger.analogaccx);
  Serial.print("|Y: ");
  Serial.print(Logger.analogaccy);
  Serial.print("|Z: ");
  Serial.println(Logger.analogaccz);
  Serial.println("Initializing Hall state:");
  if(digitalRead(digitalHall) == LOW){
    Logger.digitalHallEffect = true;
  }else{
    Logger.digitalHallEffect = false;
  }
  Serial.println("Initial Hall state:");
  Serial.println(Logger.digitalHallEffect);
  Serial.println("Initializing TMP temp: ");
  Logger.analogTMPTemp = analogRead(analogPin1);
  Serial.println("Initial TMP reading: ");
  Serial.println(Logger.analogTMPTemp);
  Serial.println("\r\nStarting up SD card");

  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
    error(2);
  }
  char filename[15];
  strcpy(filename, "/ANALOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to "); 
  Serial.println(filename);
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.println("Ready!");
  //if(TEST) delay(20000);//time to wait
}

int quickCounter = 0;
void loop() {
  if (quickCounter < 600){
  digitalWrite(8, HIGH);
  currentTime = millis();
  Logger.update(); //note timer is updated in the print function, logTimer is actually just initial time for now
  Logger.print(logfile);
  Serial.print("Just for fun, ram available: ");
  Serial.println(RamAvailable());
  digitalWrite(8, LOW);
  delay(1000);
  quickCounter++;
  } else if (quickCounter == 60){
  logfile.close();
  digitalWrite(13, HIGH);
  quickCounter++;
  } else{
  Serial.println("Eject me plz!");
  delay(10000);
  }
}
