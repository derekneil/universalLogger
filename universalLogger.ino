#include "universalLogger.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9340.h>

#include <SdFat.h>
#include <SdFatUtil.h>
#include <Time.h>
#include <Wire.h>
#include <Adafruit_STMPE610.h>

#include "Display.h"
#include "ForceMeter.h"
#include <Encoder.h>
#include "LinearEncoder.h"
#include "SensorInput.h"
#include "SensorDisplay.h"
#include "TouchButton.h"

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

const int divider = 5; // = (max expected input ~1000 )/ GRAPHHEIGHT //FIXME divider update based on max input seen?
#define lcd_cs 10
#define lcd_dc 9
#define lcd_rst -1

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
int touchX = 0;
int touchY = 0;

//SD card reader
// MOSI - pin 11
// MISO - pin 12
// CLK - pin 13
const uint8_t sdCS = 4; // determined from ILI9341 touchscreen sheild with built in SD card reader

int logging = false;
SdFat sd;
SdFile logFile;
char logFileName[13]; //limited by 8.3 fat filesystem naming :(

//main screen
	Display *display;
SensorInput *sensorInputs[NUMINPUTS]; //array of sensor inputs

//menu screen
TouchButton *logBtn;

//-----------------------------------------------------------------------------
// reusable worker methods, should be moved to new file, but le lazy


void drawMainScreen() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("drawMainScreen()"));
		}
	#endif

	//clear the screen of what ever was there
	tft.fillScreen(BACKGROUNDCOLOUR);
	tft.setTextSize(1);

	for (int i=0; i<NUMREGIONS; i++) {
		sensorInputs[i]->draw(); //XXX each of these checks to see if it's active, alternatively Display could have something that loops through it's regions and just calls draw on those...
	}
}

#define mSize 7
class Menu {
  private:
	int mIndex = 0;
	DisplayElement *m[mSize] = { nullptr };
  public:
	Menu() {}
	~Menu() {}
	void add(DisplayElement *el) { if (mIndex< mSize) { m[mIndex++] = el; } }
	void draw() {
		Display::device->fillScreen(MENUCOLOUR);
		Display::device->setTextSize(2);
		for (int i=0; i<mSize; i++){
			m[i]->draw();
		}
	}
};

void drawMainMenu() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("drawMainMenu()"));
		}
	#endif

	Menu menu;

	TouchButton backBtn(  CENTER_X1, CENTER_Y1-30, "Back");
	menu.add(&backBtn);

	if (logging==false) {
		logBtn = new TouchButton( CENTER_X1, CENTER_Y1+30, "Log OFF");
	}
	else {
		logBtn = new TouchButton( CENTER_X1, CENTER_Y1+30, "Log ON");
	}
	menu.add(logBtn);

	TouchButton resetAllBtn(CENTER_X2, CENTER_Y1+30, "Reset All");
	menu.add(&resetAllBtn);

	TouchButton *inputButtons[NUMINPUTS] = {nullptr};

	//XXX make this parametric to the number of inputs... and put them in a pretty order based on the number instead of manually tweaking
	inputButtons[0] = new TouchButton(CENTER_X1,    CENTER_Y2-30, sensorInputs[0]->label, sensorInputs[0]);
	inputButtons[1] = new TouchButton(CENTER_X1+10, CENTER_Y2+30, sensorInputs[1]->label, sensorInputs[1]);
	inputButtons[2] = new TouchButton(CENTER_X2,    CENTER_Y2-30, sensorInputs[2]->label, sensorInputs[2]);
	inputButtons[3] = new TouchButton(CENTER_X2,    CENTER_Y2+30, sensorInputs[3]->label, sensorInputs[3]);
	for (int i=0; i<NUMINPUTS; i++) {
		menu.add(inputButtons[i]);
	}

	menu.draw();

	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("drawMainMenu() listen for menu touch"));
		}
	#endif

	while (1) {
		if (!(ts.bufferEmpty())) {
			if (parseTouchBoilerPlate()) {

				if (backBtn.isPushed(touchX,touchY)) {
					#ifdef DEBUG
						if (Serial) {
							Serial.println(F("backBtn isPushed"));
						}
					#endif
					backBtn.push();
					break; //break out of this menu's touch loop
				}
				else if (logBtn->isPushed(touchX,touchY)) {
					#ifdef DEBUG
					if (Serial) {
						Serial.println(F("logBtn isPushed"));
					}
					#endif
					logBtn->push();
					toggleLogging();
					logBtn->draw();
				}
				else if (resetAllBtn.isPushed(touchX,touchY)) {
					#ifdef DEBUG
						if (Serial) {
							Serial.println(F("resetAllBtn isPushed"));
						}
					#endif
					resetAllBtn.push();
					resetAll();
					resetAllBtn.draw();
				}

				else{
					for (int i=0; i<NUMINPUTS; i++) {
						if (inputButtons[i]->isPushed(touchX,touchY)) {
							#ifdef DEBUG
								if (Serial) {
									Serial.print(inputButtons[i]->getLabel());
									Serial.println(F("Btn isPushed"));
								}
							#endif
							inputButtons[i]->push();
							drawIndividualSensorMenu(inputButtons[i]->obj);

							menu.draw(); //redraw this menu
							break;
						}
					}
				}

				//some buttons seemed to get pushed again for some reason...
				emptyTouchBuffer();

			}
			else {
				#ifdef DEBUG
					if (Serial) {
						Serial.println(F("ignoring menu touch, to soon after last touch"));
					}
				#endif
			}
		}
	} // close whiLe
}

