#ifndef TOUCHBUTTON_H
#define TOUCHBUTTON_H

#include "universalLogger.h"
#include "TouchElement.h"
#include "Display.h"
#include "Enableable.h"

#define BUTTONCOLOUR WHITE
#define BUTTONPUSHEDCOLOUR MAGENTA

class TouchButton : public TouchElement{
  protected:
    char *label       = "";
    int r             = 4;
    int colour        = BUTTONCOLOUR;
    int shaddow       = 2;
    int shaddowColour = BLACK;

	size_t buildW(char* label) {
		return strlen(label) * CHARWIDTH * MENUTEXTSIZE + CHARWIDTH * 2;
	}
	size_t buildH(char* label) {
		return CHARHEIGHT*(MENUTEXTSIZE+2);
	}

	void printLabel(int textX, int textY) {
		Display::device->setCursor(textX, textY);
		if (obj != nullptr) {
			if (!obj->isEnabled()) {
				Display::device->setTextColor(DISABLEDTEXTCOLOUR);
			}
			else {
				Display::device->setTextColor(TEXTCOLOUR);
			}
		} else {
			Display::device->setTextColor(TEXTCOLOUR);
		}
		Display::device->print(label);
	}

  public:

    Enableable *obj = nullptr;

    TouchButton(char* label="") :
    	TouchElement {
    		0,
    		0,
    		buildW(label),
    		buildH(label)
    	}
    {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.print(F("TouchButton( "));
			  Serial.print(F(label));
			  Serial.println(F(" )"));
		  }
		#endif
		this->label = label;
    }

    TouchButton(int centerX, int centerY, char* label="") :
    	TouchElement {
    		centerX,
    		centerY,
    		buildW(label),
    		buildH(label)
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
    		buildW(label),
    		buildH(label)
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
			buildW(label),
			buildH(label) //this one was different.. CHARHEIGHT*(MENUTEXTSIZE+1)
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
      label = newLabel;
      w = buildW(label);
      h = buildH(label);
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
		int textY = startY + (h - CHARHEIGHT*MENUTEXTSIZE)/2;
		int textX = startX + (w - (strlen(label) * CHARWIDTH * MENUTEXTSIZE))/2;
		printLabel(textX, textY);
    }

    /** UI press effect only for visual effect */
    void push() {
      #ifdef DEBUG
        if (Serial) {
    			Serial.print(F("TouchButton::push() "));
    			Serial.println(F(label));
        }
      #endif
      Display::device->fillRect(startX-shaddow, startY-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);
      Display::device->drawRoundRect(startX, startY, w, h, r, shaddowColour);
      int textY = startY + (h - CHARHEIGHT * MENUTEXTSIZE)/2;
      int textX = startX + (w - (strlen(label) * CHARWIDTH * MENUTEXTSIZE))/2;
      printLabel(textX, textY);

      /** delay ensures pushed visual state visible to
		* user, not worried about stopping everything since
		* this will be used in a menu screen */
      delay(500);
    }

};
#endif
