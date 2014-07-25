#ifndef STAT_H
#define STAT_H

#include "universalLogger.h"
#include "DisplayElement.h"
#include "Display.h"

class Stat : public DisplayElement {
  protected:

  public:
    int colour = TEXTCOLOUR;
    char *label;
    char *lastValue;

    Stat(int x=0, int y=0, int w=0, int h=0, char* label="") :
    	DisplayElement {x,y,w,h}
    {
		#ifdef DEBUG
		  Serial.println(F("Stat(...) "));
		#endif
        this->label = label;
        lastValue = "";
    }

    void draw() {
		#ifdef DEBUG
			Serial.print(F("Stat::draw() "));
			Serial.println(F(label));
		#endif

		//TODO erase space for label and value, then print both
		int startX = x-w/2;
		int startY = y-h/2;
		Display::device->fillRect(startX, startY, w, h, ERASECOLOUR);
		int newY = startY + (h - CHARHEIGHT)/2;
		int newX = startX + (w - ((strlen(label)+strlen(lastValue)) * CHARWIDTH))/2;
		Display::device->setCursor(newX, newY);
		Display::device->print(label);
		Display::device->print(lastValue);
    }

    void redraw() {
		#ifdef DEBUG
		  Serial.print(F("Stat::redraw() "));
		  Serial.println(F(label));
		#endif

		//TODO erase space for value only, leaving label alone, assuming it's still there, and print the lastValue only
		int startX = x-w/2;
		int startY = y-h/2;
		Display::device->fillRect(startX, startY, w, h, ERASECOLOUR);
		int newY = startY + (h - CHARHEIGHT)/2;
		int newX = startX + (w - (strlen(label) * CHARWIDTH))/2;
		Display::device->setCursor(newX, newY);
		Display::device->print(label);
    }

};
#endif
