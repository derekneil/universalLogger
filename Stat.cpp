#ifndef STAT
#define STAT

#include "universalLogger.h"
#include "DisplayElement.h"
#include "Display.h"

Stat::Stat(int centerX, int centerY, int w, int h, char* label) :
	DisplayElement {centerX,centerY,w,h}
{
	#ifdef DEBUG
	  if (Serial) {
			Serial.println(F("Stat(...) "));
		}
	#endif
	strcpy(this->label, label);
	defaultValue();
}

Stat::~Stat() {
	#ifdef DEBUG
	  if (Serial) {
			Serial.println(F("~Stat() "));
		}
	#endif
}

int Stat::operator== (const Stat param) {
	if (	centerX   == param.centerX   &&
			centerY   == param.centerY   &&
			w         == param.w         &&
			h         == param.h         && //startX and startY not needed since they're computed values
			colour    == param.colour    &&
			strcmp(label, param.label)   &&
			strcmp(lastValue,param.lastValue)
	) {
		return true;
	}
	else {
		return false;
	}
}

/** make sure initial draw has room for 5 char stat */
void Stat::defaultValue() {
	sprintf(lastValue, "%4d", 0);
	sprintf(value, "%4d", 0);
}

/** place current value in last value,
 * then update value with new value */
void Stat::setValue(char* newValue) {
	#ifdef DEBUG
		if (Serial) {
			Serial.print(F("Stat::setValue() "));
		Serial.println(F(label));
		}
	#endif
	strcpy(lastValue,value);
	strcpy(value, newValue);
	needsRedraw = true;
}

/** erase entire space for stat
 * print label
 * print lastValue */
void Stat::draw() {
	#ifdef DEBUG
		if (Serial) {
			Serial.print(F("Stat::draw() "));
			Serial.println(F(label));
		}
	#endif

	int textY = startY + (h - CHARHEIGHT)/2;
	int textX = startX + (w - ((strlen(label)+strlen(value)) * CHARWIDTH))/2;
	Display::device->fillRect(startX, startY, w, h, ERASECOLOUR);
	Display::device->setCursor(textX, textY);
	Display::device->print(label);
	Display::device->print(value);
	needsRedraw = false;
}

/** assumes label is already on screen
 * erases lastValue located after the label
 * prints new lastValue in it's place */
void Stat::redraw() {
	#ifdef DEBUG
		if (Serial) {
			Serial.print(F("Stat::redraw() "));
	  		Serial.println(F(label));
		}
	#endif
	if (needsRedraw) {
		needsRedraw = false;
		int textY = startY + (h - CHARHEIGHT)/2;
		int textX = startX + (w - ((strlen(label)+strlen(lastValue)) * CHARWIDTH))/2 + strlen(label)*CHARWIDTH;
		Display::device->fillRect(textX, startY, strlen(lastValue)*CHARWIDTH, h, ERASECOLOUR); //only erase previous stat value
		Display::device->setCursor(textX, textY);
		Display::device->print(value); //only print new value
	}
}

void Stat::reset() {
	#ifdef DEBUG
		if (Serial) {
			Serial.print(F("Stat::reset() "));
			Serial.println(F(label));
		}
	#endif
	needsRedraw = false;
	defaultValue();
}

#endif
