#ifndef STATS
#define STATS

#include "Stats.h"
#include "universalLogger.h"
#include "DisplayElement.h"
#include "Stat.h"

Stats::Stats() : DisplayElement {} {
	#ifdef DEBUG
		Serial.println(F("Stats()"));
	#endif
}

/** this is setup for a 158x24 stats display area */
Stats::Stats(int centerX/*=0*/, int centerY/*=0*/, int w/*=STDWIDTH*/, int h/*=STATHEIGHT*/) :

	DisplayElement {centerX,centerY,w,h},

	min       {centerX+40,  centerY+4,  w/2, h/3},
	max       {centerX+40,  centerY+12, w/2, h/3},
	interval  {centerX+40,  centerY+20, w/2, h/3},
	avg       {centerX+119, centerY+4,  w/2, h/3},
	last10avg {centerX+119, centerY+12, w/2, h/3},
	latest    {centerX+119, centerY+20, w/2, h/3}
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

int Stats::operator== (const Stats param) {
	#ifdef DEBUG
		Serial.println(F("Stats::operator=="));
	#endif
	if (
			min       == param.min       &&
			max       == param.max       &&
			interval  == param.interval  &&
			avg       == param.avg       &&
			last10avg == param.last10avg &&
			latest    == param.latest
	) {
		return true;
	}
	else {
		return false;
	}
}

/** depricated, you should call locateAndSize */
void Stats::locate(int centerX, int centerY){
	#ifdef DEBUG
		Serial.println(F("Stats::locate(...)"));
	#endif
	DisplayElement::locate(centerX,centerY);
	min       .locate(centerX+40,  centerY+4 );
	max       .locate(centerX+40,  centerY+12);
	interval  .locate(centerX+40,  centerY+20);
	avg       .locate(centerX+119, centerY+4 );
	last10avg .locate(centerX+119, centerY+12);
	latest    .locate(centerX+119, centerY+20);
}

void Stats::locateAndSize(int centerX, int centerY, int w, int h){
	#ifdef DEBUG
		Serial.println(F("Stats::locateAndSize(...)"));
	#endif
	DisplayElement::locateAndSize(centerX,centerY,w,h);
	min       .locateAndSize(centerX+40,  centerY+4,  w/2, h/3);
	max       .locateAndSize(centerX+40,  centerY+12, w/2, h/3);
	interval  .locateAndSize(centerX+40,  centerY+20, w/2, h/3);
	avg       .locateAndSize(centerX+119, centerY+4,  w/2, h/3);
	last10avg .locateAndSize(centerX+119, centerY+12, w/2, h/3);
	latest    .locateAndSize(centerX+119, centerY+20, w/2, h/3);
}

void Stats::draw() {
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

void Stats::redraw() {
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

void Stats::reset() {
	min      .reset();
	max      .reset();
	interval .reset();
	avg      .reset();
	last10avg.reset();
	latest   .reset();
}

#endif