void drawIndividualSensorMenu(SensorInput *si) {
	#ifdef DEBUG
	if (Serial) {
		Serial.print(F("drawMainIndividualSensorMenu(...) "));
		Serial.println(si->label);
	}
	#endif

	tft.fillScreen(MENUCOLOUR);
	tft.setTextSize(2);

	TouchButton backBtn(CENTER_X1, CENTER_Y1-30, "Back");
	backBtn.draw();



	TouchButton resetBtn(CENTER_X2, CENTER_Y1-30,"Reset", si);
	resetBtn.draw();

	//IMP draw controls for each of the inputs for a sensor

	//IMP draw specialized controls for loadcell and linearEnc???

	while (1) {
		if (!(ts.bufferEmpty())) {
			if (parseTouchBoilerPlate()) {

				if (backBtn.isPushed(touchX,touchY)) {
					#ifdef DEBUG
						if (Serial) {
							Serial.println(F("backBtn isPushed"));
						}
					#endif
					backBtn.push();
					break; //break out of this menu's touch loop
				}
				else if (resetBtn.isPushed(touchX,touchY)) {
					#ifdef DEBUG
						if (Serial) {
							Serial.println(F("resetAllBtn isPushed"));
						}
					#endif
					resetBtn.push();
					resetBtn.obj->reset();
					resetBtn.draw();
				}

				//IMP implement touch buttons for all the SensorInput variables we want to control

				//some buttons seemed to get pushed again for some reason...
				emptyTouchBuffer();

			}
			else {
				#ifdef DEBUG
					if (Serial) {
						Serial.println(F("ignoring menu touch, to soon after last touch"));
					}
				#endif
			}
		}
	}//end while

}

//-----------------------------------------------------------------------------

void pollSensors() {

	//loop through sensorInputs
	for (int i=0; i<NUMINPUTS; i++) {

		if(sensorInputs[i]->isEnabled()) {

			int newReading = sensorInputs[i]->poll();
			sensorInputs[i]->updateDataAndRedrawStats(newReading);
			sensorInputs[i]->updateViz();
		}
	}
}

void logError() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("...log error"));
		}
	#endif
	Display::device->setTextSize(4);
	Display::device->setCursor(130, 140);
	Display::device->print("LOG ERROR");
}

/** one or both inputs may be deleted depending on size */
char* strSafeCat(char *str1, int safeSize, char* str2) {
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	if (safeSize >= (len1+len2)) {
		strcat(str1, str2);
		free(str2);
		return str1;
	}
	else {
		char *output = (char*) malloc((len1+len2+1)*sizeof(char));
		sprintf(output, "%s", str1);
		free(str1);
		strcat(output, str2);
		free(str2);
		return output;
	}
}

void logOutput(){

	if (logging==true) {

		int size = 128;
		char *logStr = (char*) malloc(size*sizeof(char));
		sprintf(logStr, "%04d/%02d/%02d %02d:%02d:%02d, %d", year(), month(), day(), hour(), minute(), second(), micros());

		for (int i=0; i<NUMINPUTS; i++) {
			if(sensorInputs[i]->isEnabled()) {
				logStr = strSafeCat(logStr, size-1, sensorInputs[i]->logout());
			}
		}

		#ifdef DEBUG
			if (Serial) {
				Serial.println(logStr);
			}
		#endif

		//write to sd card
		if (!logFile.open(logFileName, O_CREAT | O_APPEND | O_WRITE)) {
			logError();
			free(logStr);
			while(1) {}
		}
		logFile.println(logStr);
		logFile.close();
		free(logStr);
	}

}

void resetAll() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println("resetAll()");
		}
	#endif
	for (int i=0; i<NUMINPUTS; i++) {
		if(sensorInputs[i]->isEnabled()) { //XXX should we do for all of them anyways??
			sensorInputs[i]->reset();
		}
	}

	//XXX what about starting a new log file? should we ask the user with a pop up window?
	//draw yes/no screen
	//if yes
	//stop logging
	//start logging
	//drawMainMenu()
}

///** zero inputs for sensors that support it */
//void calibrateAll() {
//	#ifdef DEBUG
//	if (Serial) {
//		Serial.println("calibrate... ");
//	}
//	#endif
//
//	for (int i=0; i<NUMINPUTS; i++) {
//		if(sensorInputs[i]->isEnabled()) { //XXX should we do for all of them anyways??
//			sensorInputs[i]->calibrate(); /** only some sensors implement calibrate */
//		}
//	}
//
//	delay(500);
//
//	#ifdef DEBUG
//		if (Serial) {
//			Serial.println(F("... and reset complete"));
//		}
//	#endif
//	return;
//}

