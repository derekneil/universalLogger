#ifndef STATS_H
#define STATS_H

#include "DisplayElement.h"
#include "Stat.h"

class Stats : public DisplayElement {
  protected:

  public:
	Stat min;
	Stat max;
	Stat interval;
	Stat avg;
	Stat last10avg;
	Stat latest;

	Stats() : DisplayElement {} {
		#ifdef DEBUG
			Serial.println(F("Stats()"));
		#endif
	}

	/** this is setup for a 158x24 stats display area */
	Stats(int x=0, int y=0, int w=158, int h=24) :

		DisplayElement {x,y,w,h},

		min       {x+40,  y+4,  w/2, h/3},
		max       {x+40,  y+12, w/2, h/3},
		interval  {x+40,  y+20, w/2, h/3},
		avg       {x+119, y+4,  w/2, h/3},
		last10avg {x+119, y+12, w/2, h/3},
		latest    {x+119, y+20, w/2, h/3}
	{
		#ifdef DEBUG
			Serial.println(F("Stats(...)"));
		#endif
		min      .label = "min: ";
		max      .label = "max: ";
		interval .label = "interval: ";
		avg      .label = "avg: ";
		last10avg.label = "last10avg: ";
		latest   .label = "latest: ";
	}

	void locate(int x, int y){
		#ifdef DEBUG
			Serial.println(F("Stats::locate(...)"));
		#endif
		DisplayElement::locate(x,y);
		min       .locate(x+40,  y+4 );
		max       .locate(x+40,  y+12);
		interval  .locate(x+40,  y+20);
		avg       .locate(x+119, y+4 );
		last10avg .locate(x+119, y+12);
		latest    .locate(x+119, y+20);
	}

	void locateAndSize(int x, int y, int w, int h){
		#ifdef DEBUG
			Serial.println(F("Stats::locateAndSize(...)"));
		#endif
		DisplayElement::locateAndSize(x,y,w,h);
		min       .locateAndSize(x+40,  y+4,  w/2, h/3);
		max       .locateAndSize(x+40,  y+12, w/2, h/3);
		interval  .locateAndSize(x+40,  y+20, w/2, h/3);
		avg       .locateAndSize(x+119, y+4,  w/2, h/3);
		last10avg .locateAndSize(x+119, y+12, w/2, h/3);
		latest    .locateAndSize(x+119, y+20, w/2, h/3);
	}

	void draw() {
		#ifdef DEBUG
			Serial.println(F("Stats::draw()"));
		#endif
		min      .draw();
		max      .draw();
		interval .draw();
		avg      .draw();
		last10avg.draw();
		latest   .draw();
	}

	void redraw() {
		#ifdef DEBUG
			Serial.println(F("Stats::redraw()"));
		#endif
		min      .redraw();
		max      .redraw();
		interval .redraw();
		avg      .redraw();
		last10avg.redraw();
		latest   .redraw();
	}

};
#endif
