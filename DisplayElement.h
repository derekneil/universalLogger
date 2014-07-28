#ifndef DISPLAYELEMENT_H
#define DISPLAYELEMENT_H

#include "Display.h"

class DisplayElement {
  protected:
	int x=0; // center
	int y=0; // coordinates
	int w=0;
	int h=0;
  public:
	DisplayElement(){
		#ifdef DEBUG
			Serial.println(F("DisplayElement()"));
		#endif
	}

	DisplayElement(int x, int y, int w, int h)
	{
		#ifdef DEBUG
			Serial.println(F("DisplayElement(...)"));
		#endif
		this->x=x;
		this->y=y;
		this->w=w;
		this->h=h;
	}

	virtual ~DisplayElement() {
		#ifdef DEBUG
			Serial.println(F("DisplayElement destructor"));
		#endif
	} //shut eclipse up

	virtual void locate(int x, int y)
	{
		#ifdef DEBUG
			Serial.println(F("DisplayElement::locate(...)"));
		#endif
		this->x=x;
		this->y=y;
	}

	virtual void locateAndSize(int x, int y, int w, int h)
	{
		#ifdef DEBUG
			Serial.println(F("DisplayElement::locateAndSize(...)"));
		#endif
		this->x=x;
		this->y=y;
		this->w=w;
		this->h=h;
	}

	virtual void draw() {
		#ifdef DEBUG
			Serial.println(F("DisplayElement::draw() not implemented yet for this class"));
		#endif
	}

	virtual void redraw() {
		#ifdef DEBUG
			Serial.println(F("DisplayElement::redraw() not implemented yet for this class"));
		#endif
	}

	/** assumes upper left 0,0 start point */
	virtual void clear(int backgroundColour=0) {
		#ifdef DEBUG
			Serial.println(F("DisplayElement::clear(...)"));
		#endif
		int startX = x - w/2;
		int startY = y - h/2;
		Display::device->fillRect(startX , startY, w, h, backgroundColour);
	}
};
#endif
