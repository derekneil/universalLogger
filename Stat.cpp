#ifndef STAT
#define STAT

#include "universalLogger.h"
#include "DisplayElement.h"
#include "Display.h"

Stat::Stat(int centerX, int centerY, int w, int h, char* label) :
	DisplayElement {centerX,centerY,w,h}
{
	#ifdef DEBUG
	  Serial.println(F("Stat(...) "));
	#endif
	this->label = label;
	lastValue = "";
}

Stat::~Stat() {
	delete label;
	delete lastValue;
}

int Stat::operator== (const Stat param) {
	if (
			centerX   == param.centerX   &&
			centerY   == param.centerY   &&
			w         == param.w         &&
			h         == param.h         &&
			colour    == param.colour    &&
			label     == param.label     &&
			lastValue == param.lastValue
	) {
		return true;
	}
	else {
		return false;
	}
}

/** erase entire space for stat
 * print label
 * print lastValue */
void Stat::draw() {
	#ifdef DEBUG
		Serial.print(F("Stat::draw() "));
		Serial.println(F(label));
	#endif

	int startX = centerX-w/2;
	int startY = centerY-h/2;
	int textY = startY + (h - CHARHEIGHT)/2;
	int textX = startX + (w - ((strlen(label)+strlen(lastValue)) * CHARWIDTH))/2;
	Display::device->fillRect(startX, startY, w, h, ERASECOLOUR);
	Display::device->setCursor(textX, textY);
	Display::device->print(label);
	Display::device->print(lastValue);
}

/** assumes label is already on screen
 * erases lastValue located after the label
 * prints new lastValue in it's place */
void Stat::redraw() {
	#ifdef DEBUG
	  Serial.print(F("Stat::redraw() "));
	  Serial.println(F(label));
	#endif

	int startX = centerX-w/2;
	int startY = centerY-h/2;
	int textY = startY + (h - CHARHEIGHT)/2;
	int textX = startX + (w - (strlen(lastValue) * CHARWIDTH))/2 + strlen(label)*CHARWIDTH;
	Display::device->fillRect(textX, startY, strlen(label)*CHARWIDTH, h, ERASECOLOUR);
	Display::device->setCursor(textX, textY);
	Display::device->print(lastValue);
}

void Stat::reset() {
	delete lastValue;
	lastValue = "";
}

#endif
