#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9340.h>
#include <Encoder.h>
#include <openGLCD.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <Time.h>
#include <Wire.h>
#include <Adafruit_STMPE610.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif
#define LOADCELL Serial1

// #define DEBUG

//SD card reader
// MOSI - pin 11
// MISO - pin 12
// CLK - pin 13
const uint8_t sdCS = 4;

int logging = false;
SdFat sd;
SdFile logFile;
char logFileName[13];

#define DIGITS    1
#define END       2
#define COMPLETE  3
int loadCellReadingState = COMPLETE;

char loadCellReading[] = {' ','0','0','0','0','0'};
int loadCellReadingInt = 0;
int loadCellReadingIndex = 0;

int minMaxForce = -999;

#define LOCK 1
#define UNLOCK 0
int peakLock = LOCK;

const int arrayForceSize = 150; //150 ish for normal data capture
int arrayForce[arrayForceSize] = {-998};
int arrayForceIndex = 0;
int currentPeakForce = -998;

const int array2minSize = 60;
int array2minPeakForce[array2minSize] = {0};
int array2minIndex = 0;
int avgShortTermForce = 0;

const int array2hrSize = 60;
int array2hrPeakForce[array2hrSize] = {0};
int array2hrIndex = 0;
int avgLongtermForce = 0;
float avgForce = 0;
int cycles = 0;

#define STATIC 0
#define DYNAMIC 1
int mode = DYNAMIC;

const int probeBPin = 4;
const int probeAPin = 5;

int valProbe = 0;

Encoder probe(probeBPin ,probeAPin);
//colorLCD stuff
#define GRAPHHEIGHT 200
#define GRAPHWIDTH 120
#define RIGHTGRAPHPOS 199
#define LEFTGRAPHPOS 1
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

Adafruit_ILI9340 tft = Adafruit_ILI9340(lcd_cs, lcd_dc, -1);

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

//-----------------------------------------------------------------------------
// reusable worker methods, should be moved to new file, but le lazy

void clearGraph(int graphStartX) {
  tft.fillRect(graphStartX, 0, GRAPHWIDTH, GRAPHHEIGHT, BACKGROUNDCOLOUR);
}

void drawGraphLines(int *graphStartX, int last, int temp) {
    if (last > temp) {
      //erase, aka draw background
      tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT-last, last-temp, BACKGROUNDCOLOUR);
      tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT-last, last-temp, BACKGROUNDCOLOUR);
    }
    else if (last < temp) {
      //add, aka draw white
      tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT-temp, temp-last, TEXTCOLOUR);
      tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT-temp, temp-last, TEXTCOLOUR);
    }
    else {
      (*graphStartX)++;
      (*graphStartX)++;
    }
}

void redrawGraph(int graphStartX, int array[], const int arrayIndex, const int arraySize, const int bumped) {

  int i = arrayIndex + 1;
  if (i==arraySize) { i=0; } //avoids % operation

  //setup values and use bumped for graphing oldest value in graph
  int temp = array[i]/divider;
  if(temp < 0) { temp=0; }
  int lastI = -1;
  int last = bumped / divider;
  if(last < 0) { last=0; }

  //loop through all the middle values in the graph
  while( i!=arrayIndex ) {

    drawGraphLines(&graphStartX, last, temp);

    i++;
    if (i==arraySize) { i=0; } //avoids % operation
    temp = array[i]/divider;
    if(temp < 0) { temp=0; }
    lastI = i-1;
    if (lastI <0) { lastI = arraySize-1; }
    last = array[lastI]/divider;
    if(last < 0) { last=0; }
  }

  //draw latest value added to graph
  drawGraphLines(&graphStartX, last, temp);

}

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

void drawButton(int x, int y, int w, int h, char* str) {
  int r = 4;
  tft.fillRect(x-shaddow, y-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);
  tft.drawFastHLine(x+r -shaddow, y -shaddow  , w-2*r, SHADDOWCOLOUR); // Top
  tft.drawFastVLine(x -shaddow  , y+r -shaddow, h-2*r, SHADDOWCOLOUR); // Left
  tft.drawCircleHelper(x+r -shaddow, y+r -shaddow, r, 1, SHADDOWCOLOUR); //top left corner
  tft.drawCircleHelper(x+w-r-1 -shaddow, y+r -shaddow, r, 2, SHADDOWCOLOUR); //top right corner
  tft.drawCircleHelper(x+r -shaddow, y+h-r-1 -shaddow, r, 8, SHADDOWCOLOUR); //bottom left corner
  tft.drawRoundRect(x, y, w, h, BUTTONRADIUS,BUTTONCOLOUR);
  int newY = y + (h - CHARHEIGHT)/2;
  int newX = x + (w - (strlen(str) * CHARWIDTH))/2;
  tft.setCursor(newX, newY);
  tft.print(str);
}

