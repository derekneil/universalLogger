#ifndef SENSORDISPLAY
#define SENSORDISPLAY

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
		Serial.println(F("SensorDisplay()"));
	#endif
}

SensorDisplay::SensorDisplay(SensorData *dataSource) :
	//not really worth changing all these constructors, they might be used a different way later
	DisplayElement          {0,0,STDWIDTH,STDHEIGHT},
	viz { new GraphScrolling(0,0,STDWIDTH,STDHEIGHT-STATHEIGHT) },
	stats                   {0,0,STDWIDTH,STATHEIGHT}
{
	#ifdef DEBUG
		Serial.println(F("SensorDisplay(...)"));
	#endif
	data = dataSource;
}

SensorDisplay::~SensorDisplay() {
	#ifdef DEBUG
		Serial.println(F("~SensorDisplay()"));
	#endif
	delete viz;
	//delete stats; //not required since it's not a pointer
	//external owner responsible for freeing SensorData *data memory
}

SensorDisplay& SensorDisplay::operator= (SensorDisplay *param) {
	#ifdef DEBUG
		Serial.println(F("SensorDisplay::operator="));
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
		Serial.println(F("SensorDisplay::operator=="));
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
		return true;
	}
	else {
		return false;
	}
}

void SensorDisplay::setTFT(Adafruit_ILI9340 *tft) {
	viz->tft = tft;
	stats.setTFT(tft);
}

/** deletes old viz it points to, assumes ownership of new viz being passed to it */
void SensorDisplay::setViz(Visualization *viz) {
	#ifdef DEBUG
		Serial.println(F("SensorDisplay::setViz(...)"));
	#endif
	delete viz;
	this->viz = viz;
}

void SensorDisplay::draw() {
	#ifdef DEBUG
		Serial.println(F("SensorDisplay::draw()"));
	#endif
	viz->draw(data);
	stats.draw();
}

void SensorDisplay::redraw() {
	#ifdef DEBUG
		Serial.println(F("SensorDisplay::redraw(...)"));
	#endif
    if (enabled && needsRedraw) {
		viz->redraw(data);
		needsRedraw = false;
    }
	//individual stats.stat 's are redrawn by SensorInput::updateDataAndRedrawStat(...) as their values change
}

/** the calling code is responsible for calling redraw after calling this function */
// void locate(int x, int y) {
// 	#ifdef DEBUG
// 		Serial.println(F("SensorDisplay::locate(...)"));
// 	#endif
// 	DisplayElement::locate(x,y);
// 	viz->locate(x, y-STATHEIGHT/2);
// 	stats.locate(x, y /* can't do this without the height */ );

// }

/** the calling code is responsible for calling redraw after calling this function */
void SensorDisplay::locateAndSize(int centerX, int centerY, int w, int h) {
	#ifdef DEBUG
		Serial.println(F("SensorDisplay::locateAndSize(...)"));
	#endif
	DisplayElement::locateAndSize(centerX,centerY,w,h);
	viz->locateAndSize(centerX, centerY-STATHEIGHT/2, w, h-STATHEIGHT);
	int statsY = centerY + (h-STATHEIGHT)/2 + STATHEIGHT/2;
	stats.locateAndSize(centerX, statsY, w, STATHEIGHT);
}

void SensorDisplay::reset() {
	stats.reset();
}

#endif
