#ifndef MENU_H
#define MENU_H

//#include <vector> //weird compiler issue when this is added, something eclipse arduino ish
#include "universalLogger.h"
#include "DisplayElement.h"

#define mSize 7
class Menu {
  private:
	int mIndex = 0;
	DisplayElement *m[mSize] = { nullptr };
//	std::vector<DisplayElement*> m;
  public:
	Menu() {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.println(F("Menu()"));
		  }
		#endif
	}
	~Menu() {}
	void add(DisplayElement *el) {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.println(F("Menu::add(...)"));
		  }
		#endif
		if (mIndex< mSize) {
			m[mIndex++] = el;
		}
//		m.push_back(el);
	}
	void draw() {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.println(F("Menu::draw()"));
		  }
		#endif
		Display::device->fillScreen(MENUCOLOUR);
		Display::device->setTextSize(2);
		for (int i=0; i<mSize; i++){
			m[i]->draw();
		}
//	    for (auto &el : m) {
//	        el->draw();
//	    }
	}
};
#endif