void pushButton(int x, int y, int w, int h, char* str) {
  tft.fillRect(x-shaddow, y-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);
  tft.drawRoundRect(x, y, w, h, BUTTONRADIUS, SHADDOWCOLOUR);
  int newY = y + (h - CHARHEIGHT)/2;
  int newX = x + (w - (strlen(str) * CHARWIDTH))/2;
  tft.setCursor(newX, newY);
  tft.print(str);
}

//touch screen buttons
class TouchButton {
  private:
    int x;
    int y;
    int w;
    int h;
    char *label;
  public:
    TouchButton(int x, int y, int w, int h, char* label) {
      this->x = x;
      this->y = y;
      this->w = w;
      this->h = h;
      this->label = label;
    }
    int isPushed(int tx, int ty) {
      return tx > x - w/2  &&  tx < x + w/2
             && ty > y - h/2  &&  ty < y + h/2;
    }
    void setLabel(char *newLabel) {
      #ifdef DEBUG
        Serial.print(F("setLabel() "));
        Serial.print(F(label));
        Serial.print(F(" to "));
        Serial.println(F(newLabel));
      #endif
      this->label = newLabel;
    }
    void draw() {
      #ifdef DEBUG
        Serial.print(F("draw() "));
        Serial.println(F(label));
      #endif
      drawButton(x-w/2, y-h/2, w, h, label);
    }
    void push() {
      #ifdef DEBUG
        Serial.print(F("push() "));
        Serial.println(F(label));
      #endif
      pushButton(x-w/2, y-h/2, w, h, label);
    }
};

TouchButton *logBtn;
TouchButton *modeBtn;
TouchButton *calBtn;

//-----------------------------------------------------------------------------

void pollProbe() {
  valProbe = probe.read();
  
  char probeString[6];
  sprintf(probeString,"%5d", valProbe);
  rePrint(158, TXTROW3, 5, probeString);
}

void pollForceMeter() {
  if (LOADCELL.available() > 0) {
    
    char incomingByte = LOADCELL.read();
    
    // -- debugging bytes from loadcell --
//    char temp[16];
//    sprintf(temp, "index %d, byte >%c<", loadCellReadingIndex, incomingByte);
//    Serial.println(temp);

    // -- parseLoadCell incomingByte --
    switch (incomingByte) {
      case 32: // leading space ' ' represents positive value
      case 45: // leading minus sign '-'
        loadCellReadingIndex=0;
        loadCellReading[loadCellReadingIndex] = incomingByte;
        loadCellReadingIndex++;
        loadCellReadingState = DIGITS;
        break;
      case 46: // ending '.'
        if (loadCellReadingState == END && loadCellReadingIndex == 6) {
          loadCellReadingState = COMPLETE;
        }
        break;
      case 13:
        if (loadCellReadingState == COMPLETE) {
          loadCellReadingState = DIGITS; // must change or you'll output twice
        }
        break;
      case 48: // == zero '0'
      case 49:
      case 50:
      case 51:
      case 52:
      case 53:
      case 54:
      case 55:
      case 56:
      case 57: // == nine '9'
        if (loadCellReadingState == DIGITS && loadCellReadingIndex < 6) {
          loadCellReading[loadCellReadingIndex] = incomingByte;
          loadCellReadingIndex++;
          if(loadCellReadingIndex == 6) {
            loadCellReadingState = END;
          }
        }
        break;
    }
  
  }
    
}

int shouldCalculatePeakForce() {
  int proceed = 0;
  if( mode == DYNAMIC ) {
    if (peakLock==LOCK && loadCellReadingInt > 50) {
      peakLock = UNLOCK;
    }
    if (peakLock==UNLOCK && loadCellReadingInt < 10) {
      peakLock = LOCK;
      proceed = 1;
    }
  }
  
  else { //mode == STATIC
    if(arrayForceIndex == arrayForceSize) {
      proceed=1;
    }
  }
  return proceed;
}

