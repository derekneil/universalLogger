#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9340.h>

#include <openGLCD.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <Time.h>
#include <Wire.h>
#include <Adafruit_STMPE610.h>
#include "Data.h"
#include "SensorInput.h"
#include "Graph.h"
#include "TouchButton.h"

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// #define DEBUG

//colorLCD stuff
// #define GRAPHHEIGHT 200
// #define GRAPHWIDTH 120
// #define RIGHTGRAPHPOS 199
// #define LEFTGRAPHPOS 1
#define TXTROW1 205
#define TXTROW2 220
#define TXTROW3 230
#define CHARWIDTH 6
#define CHARHEIGHT 8
const int divider = 5; //= (max expected input ~1000 )/ GRAPHHEIGHT
#define lcd_cs 10
#define lcd_dc 9
#define lcd_rst -1
#define BACKGROUNDCOLOUR ILI9340_BLUE
#define TEXTCOLOUR ILI9340_WHITE
#define ERASECOLOUR ILI9340_BLUE
#define BUTTONCOLOUR ILI9340_WHITE
#define BUTTONPUSHEDCOLOUR ILI9340_MAGENTA
#define BUTTONRADIUS 4
#define SHADDOWCOLOUR ILI9340_BLACK
const int shaddow = 2;
#define SCREENWIDTH 320
#define SCREENHEIGHT 240

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
SensorInput sensorInputs[];

//-----------------------------------------------------------------------------
// reusable worker methods, should be moved to new file, but le lazy



void rePrint(int x, int y, int len, int value) {
  tft.fillRect(x, y, CHARWIDTH*len, CHARHEIGHT, ERASECOLOUR);
  tft.setCursor(x, y);
  tft.print(value);
}

void rePrint(int x, int y, int len, char* str) {
  tft.fillRect(x, y, CHARWIDTH*len, CHARHEIGHT, ERASECOLOUR);
  tft.setCursor(x, y);
  tft.print(str);
}



void drawMainScreen() {
  tft.fillScreen(BACKGROUNDCOLOUR);

  //TODO draw each enabled sensorInput from scratch

}

void drawMenuScreen() {
  logBtn->draw();
  modeBtn->draw();
  calBtn->draw();
}

//-----------------------------------------------------------------------------

void poll() {

  //loop through sensorInputs
  for (int i=0; i<NUMINPUTS; i++) {

    if(sensorInputs[i]->enabled) {

      //get value
      int newReading = sensorInputs[i]->poll();

      //update data & stats
      sensorInputs[i]->updateDataAndStats(newReading);

      //update graph
      sensorInputs[i]->updateGraph();
    }
  }
}

void logOutput(){
  
  if (logging==true) {

    char logString[128];

    //initial datestamp for log entry
    sprintf(logString, "%04d/%02d/%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second(), micros());
    
    for (int i=0; i<NUMINPUTS; i++) {
      if(sensorInputs[i]->enabled) {

        sprintf(logString, ", %s", sensorInputs[i]->logout());

      }
    }

    // // linear probe
    // if (linearProbe) {
    //   sprintf(logString, ", %d", valProbe);
    // }

    // // log number of cycles
    // if (cycles) {
    //   sprintf(logString, ", %d", cycles);
    // }

    // // loadcell data
    // if (loadcell) {
    //   sprintf(logString, ", %d, %d, %d, %d, %d, %d, %f", cycles, loadCellReadingInt, minMaxForce, avgShortTermForce, avgLongtermForce, avgForce);
    // }


    //write to sd card
    if (!logFile.open(logFileName, O_CREAT | O_APPEND | O_WRITE)) {
      logFile.println(logString);
      logFile.close();
    #ifdef DEBUG
      Serial.println(logString);
    #endif
    }
    else {
      rePrint(130, 0, 9, "log error");
      delay(1000);
      rePrint(130, 0, 9, "");
      #ifdef DEBUG
        Serial.println(F("...log file error"));
      #endif
      
    }
  }
  
}

void resetAll() {
  //calibrate and clear all data?? 
  for (int i=0; i<NUMINPUTS; i++) {
    if(sensorInputs[i]->enabled) {

      sensorInputs[i]->reset();

    }
  }

  // what about starting a new log file?
}

void calibrateAll() { //just zero inputs
  #ifdef DEBUG
    Serial.println("calibrate...");
  #endif

  for (int i=0; i<NUMINPUTS; i++) {
    if(sensorInputs[i]->enabled) {

      sensorInputs[i]->calibrate();

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
        logBtn->setLabel("LOG ON");
      }
    }
  else {
      logging = false;
      logBtn->setLabel("LOG OFF");
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

void parseTouch() {
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
    Serial.print(F("-> mapped to "));
    Serial.print(x);
    Serial.print(F(", "));
    Serial.println(y);
  #endif

  emptyTouchBuffer();

  //make sure we don't get duplicate touches
  int newTouchTime = millis();
  if((newTouchTime > lastTouchTime + MINTOUCHINTERVAL) ){
    lastTouchTime = newTouchTime;

    if (logBtn->isPushed(x,y)) {
      #ifdef DEBUG
        Serial.println(F("logBtn isPushed"));
      #endif
      logBtn->push();
      toggleLogging();
      logBtn->draw();
    }
    else if (modeBtn->isPushed(x,y)) {
      #ifdef DEBUG
        Serial.println(F("modeBtn isPushed"));
      #endif
      modeBtn->push();
      toggleMode();
      modeBtn->draw();
    }
    else if (calBtn->isPushed(x,y)) {
       #ifdef DEBUG
        Serial.println(F("calBtn isPushed"));
      #endif
      calBtn->push();
      calibrate();
      calBtn->draw();
      emptyTouchBuffer(); //just to make sure
    }

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

  #ifdef DEBUG
    Serial.print("tft height: ");
    Serial.print(tft.height());
    if (tft.height()==SCREENHEIGHT) {
      Serial.println(" as expected :)");
    }
    else {
      Serial.print(" does not match ");
      Serial.print(SCREENHEIGHT);
      Serial.println(" DANGER!!!!");
    }
    Serial.print("tft width: ");
    Serial.print(tft.width());
        if (tft.width()==SCREENWIDTH) {
      Serial.println(" as expected :)");
    }
    else {
      Serial.print(" does not match ");
      Serial.print(SCREENWIDTH);
      Serial.println(" DANGER!!!!");
    }
  #endif

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
    // tft.setCursor(5, 30);
    tft.println(F("Insert microSD card"));
    delay(500);
    #ifdef DEBUG
      Serial.println(F("Insert microSD card"));
    #endif

  }

  sensorInputs[4] = {
    new ForceMeter(),
    new LinearEncoder(), 
    new SensorInput(), 
    new SensorInput()
  };

  setupMenuScreen();

  setupSecondLevelMenuScreens(); //???????????????????

  drawMainScreen();

  #ifdef DEBUG
    Serial.println(F("setup complete"));
  #endif

} //end setup()


void loop() {

  pollSensors();

  logOutput();
  
  if (!ts.bufferEmpty()) { //this should stay at the begining or end of a loop
    parseTouch();
  }
  
} //end loop()

