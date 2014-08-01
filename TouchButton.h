#ifndef TOUCHBUTTON_H
#define TOUCHBUTTON_H

#include "universalLogger.h"
#include "Touch.h"
#include "Display.h"

#define CHARWIDTH 6
#define CHARHEIGHT 8
#define BACKGROUNDCOLOUR ILI9340_BLUE
#define TEXTCOLOUR ILI9340_WHITE

#ifdef DEBUG
  #define ERASECOLOUR ILI9340_RED
#endif
#ifndef DEBUG
  #define ERASECOLOUR ILI9340_BLUE
#endif
#define BUTTONCOLOUR ILI9340_WHITE
#define BUTTONPUSHEDCOLOUR ILI9340_MAGENTA
#define RADIUS 4
#define SHADDOW ILI9340_BLACK

class TouchButton : public TouchElement{
  protected:
    char *label       = "";
    int r             = 4;
    int colour        = BUTTONCOLOUR;
    int shaddow       = 2;
    int shaddowColour = 0; //zero is usually black

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

    TouchButton(int centerX, int centerY, char* label="") :
    	TouchElement {
    		centerX,
    		centerY,
    		strlen(label)*CHARWIDTH*(Display::device->getTextSize())+CHARWIDTH*2,
    		CHARHEIGHT*(Display::device->getTextSize()+2)
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
    		strlen(label)*CHARWIDTH*(Display::device->getTextSize())+CHARWIDTH,
    		CHARHEIGHT*(Display::device->getTextSize()+1)
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
    	TouchElement {centerX,centerY,w,h}
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
      w = strlen(label)*CHARWIDTH*(Display::device->getTextSize())+CHARWIDTH;
      h = CHARHEIGHT*(Display::device->getTextSize()+1);
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
      int startX = centerX-w/2;
      int startY = centerY-h/2;
      Display::device->fillRect(startX-shaddow, startY-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);
      Display::device->drawFastHLine(startX+r -shaddow, startY -shaddow  , w-2*r, shaddowColour); // Top
      Display::device->drawFastVLine(startX -shaddow  , startY+r -shaddow, h-2*r, shaddowColour); // Left
      Display::device->drawCircleHelper(startX+r -shaddow, startY+r -shaddow, r, 1, shaddowColour); //top left corner
      Display::device->drawCircleHelper(startX+w-r-1 -shaddow, startY+r -shaddow, r, 2, shaddowColour); //top right corner
      Display::device->drawCircleHelper(startX+r -shaddow, startY+h-r-1 -shaddow, r, 8, shaddowColour); //bottom left corner
      Display::device->drawRoundRect(startX, startY, w, h, r, colour);
      int newY = startY + (h - CHARHEIGHT)/2;
      int newX = startX + (w - (strlen(label) * CHARWIDTH))/2;
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
      int startX = centerX-w/2;
      int startY = centerY-h/2;
      Display::device->fillRect(startX-shaddow, startY-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);
      Display::device->drawRoundRect(startX, startY, w, h, r, shaddowColour);
      int newY = startY + (h - CHARHEIGHT)/2;
      int newX = startX + (w - (strlen(label) * CHARWIDTH))/2;
      Display::device->setCursor(newX, newY);
      Display::device->print(label);
      delay(500);
    }

};
#endif