void updatePeaks(char newLoadCellReading[]) {
  loadCellReadingInt = atoi(newLoadCellReading);
  
  // -- save force value --
  arrayForce[arrayForceIndex] = loadCellReadingInt;
  arrayForceIndex++;
  int newPeak = shouldCalculatePeakForce(); // need to catch array being full
  if(arrayForceIndex==arrayForceSize) {
    arrayForceIndex=0;
  }
  
  char loadCellString[7];
  sprintf(loadCellString,"%6d", loadCellReadingInt);
  rePrint(280, TXTROW3, 6, loadCellString);
  
  // -- see if it's time to calculate new peak force --
  if ( newPeak==1 ) {

    currentPeakForce = -998;
    for (int i=0; i<arrayForceSize; i++) {
      if(arrayForce[i] > currentPeakForce) {
        currentPeakForce = arrayForce[i];
      }
      arrayForce[i] = -998;
    }
    cycles++;
    rePrint(158, TXTROW1, 6, cycles);
    
    rePrint(292, TXTROW2, 4, currentPeakForce);
    
    // -- update max and avgPeaks --
    if (currentPeakForce > minMaxForce) {
      minMaxForce = currentPeakForce;
      
      char minMaxForceString[5];
      sprintf(minMaxForceString,"%4d", minMaxForce);
      rePrint(30, TXTROW3, 4, minMaxForceString);
    }
    avgForce = (avgForce*(cycles-1) + currentPeakForce) / cycles;
    char avgForceString[5];
    sprintf(avgForceString,"%4.0f", avgForce);
    rePrint(30, TXTROW2, 4, avgForceString);
    
    // -- save peak value -- 
    int bumped2min = array2minPeakForce[array2minIndex];
    array2minPeakForce[array2minIndex] = currentPeakForce;
    
    // -- redraw short term graph --
    redrawGraph(RIGHTGRAPHPOS, array2minPeakForce, array2minIndex, array2minSize, bumped2min);
    
    array2minIndex++;
    
    //see if it's time to calculate new short term peak avg
    if (array2minIndex==array2minSize) {
      array2minIndex=0;

      int nonZero = 0;
      avgShortTermForce = 0;
      for (int i=0; i<array2minSize; i++) {
        if(array2minPeakForce[i] > 0) {
          nonZero++; 
          avgShortTermForce += array2minPeakForce[i];
        }
      }
      if (nonZero > 0) { 
        avgShortTermForce /= nonZero;
      }
      
      char avgShortTermForceString[9];
      sprintf(avgShortTermForceString,"%4d", avgShortTermForce);
      rePrint(280, TXTROW1, 4, avgShortTermForceString);
      
      // -- save short term peak avg --
      int bumped2hr = array2hrPeakForce[array2hrIndex];
      array2hrPeakForce[array2hrIndex] = avgShortTermForce;      

      // -- redraw long term graph --
      redrawGraph(LEFTGRAPHPOS, array2hrPeakForce, array2hrIndex, array2hrSize, bumped2hr);

      array2hrIndex++;

      //see if it's time to calculate new long term peak avg
      if (array2hrIndex==array2hrSize) {
        array2hrIndex = 0;
      }
      
      nonZero = 0;
      avgLongtermForce = 0;
      for (int i=0; i<array2hrSize; i++) {   
        if(array2hrPeakForce[i] > 0) {
          nonZero++;
          avgLongtermForce += array2hrPeakForce[i];
        }
      }
      if (nonZero > 0) { 
        avgLongtermForce /= nonZero;
      }
      
      char avgLongtermForceString[9];
      sprintf(avgLongtermForceString,"%4d", avgLongtermForce);
      rePrint(66, TXTROW1, 4, avgLongtermForceString);

    }
    
  }

}

