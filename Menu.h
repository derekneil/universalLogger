#ifndef MENU_H
#define MENU_H

#include "universalLogger.h"
#include "DisplayElement.h"

class Menu {
  private:
	std::vector<DisplayElement*> m;
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
		m.push_back(el);
	}
	void draw() {
		#ifdef DEBUG
		  if (Serial) {
			  Serial.println(F("Menu::draw()"));
		  }
		#endif
		Display::device->fillScreen(MENUCOLOUR);
		Display::device->setTextSize(2);
	    for (auto &el : m) {
	        el->draw();
	    }
	}
};
#endif
