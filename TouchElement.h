#ifndef TOUCH_H
#define TOUCH_H

#include "universalLogger.h"
#include "DisplayElement.h"

class TouchElement : public DisplayElement {

  public:

	TouchElement() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("Touch()"));
			}
		#endif
	}


    TouchElement(int centerX, int centerY, int w, int h) :
    	DisplayElement {centerX,centerY,w,h}
    {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("Touch(...)"));
			}
		#endif

    }

    virtual int isPushed(int tx, int ty) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("Touch::isPushed()"));
			}
		#endif
		return tx > centerX - w/2  &&  tx < centerX + w/2
			 && ty > centerY - h/2  &&  ty < centerY + h/2;
    }

    virtual void push() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("Touch::push() not implemented yet for this class"));
			}
		#endif
    }

};
#endif
