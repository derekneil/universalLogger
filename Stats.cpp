#ifndef STATS
#define STATS

#include "Stats.h"
#include "universalLogger.h"
#include "DisplayElement.h"
#include "Stat.h"

Stats::Stats() : DisplayElement {} {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Stats()"));
		}
	#endif
}

/** recommended minimum 158(w) x 30(h) stats display area */
Stats::Stats(int centerX/*=0*/, int centerY/*=0*/, int w/*=STDWIDTH*/, int h/*=STATHEIGHT*/) :

	DisplayElement {centerX,centerY,w,h},

	min       {centerX-w/4, centerY-h/3, w/2, h/3, "min: "},
	max       {centerX-w/4, centerY,     w/2, h/3, "max: "},
	interval  {centerX-w/4, centerY+h/3, w/2, h/3, "int: "},
	avg       {centerX+w/4, centerY-h/3, w/2, h/3, "avg: "},
	last10avg {centerX+w/4, centerY,     w/2, h/3, "10avg: "},
	latest    {centerX+w/4, centerY+h/3, w/2, h/3, "last: "}
{
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Stats(...)"));
		}
	#endif
	// strcpy(min      .label, 
	// strcpy(max      .label, 
	// strcpy(interval .label, 
	// strcpy(avg      .label, 
	// strcpy(last10avg.label, 
	// strcpy(latest   .label, 
}

Stats::~Stats() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("~Stats()"));
		}
	#endif
}

int Stats::operator== (const Stats param) {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Stats::operator=="));
		}
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
void Stats::locateCenter(int centerX, int centerY){
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Stats::locate(...)"));
		}
	#endif
	DisplayElement::locateCenter(centerX,centerY);
	min       .locateCenter(centerX-w/4, centerY-h/3 );
	max       .locateCenter(centerX-w/4, centerY);
	interval  .locateCenter(centerX-w/4, centerY+h/3);
	avg       .locateCenter(centerX+w/4, centerY-h/3 );
	last10avg .locateCenter(centerX+w/4, centerY);
	latest    .locateCenter(centerX+w/4, centerY+h/3);
}

void Stats::locateCenterAndSize(int centerX, int centerY, int w, int h){
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Stats::locateAndSize(...)"));
		}
	#endif
	DisplayElement::locateCenterAndSize(centerX,centerY,w,h);
	min       .locateCenterAndSize(centerX-w/4, centerY-h/3, w/2, h/3);
	max       .locateCenterAndSize(centerX-w/4, centerY,     w/2, h/3);
	interval  .locateCenterAndSize(centerX-w/4, centerY+h/3, w/2, h/3);
	avg       .locateCenterAndSize(centerX+w/4, centerY-h/3, w/2, h/3);
	last10avg .locateCenterAndSize(centerX+w/4, centerY,     w/2, h/3);
	latest    .locateCenterAndSize(centerX+w/4, centerY+h/3, w/2, h/3);
}

void Stats::draw() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Stats::draw()"));
		}
	#endif
	min      .draw();
	max      .draw();
	interval .draw();
	avg      .draw();
	last10avg.draw();
	latest   .draw();
}

/** this should only be used for redrawing after screen changes
 * individual updates are handled by update after polling a sensor */
void Stats::redraw() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Stats::redraw()"));
		}
	#endif
	min      .redraw();
	max      .redraw();
	interval .redraw();
	avg      .redraw();
	last10avg.redraw();
	latest   .redraw();
}

void Stats::reset() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("Stats::reset()"));
		}
	#endif
	min      .reset();
	max      .reset();
//	interval .reset(); //excluded because
	avg      .reset();
	last10avg.reset();
	latest   .reset();
}

#endif
