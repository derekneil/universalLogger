#ifndef DISPLAY
#define DISPLAY

#include "Display.h"
#include "universalLogger.h"
#include "SensorDisplay.h"
#include <Adafruit_ILI9340.h>
#include <Adafruit_STMPE610.h>


	Adafruit_ILI9340  *Display::device;
	Adafruit_STMPE610 *Display::touch;

Display::Display(Adafruit_ILI9340 *tft, Adafruit_STMPE610 *ts) {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Display(...)"));
		}
	#endif
	device     = tft;
	touch      = ts;

	 #ifdef DEBUG
		if (Serial) {
			Serial.print("tft height: ");
			Serial.print(device->height());
			if (device->height()==SCREENHEIGHT) {
				Serial.println(" as expected :)");
			}
			else {
				Serial.print(" does not match ");
				Serial.print(SCREENHEIGHT);
				Serial.println(" DANGER!!!!");
			}
			Serial.print("tft width: ");
			Serial.print(device->width());
			if (device->width()==SCREENWIDTH) {
				Serial.println(" as expected :)");
			}
			else {
				Serial.print(" does not match ");
				Serial.print(SCREENWIDTH);
				Serial.println(" DANGER!!!!");
			}
	   	}
	#endif
}

Display::~Display() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("~Display()"));
		}
	#endif
//	delete SDs;
}

int Display::hasSpace() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Display::hasSpace()"));
		}
	#endif
	return numSDs<numRegions;
}

int Display::isEmpty() {
	return numSDs == 0;
}

int Display::remove(SensorDisplay *sd) {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Display::remove(...)"));
		}
	#endif
	//loop through vector of active SensorDisplays to find it
	for (int i=0; i<numRegions; i++ ) {
		//match the one we're looking for
		if (sd->enabled && SDs[i] == sd) {

			//disable it
			sd->enabled = 0;

			//erase this pointer to it
			SDs.erase( (SDs.begin())+i );
			numSDs--;
			updateDisplayLayout();
			return true;
		}
	}
	return false;
}

void Display::updateDisplayLayout() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Display::updateDisplayLayout()"));
		}
	#endif

	switch (numSDs) {

		case 1:
			// single full screen SensorDisplay
			SDs[0]->locateCenterAndSize(CENTER_X, CENTER_Y, FULLWIDTH, FULLHEIGHT);
			SDs[0]->viz->doubleWidth = true;
			break;

		case 2:
			//two equal size tall SensorDisplay's
			SDs[0]->locateCenterAndSize(CENTER_X1, CENTER_Y, HALFWIDTH, FULLHEIGHT);
			SDs[0]->viz->doubleWidth = false;
			SDs[1]->locateCenterAndSize(CENTER_X2, CENTER_Y, HALFWIDTH, FULLHEIGHT);
			SDs[1]->viz->doubleWidth = false;
			break;

		case 3:
			//first Sensor Display is full height
			SDs[0]->locateCenterAndSize(CENTER_X1, CENTER_Y, HALFWIDTH, FULLHEIGHT);
			SDs[0]->viz->doubleWidth = false;
			//other two are half height
			SDs[1]->locateCenterAndSize(CENTER_X2, CENTER_Y1, HALFWIDTH, HALFHEIGHT);
			SDs[1]->viz->doubleWidth = false;
			SDs[2]->locateCenterAndSize(CENTER_X2, CENTER_Y2, HALFWIDTH, HALFHEIGHT);
			SDs[2]->viz->doubleWidth = false;
			break;

		case 4:
			//four equal size SensorDisplay's
			SDs[0]->locateCenterAndSize(CENTER_X1, CENTER_Y1, HALFWIDTH, HALFHEIGHT);
			SDs[0]->viz->doubleWidth = false;
			SDs[1]->locateCenterAndSize(CENTER_X2, CENTER_Y1, HALFWIDTH, HALFHEIGHT);
			SDs[1]->viz->doubleWidth = false;
			SDs[2]->locateCenterAndSize(CENTER_X1, CENTER_Y2, HALFWIDTH, HALFHEIGHT);
			SDs[2]->viz->doubleWidth = false;
			SDs[3]->locateCenterAndSize(CENTER_X2, CENTER_Y2, HALFWIDTH, HALFHEIGHT);
			SDs[3]->viz->doubleWidth = false;
			break;
	}
}

int Display::add(SensorDisplay *sd) {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Display::add(...)"));
		}
	#endif
	if (!hasSpace()) {
		return false;
	}

	//else enable it
	sd->enabled = true;

	//add it to the vector of sensor displays
	SDs.push_back(sd);
	numSDs++;
	updateDisplayLayout();
	return true;
}

#endif