void logOutput(){
  
  if (logging==true) {

    char logString[128];

    //initial datestamp for log entry
    sprintf(logString, "%04d/%02d/%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second(), micros());
    
    // linear probe
    if (linearProbe) {
      sprintf(logString, ", %d", valProbe);
    }

    // log number of cycles
    if (cycles) {
      sprintf(logString, ", %d", cycles);
    }

    // loadcell data
    if (loadcell) {
      sprintf(logString, ", %d, %d, %d, %d, %d, %d, %f", cycles, loadCellReadingInt, minMaxForce, avgShortTermForce, avgLongtermForce, avgForce);
    }

    if (analog1) {

    }

    if (analog2) {

    }

    if (digital1) {

    }

    if (digital2) {

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
      rePrint(130, 0, 9, "log error");
      delay(1000);
      rePrint(130, 0, 9, "");
      #ifdef DEBUG
        Serial.println(F("...log file error"));
      #endif
      
    }
  }
  
}

void zeroArray(int array[], int len, int val) {
  for(int i=0; i<len; i++){
    array[i] = val;
  }
}

void calibrate() {
  #ifdef DEBUG
    Serial.println("calibrate...");
  #endif

  probe.write(0);
  
  minMaxForce = -999;
  peakLock = LOCK;
  zeroArray(arrayForce, arrayForceSize, -998);
  peakLock = LOCK;
  arrayForce[arrayForceSize] = {-998};
  arrayForceIndex = 0;
  currentPeakForce = -998;
  
  zeroArray(array2minPeakForce, array2minSize, 0);
  array2minIndex = 0;
  avgShortTermForce = 0;
  clearGraph(RIGHTGRAPHPOS);

  zeroArray(array2hrPeakForce, array2hrSize, 0);
  array2hrIndex = 0;
  avgLongtermForce = 0;
  clearGraph(LEFTGRAPHPOS);
  
  avgForce = 0;
  cycles = 0;
  
  loadCellReadingState = DIGITS;
  loadCellReadingInt = 0;
  loadCellReadingIndex = 0;

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

void stopLogging() {
  logging = false;
}

void toggleLogging() {
    if (logging==false) {
      if (startLogging()) {
        logBtn->setLabel("LOG ON");
      }
    }
  else {
      stopLogging();
      logBtn->setLabel("LOG OFF");
    }
}

void toggleMode() {
  if (mode==DYNAMIC) {
    mode = STATIC;
    modeBtn->setLabel("STATIC");
  }
  else {
    mode = DYNAMIC;
    modeBtn->setLabel("DYNAMIC");
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
  
  LOADCELL.begin(19200, SERIAL_8N1_RXINV_TXINV);
  
  SdFile::dateTimeCallback(dateTime);
  while (!sd.begin(sdCS, SPI_HALF_SPEED)) {
    // tft.setCursor(5, 30);
    tft.println(F("Insert microSD card"));
    delay(500);
    #ifdef DEBUG
      Serial.println(F("Insert microSD card"));
    #endif

  }
  tft.fillScreen(BACKGROUNDCOLOUR);
  
  //graph side bars
  tft.drawFastVLine(  0, 0, GRAPHHEIGHT, TEXTCOLOUR);
  tft.drawFastVLine(121, 0, GRAPHHEIGHT, TEXTCOLOUR);
  tft.drawFastVLine(198, 0, GRAPHHEIGHT, TEXTCOLOUR);
  tft.drawFastVLine(319, 0, GRAPHHEIGHT, TEXTCOLOUR);

  //left labels
  tft.setCursor(12, TXTROW1);
  tft.print("2hr avg: ");
  tft.print("____");
  tft.setCursor(0, TXTROW2);
  tft.print("avg: ");
  tft.setCursor(0, TXTROW3);
  tft.print("max: ");

  //center labels
  tft.setCursor(110, TXTROW1);
  tft.print("cycles: ");

  tft.setCursor(104, TXTROW3);
  tft.print("encoder: ");
  tft.print("_____");

  //right labels
  tft.setCursor(220, TXTROW1);
  tft.print("2min avg: ");
  tft.print("____");
  tft.setCursor(228, TXTROW2);
  tft.print("last peak: ");
  tft.setCursor(240, TXTROW3);
  tft.print("force: ");

  //draw center buttons
  logBtn = new TouchButton(159, 30, 60, 40, "LOG OFF");
  modeBtn = new TouchButton(159, 100, 60, 40, "STATIC");
  calBtn = new TouchButton(159, 170, 60, 40, "CALIBRATE");
  logBtn->draw();
  modeBtn->draw();
  calBtn->draw();
  

  #ifdef DEBUG
    Serial.println(F("setup complete"));
  #endif

} //end setup()


void loop() {

  pollForceMeter();

  if (loadCellReadingState == COMPLETE) {

    updatePeaks(loadCellReading);

    pollProbe();
    
    // See if there's any touch data for us
    if (!ts.bufferEmpty()) {
      parseTouch();
    }

    if (logging==true) {
      logOutput();
    }
  }
  
} //end loop()

