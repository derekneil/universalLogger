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

	DisplayElement(int startX, int startY);

	DisplayElement(int centerX, int centerY, int w, int h);

	virtual ~DisplayElement();

	virtual void locateCenter(int centerX, int centerY);

	virtual void locateCenterAndSize(int centerX, int centerY, int w, int h);

	virtual void locateLeft(int startX, int startY);

	virtual void locateLeftAndSize(int startX, int startY, int w, int h);

	virtual void sizeAndSetCenter(int w, int h);

	virtual void sizeAndSetStart(int w, int h);

	virtual void draw();

	virtual void redraw();

	virtual void clear(int colour=BACKGROUNDCOLOUR);

	int getStartX() const { return startX; }
	int getStartY() const { return startY; }
	int getCenterX() const { return centerX; }
	int getCenterY() const { return centerY; }
	int getW() const { return w; }
	int getH() const { return h; }
	
};
#endif
