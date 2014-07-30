#ifndef DISPLAY
#define DISPLAY

#include "Display.h"
#include "universalLogger.h"
#include "SensorDisplay.h"
#include <Adafruit_ILI9340.h>
//#include <Adafruit_STMPE610.h> //library doesn't have ifndef check in the header....

	int                Display::numSDs;
	int                Display::lastNumSDs;
	SensorDisplay     *Display::SDs;
	int                Display::numRegions;

	Adafruit_ILI9340  *Display::device;
	Adafruit_STMPE610 *Display::touch;

Display::Display(Adafruit_ILI9340 *tft, Adafruit_STMPE610 *ts, int regions) {
	#ifdef DEBUG
		Serial.println(F("Display(...)"));
	#endif
	device     = tft;
	touch      = ts;
	numSDs     = 0;
	lastNumSDs = -1;
	numRegions = regions;
	SDs = new SensorDisplay[regions];

	 #ifdef DEBUG
		Serial.print("tft height: ");
		 Serial.print(device->`height());
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
	   #endif
}

Display::~Display() {
	#ifdef DEBUG
		Serial.println(F("~Display()"));
	#endif
	delete SDs;
}

int Display::hasSpace() {
	#ifdef DEBUG
		Serial.print(F("Display::hasSpace()"));
	#endif
	return numSDs<numRegions;
}

int Display::remove(SensorDisplay *sd) {
	#ifdef DEBUG
		Serial.println(F("Display::remove(...)"));
	#endif
	//loop through array of active SensorDisplays
	for (int i=0; i<numRegions; i++ ) {
		//match the one we're looking for
		if (SDs[i] == sd && sd->enabled) {

			//disable it
			sd->enabled = 0;

			//set this memory address in the array as blank
			SDs[i] = NULL; //TODO figure out best way to have empty spots in the array of SensorDisplay's or use a linked list :/
			lastNumSDs = numSDs;
			numSDs--;
			updateDisplayLayout();
			return true;
		}
	}
	return false;
}

void Display::updateDisplayLayout() {
	#ifdef DEBUG
		Serial.println(F("Display::changeDisplayLayout()"));
	#endif

	switch (numSDs) {

		case 1:
			// single full screen SensorDisplay
			SDs[0].locateAndSize(CENTER_X, CENTER_Y, FULLWIDTH, FULLHEIGHT);
			SDs[0].viz->doubleWidth = true;
			break;

		case 2:
			//two equal size tall SensorDisplay's
			SDs[0].locateAndSize(CENTER_X1, CENTER_Y, HALFWIDTH, FULLHEIGHT);
			SDs[0].viz->doubleWidth = false;
			SDs[1].locateAndSize(CENTER_X2, CENTER_Y, HALFWIDTH, FULLHEIGHT);
			SDs[1].viz->doubleWidth = false;
			break;

		case 3:
			//first Sensor Display is full height
			SDs[0].locateAndSize(CENTER_X1, CENTER_Y, HALFWIDTH, FULLHEIGHT);
			SDs[0].viz->doubleWidth = false;
			//other two are half height
			SDs[1].locateAndSize(CENTER_X2, CENTER_Y1, HALFWIDTH, HALFHEIGHT);
			SDs[1].viz->doubleWidth = false;
			SDs[2].locateAndSize(CENTER_X2, CENTER_Y2, HALFWIDTH, HALFHEIGHT);
			SDs[2].viz->doubleWidth = false;
			break;

		case 4:
			//four equal size SensorDisplay's
			SDs[0].locateAndSize(CENTER_X1, CENTER_Y1, HALFWIDTH, HALFHEIGHT);
			SDs[0].viz->doubleWidth = false;
			SDs[1].locateAndSize(CENTER_X2, CENTER_Y1, HALFWIDTH, HALFHEIGHT);
			SDs[1].viz->doubleWidth = false;
			SDs[2].locateAndSize(CENTER_X1, CENTER_Y2, HALFWIDTH, HALFHEIGHT);
			SDs[2].viz->doubleWidth = false;
			SDs[3].locateAndSize(CENTER_X2, CENTER_Y2, HALFWIDTH, HALFHEIGHT);
			SDs[3].viz->doubleWidth = false;
			break;
	}
}

int Display::add(SensorDisplay *sd) {
	#ifdef DEBUG
		Serial.println(F("Display::add(...)"));
	#endif
	if (!hasSpace()) {
		return false;
	}
	for (int i=0; i<numRegions; i++ ) {
		// see if there's space
		if (SDs[i] == NULL) { //TODO figure out best way to have empty spots in the array of SensorDisplay's or use a linked list :/

			//enable it
			sd->enabled = 1;

			//set this memory address in the array
			SDs[i] = sd;
			lastNumSDs = numSDs;
			numSDs++;
			updateDisplayLayout();
			return true;
		}
	}

	return false;
}

#endif
