#ifndef TOUCHARROW_H
#define TOUCHARROW_H

#include "universalLogger.h"
#include "TouchElement.h"
#include "Display.h"

#define BUTTONCOLOUR WHITE
#define BUTTONPUSHEDCOLOUR MAGENTA

class TouchArrow : public TouchElement{
  protected:
	int dir           = 0;
    int colour        = BUTTONCOLOUR;
    int shaddow       = 2;
    int shaddowColour = BLACK;

  public:

    TouchArrow(int dir) :
    	TouchElement { 0,0,0,0 }
    {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.print(F("TouchArrow( "));
			  Serial.print(dir);
			  Serial.println(F(" )"));
		  }
		#endif
		this->dir = dir;
		w=h=CHARHEIGHT*(MENUTEXTSIZE+2);
    }

    void draw() {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchArrow::draw() "));
				Serial.println(dir);
			}
		#endif
		Display::device->fillRect(startX-shaddow, startY-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);

		if (dir==0) { //up
			Display::device->drawLine(startX+w/2-shaddow, startY, startX-shaddow, startY+h, shaddowColour); // down

			Display::device->drawFastHLine(startX, startY+h, w, colour); // Top
			Display::device->drawLine(startX+w/2, startY, startX, startY+h, colour); // down
			Display::device->drawLine(startX+w/2, startY, startX+w, startY+h, colour); // down
		}
		else if (dir==2) { //down
			Display::device->drawFastHLine(startX -shaddow, startY -shaddow  , w, shaddowColour); // Top
			Display::device->drawLine(startX -shaddow, startY -shaddow, startX+w/2, startY+h, shaddowColour); // down

			Display::device->drawFastHLine(startX, startY, w, colour); // Top
			Display::device->drawLine(startX, startY, startX+w/2, startY+h, colour); // down
			Display::device->drawLine(startX+w, startY, startX+w/2, startY+h, colour); // down
		}
    }

	/** UI press effect only for visual effect */
	void push() {
		#ifdef DEBUG
		if (Serial) {
				Serial.print(F("TouchArrow::push() "));
				Serial.println(dir);
		}
		#endif
		Display::device->fillRect(startX-shaddow, startY-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);

		if (dir==0) { //up
			Display::device->drawFastHLine(startX, startY+h, w, shaddowColour); // Top
			Display::device->drawLine(startX+w/2, startY, startX, startY+h, shaddowColour); // down
			Display::device->drawLine(startX+w/2, startY, startX+w, startY+h, shaddowColour); // down
		}
		else if (dir==2) { //down
			Display::device->drawFastHLine(startX, startY, w, shaddowColour); // Top
			Display::device->drawLine(startX, startY, startX+w/2, startY+h, shaddowColour); // down
			Display::device->drawLine(startX+w, startY, startX+w/2, startY+h, shaddowColour); // down
		}

		/** delay ensures pushed visual state visible to
		* user, not worried about stopping everything since
		* this will be used in a menu screen */
		delay(500);
    }

};
#endif
