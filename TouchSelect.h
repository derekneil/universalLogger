#ifndef TOUCHSELECT_H
#define TOUCHSELECT_H

#include "universalLogger.h"
#include "TouchElement.h"
#include "Display.h"
#include "LinkedList.h"
#include "TouchButton.h"

#define BUTTONCOLOUR WHITE
#define BUTTONPUSHEDCOLOUR MAGENTA

class TouchSelect : public TouchElement{
  protected:
    char label[32];

  public:

    LinkedList<TouchButton> btns;
    int *siElement = nullptr;

    TouchSelect(int startX, int startY, char* label="", int *siElement=nullptr, int n=0, ...) :
    	TouchElement {
    		startX,
    		startY
    	}
    {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.print(F("TouchSelect(...) "));
			  Serial.println(F(label));
		  }
		#endif
		strcpy(this->label, label);
		this->siElement = siElement;
		va_list args;
		va_start(args, n);
		int width = CHARWIDTH + strlen(label)*CHARWIDTH*MENUTEXTSIZE;
		for(;n;n--){
			char *s = va_arg(args, char*);
			TouchButton *tmp = new TouchButton(s);
			tmp->locateLeft(startX+width, startY);
			width += tmp->getW();
			btns.add( *tmp );
			/** adding to the linked list is a deep copy
			 * so changing after adding it to the list has no effect */
		}
		va_end(args);
		sizeAndSetCenter(
			(width)*CHARWIDTH*MENUTEXTSIZE + CHARWIDTH,
			CHARHEIGHT*(MENUTEXTSIZE+2)
		);
    }

    ~TouchSelect() {
//    	delete btns; //TODO make sure linked list destructor is being called
    }


private: //TODO should these just stay as private???
    void sizeAndSetCenter(int w, int h) {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchSelect::sizeAndSetCenter( "));
				Serial.print(w);
				Serial.print(F(", "));
				Serial.print(h);
				Serial.println(F(" )"));
			}
		#endif
    	DisplayElement::sizeAndSetCenter(w,h);

//		for(int i=0; i<btns.size(); i++){
//		  btns.get(i).sizeAndSetCenter(w,h);
//		}
    }
    void sizeAndSetStart(int w, int h) {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchSelect::sizeAndSetStart( "));
				Serial.print(w);
				Serial.print(F(", "));
				Serial.print(h);
				Serial.println(F(" )"));
			}
		#endif
    	DisplayElement::sizeAndSetStart(w,h);
//		for(int i=0; i<btns.size(); i++){
//		  btns.get(i).sizeAndSetStart(w,h);
//		}
    }


  public:
	char* getLabel() {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchSelect::getLabel() "));
				Serial.println(F(label));
			}
		#endif
		return label;
	}

    void draw() {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchSelect::draw() "));
				Serial.println(F(label));
			}
		#endif

		Display::device->fillRect(startX, startY, w, h, ERASECOLOUR);

		for(int i=0; i<btns.size(); i++){
		  btns.get(i).draw();
		}

		int newY = startY + (h - CHARHEIGHT*MENUTEXTSIZE)/2;
		int newX = startX + (CHARWIDTH * MENUTEXTSIZE);
		Display::device->setCursor(newX, newY);
		Display::device->print(label);
    }

	void push() {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("TouchSelect::push() "));
				Serial.println(F(label));
			}
		#endif

		for(int i=0; i<btns.size(); i++){
			TouchButton tmp = btns.get(i);
			tmp.draw();
			if (tmp.isPushed(lastTx, lastTy)) {
//				(tmp.*tmp.pushAction)();

			    /** generic version of pushAction based on one or two arguments??*/
				(*siElement) = i;
				tmp.push();
			}
		}

		/** delay ensures pushed visual state visible to
		* user, not worried about stopping everything since
		* this will be used in a menu screen */
		delay(500);
	}
};
#endif
