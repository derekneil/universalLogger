#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "SensorData.h"

/** wrapper class for visualizations so DisplayElement
 * can hold another DisplayElement */
class Visualization : public DisplayElement {
  protected:
  	
  public:
	int doubleWidth = 0;
	Visualization() {
		#ifdef DEBUG
			Serial.println(F("Visualization()"));
		#endif
	}
	Visualization(int x=0, int y=0, int w=0, int h=0) :
		DisplayElement {x,y,w,h}
	{
		#ifdef DEBUG
			Serial.println(F("Visualization(...)"));
		#endif
	}

	virtual ~Visualization() {
		#ifdef DEBUG
			Serial.println(F("~Visualization()"));
		#endif
	} //shut eclipse up

	virtual void draw() {
		#ifdef DEBUG
			Serial.println(F("Visualization::draw() not implemented yet for this class"));
		#endif
	}

	virtual void redraw() {
		#ifdef DEBUG
			Serial.println(F("Visualization::redraw() not implemented yet for this class"));
		#endif
	}

};
#endif
