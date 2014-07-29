#ifndef TOUCH_H
#define TOUCH_H

#include "DisplayElement.h"

class TouchElement : public DisplayElement {
  protected:

  public:
    TouchElement(int x, int y, int w, int h, char* label="") : DisplayElement {x,y,w,h}
    {
		#ifdef DEBUG
		  Serial.print(F("Touch()"));
		#endif

    }

    virtual int isPushed(int tx, int ty) {
		#ifdef DEBUG
		  Serial.print(F("Touch::isPushed()"));
		#endif
		return tx > centerX - w/2  &&  tx < centerX + w/2
			 && ty > centerY - h/2  &&  ty < centerY + h/2;
    }

    virtual void push() {
      #ifdef DEBUG
        Serial.print(F("Touch::push() not implemented yet for this class"));
      #endif
    }

  private:

};
#endif
