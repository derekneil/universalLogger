#ifndef TOUCHADJUST_H
#define TOUCHADJUST_H

#include "universalLogger.h"
#include "TouchElement.h"
#include "Display.h"
#include "TouchArrow.h"

#define BUTTONCOLOUR WHITE
#define BUTTONPUSHEDCOLOUR MAGENTA

class TouchAdjust : public TouchElement{
  protected:
    char label[32];

  public:
    char valStr[16];
    int *siElement = nullptr;

    TouchArrow up = TouchArrow(0);
    TouchArrow down = TouchArrow(2);
    SensorInput *obj = nullptr;

    template <class T>
    TouchAdjust(int startX, int startY, char* label="", T *obj=nullptr, int *siElement=nullptr) :
    	TouchElement {
    		startX,
    		startY
    	}
    {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.print(F("TouchAdjust(...) "));
			  Serial.println(F(label));
		  }
		#endif
		strcpy(this->label, label);
		this->siElement = siElement;
		this->obj = obj;
		sprintf(valStr, "%d sec", *siElement);
		int spacer = CHARHEIGHT*(MENUTEXTSIZE+2);
		int width = strlen(label)*CHARWIDTH*(MENUTEXTSIZE);
		width += strlen(valStr)*CHARWIDTH*(MENUTEXTSIZE) + spacer;
		up.locateLeft(startX+width, startY);
		width += up.getW() + spacer;
		down.locateLeft(startX+width, startY);
		width += down.getW();
		sizeAndSetCenter(
			(width)*CHARWIDTH*MENUTEXTSIZE + CHARWIDTH,
			spacer
		);

		sizeAndSetCenter(
			width,
			CHARHEIGHT*(MENUTEXTSIZE+2)
		);
    }


private: //TODO should these just stay as private???
    void sizeAndSetCenter(int w, int h) {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchAdjust::sizeAndSetCenter( "));
				Serial.print(w);
				Serial.print(F(", "));
				Serial.print(h);
				Serial.println(F(" )"));
			}
		#endif
    	DisplayElement::sizeAndSetCenter(w,h);
    }


  public:
    void draw() {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchAdjust::draw() "));
				Serial.println(F(label));
			}
		#endif

		Display::device->fillRect(startX, startY, w, h, ERASECOLOUR);
		up.draw();
		down.draw();
		int newY = startY + (h - CHARHEIGHT*MENUTEXTSIZE)/2;
		int newX = startX + (CHARWIDTH * MENUTEXTSIZE);
		Display::device->setCursor(newX, newY);
		Display::device->print(label);
		Display::device->print(valStr);

    }

	void push() {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchAdjust::push() "));
				Serial.println(F(label));
			}
		#endif

		if (up.isPushed(lastTx, lastTy)) {
			(*siElement)++;
			up.push();
		}
		else if (down.isPushed(lastTx, lastTy)) {
			(*siElement)--;
			down.push();
		}
	}
};
#endif
