#ifndef DISPLAYELEMENT
#define DISPLAYELEMENT

#include "DisplayElement.h"
#include "universalLogger.h"
#include "Display.h"

	DisplayElement::DisplayElement() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement()"));
			}
		#endif
	}

	//** for left aligned things */
	DisplayElement::DisplayElement(int startX, int startY) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement(...)"));
			}
		#endif
		this->startX=startX;
		this->startY=startY;
	}

	/** for center aligned things */
	DisplayElement::DisplayElement(int centerX, int centerY, int w, int h) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement(...)"));
			}
		#endif
		this->centerX=centerX;
		this->centerY=centerY;
		this->w=w;
		this->h=h;
		this->startX=centerX-w/2;
		this->startY=centerY-h/2;
	}

	DisplayElement::~DisplayElement() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("~DisplayElement()"));
			}
		#endif
	} //shut eclipse up

	void DisplayElement::locateCenter(int centerX, int centerY) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement::locate(...)"));
			}
		#endif
		this->centerX=centerX;
		this->centerY=centerY;
		this->startX=centerX-w/2;
		this->startY=centerY-h/2;
	}

	void DisplayElement::locateCenterAndSize(int centerX, int centerY, int w, int h) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement::locateAndSize(...)"));
			}
		#endif
		this->centerX=centerX;
		this->centerY=centerY;
		this->w=w;
		this->h=h;
		this->startX=centerX-w/2;
		this->startY=centerY-h/2;
	}

	void DisplayElement::locateLeft(int startX, int startY) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement::locate(...)"));
			}
		#endif
		this->startX=startX;
		this->startY=startY;
		this->centerX=startX+w/2;
		this->centerY=startY+h/2;
	}

	void DisplayElement::locateLeftAndSize(int startX, int startY, int w, int h) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement::locateAndSize(...)"));
			}
		#endif
		this->startX=startX;
		this->startY=startY;
		this->w=w;
		this->h=h;
		this->centerX=startX+w/2;
		this->centerY=startY+h/2;
	}

	void DisplayElement::sizeAndSetCenter(int w, int h) {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("DisplayElement::size( "));
				Serial.print(w);
				Serial.print(F(", "));
				Serial.print(h);
				Serial.println(F(" )"));
			}
		#endif
		this->w=w;
		this->h=h;
		if (startX==0 && startY==0) {
			this->startX=centerX-w/2;
			this->startY=centerY-h/2;
		}

		this->centerX = (startX+w/2 < SCREENWIDTH)? startX+w/2 : SCREENWIDTH;
		this->centerY= (startY+h/2 < SCREENHEIGHT)? startY+h/2 : SCREENHEIGHT;
	}

	void DisplayElement::sizeAndSetStart(int w, int h) {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("DisplayElement::size( "));
				Serial.print(w);
				Serial.print(F(", "));
				Serial.print(h);
				Serial.println(F(" )"));
			}
		#endif
		this->w=w;
		this->h=h;
		this->startX = (centerX-w/2 > 0)? centerX-w/2 : 0;
		this->startY= (centerY-h/2)? centerY-h/2 : 0;
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
	void DisplayElement::clear(int colour /**=BACKGROUNDCOLOUR*/) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement::clear(...)"));
			}
		#endif
		Display::device->fillRect(startX , startY, w, h, colour);
	}

#endif
