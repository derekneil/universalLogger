#ifndef STATS_H
#define STATS_H

#include "universalLogger.h"
#include "DisplayElement.h"
#include "Stat.h"

#define STATHEIGHT 24

class Stats : public DisplayElement {

  public:
	Stat min;
	Stat max;
	Stat interval;
	Stat avg;
	Stat last10avg;
	Stat latest;

	Stats();

	/** this is setup for a 158x24 stats display area */
	Stats(int centerX, int centerY, int w, int h);

	int operator== (const Stats param);

	/** depricated, you should call locateAndSize */
	void locate(int centerX, int centerY);

	void locateAndSize(int centerX, int centerY, int w, int h);

	void draw();

	void redraw();

	void reset();

};
#endif
