#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9340.h>

#include <openGLCD.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <Time.h>
#include <Wire.h>
#include <Adafruit_STMPE610.h>

#include "ForceMeter.h"
#include "LinearEncoder.h"
#include "SensorInput.h"
#include "SensrDisplay.h"
#include "SensorData.h"
#include "TouchButton.h"

#include "universalLogger.h"

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

const int divider = 5; //= (max expected input ~1000 )/ GRAPHHEIGHT
#define lcd_cs 10
#define lcd_dc 9
#define lcd_rst -1
const int shaddow = 2;

Adafruit_ILI9340 tft = Adafruit_ILI9340(lcd_cs, lcd_dc, lcd_rst);

//touch screen stuff
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000
#define MINTOUCHINTERVAL 500
int lastTouchTime = 0;
#ifdef DEBUG
  uint8_t checkBit = 0;
#endif

//SD card reader
// MOSI - pin 11
// MISO - pin 12
// CLK - pin 13
const uint8_t sdCS = 4; // determined from ILI9341 touchscreen sheild with built in SD card reader

int logging = false;
SdFat sd;
SdFile logFile;
char logFileName[13]; //limited by 8.3 fat filesystem naming :(

//menu screen
TouchButton *logBtn;
TouchButton *modeBtn;
TouchButton *calBtn;

//main screen
SensorInput *sensorInputs; //array of sensor inputs 

//-----------------------------------------------------------------------------
// reusable worker methods, should be moved to new file, but le lazy


void drawMainScreen() {
  tft.fillScreen(BACKGROUNDCOLOUR);

  for (int i=0; i<NUMREGIONS; i++) {
    //TODO draw each enabled sensorInput from scratch
    sensorInputs[i].draw();
  }

}

void drawMenuScreen() {

  //TODO draw back button

  //TODO draw LOG label & toggle switch
  // logBtn.draw();

  //TODO draw global RESET, MODE, CALIBRATE buttons
  // resetAllBtn.draw();
  // modeAllBtn.draw();
  // calAllBtn.draw();
  
  //TODO listen for touch events in infinite loop
  if (!(Display::touch->bufferEmpty())) { //this should stay at the begining or end of a loop
    parseMenuTouch();
  }

  //TODO touching back button breaks out of loop to return 

}

void drawSensorMenuScreen(SensorInput *si) {

  //TODO draw back button

  //TODO draw controls for each of the inputs for a sensor

  //TODO draw specilized controls for loadcell and linearEnc???

  //TODO listen for touch events in infinite loop
  if (!(Display::touch->bufferEmpty())) { //this should stay at the beginning or end of a loop
    parseSensorMenuTouch();
  }

  //TODO touching back button breaks out of loop to return 
}

//-----------------------------------------------------------------------------

void pollSensors() {

  //loop through sensorInputs
  for (int i=0; i<NUMINPUTS; i++) {

    if(sensorInputs[i].enabled) {

      //get value
      int newReading = sensorInputs[i].poll();

      //update data & stats
      sensorInputs[i].updateDataAndRedrawStats(newReading);

      //update graph
      sensorInputs[i].updateViz();
    }
  }
}

