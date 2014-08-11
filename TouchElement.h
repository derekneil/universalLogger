#ifndef TOUCH_H
#define TOUCH_H

#include "universalLogger.h"
#include "DisplayElement.h"

class TouchElement : public DisplayElement {

  public:

	TouchElement() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("TouchElement()"));
			}
		#endif
	}

    TouchElement(int startX, int startY) :
    	DisplayElement {startX, startY}
    {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchElement( "));
				Serial.print(startX);
				Serial.print(F(", "));
				Serial.print(startY);
				Serial.println(F(" )"));
			}
		#endif

    }


    TouchElement(int centerX, int centerY, int w, int h) :
    	DisplayElement {centerX,centerY,w,h}
    {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("TouchElement( "));
				Serial.print(centerX);
				Serial.print(F(", "));
				Serial.print(centerY);
				Serial.print(F(", "));
				Serial.print(w);
				Serial.print(F(", "));
				Serial.print(h);
				Serial.println(F(" )"));
			}
		#endif

    }

    virtual int isPushed(int tx, int ty) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("TouchElement::isPushed( "));
				Serial.print(tx);
				Serial.print(F(", "));
				Serial.print(ty);
				Serial.println(F(" )"));
			}
		#endif
		return tx > startX  &&  tx < startX + w
			 && ty > startY  &&  ty < startY + h;
    }

    virtual void push() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("TouchElement::push() not implemented yet for this class"));
			}
		#endif
    }

};
#endif
