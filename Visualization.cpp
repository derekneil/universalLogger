#ifndef VISUALIZATION
#define VISUALIZATION

#include "Visualization.h"
#include "DisplayElement.h"

Visualization::Visualization() {
	#ifdef DEBUG
		Serial.println(F("Visualization()"));
	#endif
}
Visualization::Visualization(int centerX, int centerY, int w, int h) :
	DisplayElement {centerX,centerY,w,h}
{
	#ifdef DEBUG
		Serial.println(F("Visualization(...)"));
	#endif
}

Visualization::~Visualization() {
	#ifdef DEBUG
		Serial.println(F("~Visualization()"));
	#endif
} //shut eclipse up

void Visualization::draw() {
	#ifdef DEBUG
		Serial.println(F("Visualization::draw() not implemented yet for this class"));
	#endif
}

void Visualization::redraw() {
	#ifdef DEBUG
		Serial.println(F("Visualization::redraw() not implemented yet for this class"));
	#endif
}

#endif
