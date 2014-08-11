#ifndef TOUCHBUTTON_H
#define TOUCHBUTTON_H

#include "universalLogger.h"
#include "TouchElement.h"
#include "Display.h"

#define BUTTONCOLOUR WHITE
#define BUTTONPUSHEDCOLOUR MAGENTA

class TouchButton : public TouchElement{
  protected:
    char *label       = "";
    int r             = 4;
    int colour        = BUTTONCOLOUR;
    int shaddow       = 2;
    int shaddowColour = BLACK;

  public:

    SensorInput *obj = nullptr;

    TouchButton() {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.print(F("TouchButton(...) "));
			  Serial.println(F(label));
		  }
		#endif
    }

    TouchButton(char* label=""){
		#ifdef DEBUG
		  if (Serial) {
			  Serial.print(F("TouchButton( label ) "));
			  Serial.println(F(label));
		  }
		#endif
		this->label = label;
    }

    TouchButton(int centerX, int centerY, char* label="") :
    	TouchElement {
    		centerX,
    		centerY,
    		strlen(label)*CHARWIDTH*MENUTEXTSIZE+CHARWIDTH*2,
    		CHARHEIGHT*(MENUTEXTSIZE+2)
    	}
    {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.print(F("TouchButton(...) "));
			  Serial.println(F(label));
		  }
		#endif
		this->label = label;
    }
    template <class T>
    TouchButton(int centerX, int centerY, char* label="", T *obj=nullptr) :
    	TouchElement {
    		centerX,
    		centerY,
    		strlen(label)*CHARWIDTH*MENUTEXTSIZE+CHARWIDTH*2,
    		CHARHEIGHT*(MENUTEXTSIZE+2)
    	}
    {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.print(F("TouchButton(...) "));
			  Serial.println(F(label));
		  }
		#endif
		this->label = label;
		this->obj = obj;
    }
    TouchButton(int centerX, int centerY, int w, int h, char* label="") :
    	TouchElement {
			centerX,
			centerY,
			strlen(label)*CHARWIDTH*MENUTEXTSIZE+CHARWIDTH*2,
			CHARHEIGHT*(MENUTEXTSIZE+1)
    	}
    {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.print(F("TouchButton(...) "));
			  Serial.println(F(label));
			}
		#endif
		this->label = label;
    }

    void setLabel(char *newLabel) {
      #ifdef DEBUG
        if (Serial) {
          Serial.print(F("TouchButton::setLabel() "));
          Serial.print(F(label));
          Serial.print(F(" to "));
          Serial.println(F(newLabel));
        }
  	  #endif
//      free (label); //FIXME memory leak???? code was hanging here, and with delete
      label = newLabel;
      w = strlen(label)*CHARWIDTH*MENUTEXTSIZE+CHARWIDTH*2;
      h = CHARHEIGHT*(MENUTEXTSIZE+2);
    }
    char* getLabel() {
      #ifdef DEBUG
        if (Serial) {
          Serial.print(F("TouchButton::getLabel() "));
          Serial.println(F(label));
        }
  	  #endif
      return label;
    }

    void draw() {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchButton::draw() "));
				Serial.println(F(label));
			}
		#endif
		Display::device->fillRect(startX-shaddow, startY-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);
		Display::device->drawFastHLine(startX+r -shaddow, startY -shaddow  , w-2*r, shaddowColour); // Top
		Display::device->drawFastVLine(startX -shaddow  , startY+r -shaddow, h-2*r, shaddowColour); // Left
		Display::device->drawCircleHelper(startX+r -shaddow, startY+r -shaddow, r, 1, shaddowColour); //top left corner
		Display::device->drawCircleHelper(startX+w-r-1 -shaddow, startY+r -shaddow, r, 2, shaddowColour); //top right corner
		Display::device->drawCircleHelper(startX+r -shaddow, startY+h-r-1 -shaddow, r, 8, shaddowColour); //bottom left corner
		Display::device->drawRoundRect(startX, startY, w, h, r, colour);
		int newY = startY + (h - CHARHEIGHT*MENUTEXTSIZE)/2;
		int newX = startX + (w - (strlen(label) * CHARWIDTH * MENUTEXTSIZE))/2;
		Display::device->setCursor(newX, newY);
		Display::device->print(label);
    }

    void push() {
      #ifdef DEBUG
        if (Serial) {
    			Serial.print(F("TouchButton::push() "));
    			Serial.println(F(label));
        }
      #endif
      Display::device->fillRect(startX-shaddow, startY-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);
      Display::device->drawRoundRect(startX, startY, w, h, r, shaddowColour);
      int newY = startY + (h - CHARHEIGHT * MENUTEXTSIZE)/2;
      int newX = startX + (w - (strlen(label) * CHARWIDTH * MENUTEXTSIZE))/2;
      Display::device->setCursor(newX, newY);
      Display::device->print(label);
      delay(500);
    }

};
#endif
