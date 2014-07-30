#ifndef DISPLAY_H
#define DISPLAY_H

#include "SensorDisplay.h"
#include "universalLogger.h"
#include <Adafruit_ILI9340.h>
#include <Adafruit_STMPE610.h>

#define SCREENWIDTH 320
#define SCREENHEIGHT 240

#define CENTER_X SCREENWIDTH/2-1
#define CENTER_X1 80
#define CENTER_X2 239

#define CENTER_Y SCREENHEIGHT/2-1
#define CENTER_Y1 59
#define CENTER_Y2 177

#define HALFHEIGHT STDHEIGHT // = CENTER_Y-1
#define FULLHEIGHT 238 // = 118 * 2 + 2

#define HALFWIDTH STDWIDTH // = CENTER_X-1
#define FULLWIDTH 318 // = 158 * 2 + 2

class Display {
	protected:
		static int numSDs;
		static int lastNumSDs;
		static SensorDisplay *SDs;
		static int numRegions;

	public:
		/** other classes need access to print stuff
		 * this saves having to make a whole new interface
		 * that just duplicates and calls the underlying device */
		static Adafruit_ILI9340 *device;

		static Adafruit_STMPE610 *touch;

//		Display();

		Display(Adafruit_ILI9340 *tft, Adafruit_STMPE610 *ts, int regions);

		~Display();

		static int hasSpace();

		static int remove(SensorDisplay *sd);

		static void updateDisplayLayout();

		static int add(SensorDisplay *sd);

};
#endif