int startLogging() {
	#ifdef DEBUG
		if (Serial) {
			Serial.print(F("starting new logfile called "));
		}
	#endif

	//filename limited by SDFat library to 8 chars name and 3 chars extension
	sprintf(logFileName, "%02d%02d%02d%02d.csv", day(), hour(), minute(), second());
	#ifdef DEBUG
		if (Serial) {
			Serial.println(logFileName);
		}
	#endif

	if( !logFile.open(logFileName, O_CREAT | O_APPEND | O_WRITE) ){
		logError();
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("...log file error"));
			}
		#endif
		delay(1000);
		return 0;
	}
	else { //add column header to new logfile
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("datetime, microsr"));
			}
		#endif
		logFile.print("datetime, micros");
		for (int i=0; i<NUMINPUTS; i++) {
			if(sensorInputs[i]->isEnabled()) {
				char headerStr[64];
				sprintf(headerStr, ", cycles-%d, raw.latest-%d",i+1 ,i+1);
				logFile.print(headerStr);
				#ifdef DEBUG
					if (Serial) {
						Serial.print(headerStr);
					}
				#endif
//				delete headerStr;
			}
		}
		logFile.println("");
		#ifdef DEBUG
			if (Serial) {
				Serial.println("");
			}
		#endif
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
		if (Serial) {
			Serial.println(F("empty ts buffer"));
		}
	#endif

	}
}

int parseTouchBoilerPlate() {
	// Retrieve a point
	TS_Point p = ts.getPoint();
	// Scale using the calibration 	#'s
	// and rotate coordinate system
	#ifdef DEBUG
	if (Serial) {
		Serial.print(F("\noriginal touch at "));
		Serial.print(p.x);
		Serial.print(F(", "));
		Serial.print(p.y);
	}
	#endif
	p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
	p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
	touchY = tft.height() - p.x; //YES, THESE MAP BACKWARDS!!! TY = PX..
	touchX = p.y;                //YES, THESE MAP BACKWARDS!!! TX = PY

	#ifdef DEBUG
	if (Serial) {
		Serial.print(F(". mapped to "));
		Serial.print(touchX);
		Serial.print(F(", "));
		Serial.println(touchY);
	}
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


/** would be great to reuse this with a different*/
void parseTouch() {

	if (parseTouchBoilerPlate()) {

		drawMainMenu(); /** menu handles all it's own touch and navigation */
		emptyTouchBuffer();
		drawMainScreen();

	}
	else {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("ignoring touch, to soon after last touch"));
		}
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
		#ifdef DEBUG
		if (Serial) {
			Serial.println(F("set sync provider"));
		}
	#endif

	//setup color lcd
	tft.begin();
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("tft started"));
		}
	#endif

	#if defined(__TFT_ILI9340__) && (defined(__MK20DX128__) || defined(__MK20DX256__))
		tft.setBitrate(24000000);
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("setBitrate(24000000)"));
			}
		#endif
	#endif
	tft.fillScreen(YELLOW);
	tft.fillScreen(GREEN);
	tft.fillScreen(RED);
	tft.fillScreen(MAGENTA);
	tft.fillScreen(BACKGROUNDCOLOUR);
	tft.setTextColor(TEXTCOLOUR);
	tft.setRotation(1);

	while(!ts.begin()){
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("Unable to start touchscreen."));
			}
		#endif
		tft.println(F("Unable to start touchscreen."));
		delay(500);
	}

	#ifdef DEBUG
		if (Serial) {
			Serial.println("Touchscreen started.");
		}
	#endif

	SdFile::dateTimeCallback(dateTime);
	while (!sd.begin(sdCS, SPI_HALF_SPEED)) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("Insert microSD card"));
			}
		#endif
		tft.println(F("Insert microSD card"));
		delay(500);
	}

	display = new Display(&tft, &ts);

	sensorInputs[0] = new ForceMeter(0, -1);
	sensorInputs[1] = new LinearEncoder(5, 6);
	sensorInputs[2] = new SensorInput(16, ANALOG);
	sensorInputs[3] = new SensorInput(17, DIGITAL);

	//loop through sensorInputs
	for (int i=0; i<NUMINPUTS; i++) {
		display->add( &(sensorInputs[i]->shortTermDisplay) );
	}

	drawMainScreen();

	#ifdef DEBUG
	if (Serial) {
		Serial.println(F("setup complete"));
	}
	#endif

} //end setup()


void loop() {

	delay(2000); //FIXME debug code

	pollSensors();

	logOutput();

	if (!(ts.bufferEmpty())) { //this should stay at the beginning or end of a loop
		parseTouch();
	}

	//FIXME doing something on the SD card makes the spi for the screen switch back to a faster mode...

} //end loop()

