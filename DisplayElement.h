#ifndef DISPLAYELEMENT_H
#define DISPLAYELEMENT_H

#include "universalLogger.h"

class DisplayElement {
  protected:
	int startX=0;
	int startY=0;
	int centerX=0;
	int centerY=0;
	int w=0;
	int h=0;

  public:

	DisplayElement();

	DisplayElement(int centerX, int centerY, int w, int h);

	virtual ~DisplayElement();

	virtual void locate(int centerX, int centerY);

	virtual void locateAndSize(int centerX, int centerY, int w, int h);

	virtual void draw();

	virtual void redraw();

	virtual void clear(int colour=BACKGROUNDCOLOUR);

	int getCenterX() const { return centerX; }
	int getCenterY() const { return centerY; }
	int getH() const { return h; }
	int getStartX() const { return startX; }
	int getStartY() const { return startY; }
	int getW() const { return w; }
	
};
#endif
