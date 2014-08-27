#ifndef SENSORDISPLAY
#define SENSORDISPLAY

#include "universalLogger.h"
#include "SensorDisplay.h"
#include "DisplayElement.h"
#include "Visualization.h"
#include "GraphScrolling.h"
//#include "GraphHighlighted.h"
//#include "Graph.h"
//#include "Dial.h"
#include "SensorData.h"
#include "Stats.h"

SensorDisplay::SensorDisplay() :
	viz { new GraphScrolling(0,0,STDWIDTH,STDHEIGHT-STATHEIGHT) } //,
//			data { new SensorData() }
{
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("SensorDisplay()"));
		}
	#endif
}

SensorDisplay::SensorDisplay(SensorData *dataSource) :
	DisplayElement          {0,0,STDWIDTH,STDHEIGHT},
	viz { new GraphScrolling(0,0,STDWIDTH,STDHEIGHT-STATHEIGHT) },
	stats                   {0,0,STDWIDTH,STATHEIGHT}
{
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("SensorDisplay(...)"));
		}
	#endif
	data = dataSource;
}

SensorDisplay::~SensorDisplay() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("~SensorDisplay()"));
		}
	#endif
	delete viz;
	//delete stats; //not required since it's not a pointer
	//external owner responsible for freeing SensorData *data memory
}

SensorDisplay& SensorDisplay::operator= (SensorDisplay *param) {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("SensorDisplay::operator="));
		}
	#endif
	// SensorDisplay newSensorDisplay;
	// newSensorDisplay.enabled = param->enabled;
	// newSensorDisplay.needsRedraw = param->needsRedraw;
	// newSensorDisplay.centerX = param->centerX;
	// newSensorDisplay.centerY = param->centerY;
	// newSensorDisplay.w = param->w;
	// newSensorDisplay.h = param->h;
	// newSensorDisplay.data = param->data;
	// newSensorDisplay.viz = param->viz;
	// newSensorDisplay.stats = param->stats;
	// return newSensorDisplay;
	return *param;
}

int SensorDisplay::operator== (const SensorDisplay *param) {
	#ifdef DEBUG
		if (Serial) {
			Serial.print(F("SensorDisplay::operator=="));
		}
	#endif
	if (
		enabled     == param->enabled     &&
		needsRedraw == param->needsRedraw &&
		centerX     == param->centerX     &&
		centerY     == param->centerY     &&
		w           == param->w           &&
		h           == param->h           &&
		data        == param->data        &&
		viz         == param->viz         &&
		stats       == param->stats
	) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F(" ... objects different"));
			}
		#endif
		return true;
	}
	else {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F(" ... objects same"));
			}
		#endif
		return false;
	}
}

/** deletes old viz it points to, assumes ownership of new viz being passed to it */
void SensorDisplay::setViz(Visualization *viz) {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("SensorDisplay::setViz(...)"));
		}
	#endif
	delete viz;
	this->viz = viz;
}

void SensorDisplay::draw() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("SensorDisplay::draw()"));
		}
	#endif
	viz->draw(data);
	stats.draw();
}

void SensorDisplay::redrawViz() {
	#ifdef DEBUG
		if (Serial) {
			Serial.print(F("SensorDisplay::redrawViz(...)"));
			Serial.print(F("   enabled: "));
			Serial.print(enabled);
			Serial.print(F("   needsRedraw: "));
			Serial.println(needsRedraw);
		}
	#endif
    if (enabled && needsRedraw) {
    	if (needsFullDraw == true) {
    		needsFullDraw = false;
    		viz->draw(data, divider);
    	}
    	else {
			viz->redraw(data);
    	}
		needsRedraw = false;
    }
}

void SensorDisplay::redrawStats() {
	#ifdef DEBUG
		if (Serial) {
			Serial.print(F("SensorDisplay::redrawStats(...)"));
			Serial.print(F("   enabled: "));
			Serial.print(enabled);
		}
	#endif
	if(enabled) {
		stats.redraw();
	}
}

void SensorDisplay::redraw() {
	#ifdef DEBUG
		if (Serial) {
			Serial.print(F("SensorDisplay::redraw(...)"));
			Serial.print(F("   enabled: "));
			Serial.print(enabled);
		}
	#endif
	redrawViz();
	redrawStats();
}

/** the calling code is responsible for calling redraw after calling this function */
// void locate(int x, int y) {
// 	#ifdef DEBUG
// 		if (Serial) {
			// Serial.println(F("SensorDisplay::locate(...)"));
// 		}
	// #endif
// 	DisplayElement::locate(x,y);
// 	viz->locate(x, y-STATHEIGHT/2);
// 	stats.locate(x, y /* can't do this without the height */ );

// }

/** the calling code is responsible for calling redraw after calling this function */
void SensorDisplay::locateCenterAndSize(int centerX, int centerY, int w, int h) {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("SensorDisplay::locateAndSize(...)"));
		}
	#endif
	DisplayElement::locateCenterAndSize(centerX,centerY,w,h);
	int vizCenterY = centerY-STATHEIGHT/2;
	viz->locateCenterAndSize(centerX, vizCenterY, w, h-STATHEIGHT);
	int statsCenterY = vizCenterY + (h-STATHEIGHT)/2 + STATHEIGHT/2;
	stats.locateCenterAndSize(centerX-1, statsCenterY, w, STATHEIGHT);
}

/** checks existing divider against a new value being added to data */
void SensorDisplay::checkDivider(int val) {
#ifdef DEBUG
	if (Serial) {
		Serial.print(F("SensorDisplay::checkDivider( "));
		Serial.print(val);
		Serial.print(F(" )  val/divider "));
		Serial.print(val);
		Serial.print(F(" / "));
		Serial.print(divider);
		Serial.print(F(" = "));
		Serial.print(val/divider);
		Serial.print(F(" < h: "));
		Serial.println(viz->getH());
	}
#endif
	int h = viz->getH();
	while (val/divider > h ) {
		divider++;
		needsFullDraw = true;
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("val/divider "));
				Serial.print(val);
				Serial.print(F(" / "));
				Serial.print(sd->divider);
				Serial.print(F(" = "));
				Serial.println(val/sd->divider);
			}
		#endif
	}
}

/** resets divider to 0 and checks against all existing data */
void SensorDisplay::checkDivider() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("SensorDisplay::checkDivider() "));
		}
	#endif
	int h = viz->getH();
	divider = 1;
	for (int i=0; i<data->size; i++) {
		while (data->array[i]/divider > h) {
			#ifdef DEBUG
				if (Serial) {
					Serial.print(F("data->array[i]/divider "));
					Serial.print(data->array[i]);
					Serial.print(F(" / "));
					Serial.print(divider);
					Serial.print(F(" = "));
					Serial.println(data->array[i]/divider);
				}
			#endif
			divider++;
			needsFullDraw = true;
		}
	}
}

void SensorDisplay::reset() {
	stats.reset();
	int needsRedraw = 0;
	int divider     = 1;
}

int SensorDisplay::isEnabled() {
	return enabled;
}

#endif
