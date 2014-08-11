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
    char *label       = "";

  public:

    LinkedList<TouchButton> btns;
    int *siElement = nullptr;

//    template <class T>
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
		this->label = label;
		this->siElement = siElement;
		va_list args;
		va_start(args, n);
		int btnsWidth = CHARWIDTH * MENUTEXTSIZE;
		for(;n;n--){
			char *s = va_arg(args, char*);
			btns.add( TouchButton(s) );
			btnsWidth += btns.getLast().getW(); // strlen(s); //FIXME buildW????
			#ifdef DEBUG
			  if (Serial) {
				  Serial.print(s);
				  Serial.print(" == ");
				  Serial.println(btns.getLast().getLabel());
				}
			#endif
		}
		va_end(args);
		sizeAndSetCenter( ((strlen(label)+btnsWidth+CHARWIDTH)*CHARWIDTH)*MENUTEXTSIZE,
				CHARHEIGHT*(MENUTEXTSIZE+2));
    }

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
		int newX = startX + (w - (strlen(label) * CHARWIDTH * MENUTEXTSIZE))/2;
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
