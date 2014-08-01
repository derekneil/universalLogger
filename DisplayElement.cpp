#ifndef DISPLAYELEMENT
#define DISPLAYELEMENT

#include "DisplayElement.h"
#include "universalLogger.h"
#include "Display.h"

	DisplayElement::DisplayElement(){
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement()"));
			}
		#endif
	}

	DisplayElement::DisplayElement(int centerX, int centerY, int w, int h)
	{
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement(...)"));
			}
		#endif
		this->centerX=centerX;
		this->centerY=centerY;
		this->w=w;
		this->h=h;
	}

	DisplayElement::~DisplayElement() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("~DisplayElement()"));
			}
		#endif
	} //shut eclipse up

	void DisplayElement::locate(int centerX, int centerY)
	{
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement::locate(...)"));
			}
		#endif
		this->centerX=centerX;
		this->centerY=centerY;
	}

	void DisplayElement::locateAndSize(int centerX, int centerY, int w, int h)
	{
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement::locateAndSize(...)"));
			}
		#endif
		this->centerX=centerX;
		this->centerY=centerY;
		this->w=w;
		this->h=h;
	}

	void DisplayElement::draw() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement::draw() not implemented yet for this class"));
			}
		#endif
	}

	void DisplayElement::redraw() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement::redraw() not implemented yet for this class"));
			}
		#endif
	}

	/** assumes upper left 0,0 start point */
	void DisplayElement::clear(int backgroundColour) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement::clear(...)"));
			}
		#endif
		int startX = centerX - w/2;
		int startY = centerY - h/2;
		Display::device->fillRect(startX , startY, w, h, backgroundColour);
	}

#endif
