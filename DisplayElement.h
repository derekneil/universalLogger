#ifndef DISPLAYELEMENT_H
#define DISPLAYELEMENT_H

#include "universalLogger.h"
#include <Adafruit_ILI9340.h>

class DisplayElement {

  public:
	int centerX=0; // center
	int centerY=0; // coordinates
	int w=0;
	int h=0;
    Adafruit_ILI9340 *tft;

	DisplayElement();

	DisplayElement(int centerX, int centerY, int w, int h);

	virtual ~DisplayElement();

	virtual void locate(int centerX, int centerY);

	virtual void locateAndSize(int centerX, int centerY, int w, int h);

	virtual void draw();

	virtual void redraw();

	/** assumes upper left 0,0 start point */
	virtual void clear(int backgroundColour=0);
};
#endif
