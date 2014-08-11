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

	//** for left aligned things */
	DisplayElement::DisplayElement(int startX, int startY)
	{
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("DisplayElement(...)"));
			}
		#endif
		this->startX=startX;
		this->startY=startY;
	}

	/** for center aligned things */
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

	void DisplayElement::locate(int centerX, int centerY)
	{
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
		this->startX=centerX-w/2;
		this->startY=centerY-h/2;
	}

	/** should only be called on left justified elements
	 * also sets startX&Y if they're 0 or defaults to centerX&Y */
	void DisplayElement::size(int w, int h)
	{
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

		/** FIXME this can' be used transparently with left
		 * and centered things can it? Maybe rename to
		 * sizeLeftJustified() */
		else { //if (centerX==0 && centerY==0) {
			this->centerX=startX+w/2;
			this->centerY=startY+h/2;
		}
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