void logOutput(){
  
  if (logging==true) {

    char logString[128]; //TODO how big does this need to be?? have to free dynamically allocated memeory!

    //initial datestamp for log entry
    sprintf(logString, "%04d/%02d/%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second(), micros());
    
    for (int i=0; i<NUMINPUTS; i++) {
      if(sensorInputs[i].isEnabled()) {
        sprintf(logString, ", %s", sensorInputs[i].logout());
      }
    }

    //write to sd card
    if (!logFile.open(logFileName, O_CREAT | O_APPEND | O_WRITE)) {
      logFile.println(logString);
      logFile.close();
    #ifdef DEBUG
      Serial.println(logString);
    #endif
    }
    else {
      rePrint(130, 0, 9, "log error"); //TODO just make this gigantic on screen error msg
      #ifdef DEBUG
        Serial.println(F("...log file error"));
      #endif

     while(1) {}
      
    }

    //TODO delete logString; to free memory? 

  }
  
}

void resetAll() {
  //calibrate and clear all data?? 
  for (int i=0; i<NUMINPUTS; i++) {
    if(sensorInputs[i].enabled) {

      sensorInputs[i].reset();

    }
  }

  // what about starting a new log file?
}

void calibrateAll() { //just zero inputs
  #ifdef DEBUG
    Serial.println("calibrate... ");
  #endif

  for (int i=0; i<NUMINPUTS; i++) {
    if(sensorInputs[i].enabled) {

      sensorInputs[i].calibrate(); /** only some sensors implement calibrate */

    }
  }

  delay(500);
  
  #ifdef DEBUG
    Serial.println(F("... and reset complete"));
  #endif
  return;
}

int startLogging() {
  #ifdef DEBUG
    Serial.print(F("starting new logfile called "));
  #endif

  //filename limited by SDFat library to 8 chars name and 3 chars extension
  sprintf(logFileName, "%02d%02d%02d%02d.csv", day(), hour(), minute(), second());
  #ifdef DEBUG
    Serial.println(logFileName);
  #endif
  
  if( !logFile.open(logFileName, O_CREAT | O_APPEND | O_WRITE) ){
    rePrint(130, 0, 9, "log error");
    delay(1000);
    rePrint(130, 0, 9, "");
    #ifdef DEBUG
      Serial.println(F("...log file error"));
    #endif
    return 0;
  }
  else {
    logging = true;
    logFile.close();
    return 1;
  }
}

void toggleLogging() {
    if (logging==false) {
      if (startLogging()) {
        logBtn.setLabel("LOG ON");
      }
    }
  else {
      logging = false;
      logBtn.setLabel("LOG OFF");
    }
}

void emptyTouchBuffer() {
  while(!ts.bufferEmpty()) {
    ts.getPoint();
    #ifdef DEBUG
      Serial.println(F("empty ts buffer"));
    #endif

  }
}

int parseTouchBoilerPlate() {
	  // Retrieve a point
	  TS_Point p = ts.getPoint();
	  // Scale using the calibration #'s
	  // and rotate coordinate system
	  #ifdef DEBUG
	    Serial.print(F("\noriginal touch at "));
	    Serial.print(p.x);
	    Serial.print(F(", "));
	    Serial.print(p.y);
	  #endif
	  p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
	  p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
	  int y = tft.height() - p.x;
	  int x = p.y;

	  #ifdef DEBUG
	    Serial.print(F(". mapped to "));
	    Serial.print(x);
	    Serial.print(F(", "));
	    Serial.println(y);
	  #endif

	  emptyTouchBuffer();

	  //make sure we don't get duplicate touches
	  int newTouchTime = millis();
	  if((newTouchTime > lastTouchTime + MINTOUCHINTERVAL) ){
	    lastTouchTime = newTouchTime;
	    return true;
	  }
	  return false;
}

void parseSensorMenuTouch() {
	if (parseTouchBoilerPlate()) {

		//----------START LOGIC BLOCK--------------

    //TODO implement touch buttons for all the SensorInput variables we want to control

		//----------END LOGIC BLOCK----------------

	}
	else {
	  #ifdef DEBUG
		Serial.println(F("ignoring menu touch, to soon after last touch"));
	  #endif
	}
}

void parseMenuTouch() {
	if (parseTouchBoilerPlate()) {

		//----------START LOGIC BLOCK--------------

    //TODO implement touch buttons for all the global options we want to control

    //TODO implement touch buttons for accessing menu for a particular SensorInput

		//sample logic to be used for menu touch parsing

		//    if (logBtn.isPushed(x,y)) {
		//      #ifdef DEBUG
		//        Serial.println(F("logBtn isPushed"));
		//      #endif
		//      logBtn.push();
		//      toggleLogging();
		//      logBtn.draw();
		//    }
		//    else if (modeBtn.isPushed(x,y)) {
		//      #ifdef DEBUG
		//        Serial.println(F("modeBtn isPushed"));
		//      #endif
		//      modeBtn.push();
		//      toggleMode();
		//      modeBtn.draw();
		//    }
		//    else if (calBtn.isPushed(x,y)) {
		//       #ifdef DEBUG
		//        Serial.println(F("calBtn isPushed"));
		//      #endif
		//      calBtn.push();
		//      calibrate();
		//      calBtn.draw();
		//      emptyTouchBuffer(); //just to make sure
		//    }

			//----------END LOGIC BLOCK----------------

	}
	else {
	  #ifdef DEBUG
		Serial.println(F("ignoring menu touch, to soon after last touch"));
	  #endif
	}
}


/** would be great to reuse this with a different*/
void parseTouch() {

	if (parseTouchBoilerPlate()) {

		//----------START LOGIC BLOCK--------------

		drawMainMenu(); /** menu handles all it's own touch and navigation */

		//need to erase menu screen and draw SensorDisplay's from scratch
		drawMainScreen();

		//----------END LOGIC BLOCK----------------

	}
	else {
	  #ifdef DEBUG
		Serial.println(F("ignoring touch, to soon after last touch"));
	  #endif
	}

}

time_t getTeensy3Time() { return Teensy3Clock.get(); }

// User provided date time callback function
void dateTime(uint16_t* date, uint16_t* time) {
  time_t now();
  *date = FAT_DATE(year(), month(), day());
  *time = FAT_TIME(hour(), minute(), second());
}

//-----------------------------------------------------------------------------

void setup() {
  #ifdef DEBUG
    //serial console
    Serial.begin(9600);
    while (!Serial) {}
    Serial.println(F("setup begin"));
  #endif
  
  //setup teensy clock
  setSyncProvider(getTeensy3Time);
  
  //setup color lcd
  tft.begin();

  #if defined(__TFT_ILI9340__) && (defined(__MK20DX128__) || defined(__MK20DX256__))
    //want try the fastest?
    tft.setBitrate(24000000);
    #ifdef DEBUG
      Serial.println(F("setBitrate(24000000)"));
    #endif
  #endif
  tft.fillScreen(BACKGROUNDCOLOUR);
  tft.setTextColor(TEXTCOLOUR);
  tft.setTextSize(1);
  tft.setRotation(1);

  while(!ts.begin()){
    tft.println(F("Unable to start touchscreen."));
    #ifdef DEBUG
      Serial.println("Unable to start touchscreen.");
    #endif
    delay(500);
  }

  #ifdef DEBUG
    Serial.println("Touchscreen started.");
  #endif
  
  SdFile::dateTimeCallback(dateTime);
  while (!sd.begin(sdCS, SPI_HALF_SPEED)) {
    tft.println(F("Insert microSD card"));
    delay(500);
    #ifdef DEBUG
      Serial.println(F("Insert microSD card"));
    #endif

  }

  sensorInputs[4] = {};
  sensorInputs[0] = ForceMeter(0,1);
  sensorInputs[1] = LinearEncoder(5,6);
  sensorInputs[2] = SensorInput(16, ANALOG);
  sensorInputs[3] = SensorInput(17, DIGITAL);

  Display display(tft, ts, NUMREGIONS);
  //loop through sensorInputs
  for (int i=0; i<NUMINPUTS; i++) {
	  display.add(sensorInputs[i].shortTermDisplay);
  }

  drawMainScreen();

  #ifdef DEBUG
    Serial.println(F("setup complete"));
  #endif

} //end setup()


void loop() {

  pollSensors();

  logOutput();
  
  if (!(Display::touch.bufferEmpty()) { //this should stay at the begining or end of a loop
    parseTouch();
  }
  
} //end loop()

