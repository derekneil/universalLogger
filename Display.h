#ifndef DISPLAY_H
#define DISPLAY_H

#include "SensorDisplay.h"
#include "universalLogger.h"
#include <Adafruit_ILI9340.h>
#include <Adafruit_STMPE610.h>

class Display {
	protected:
		static int numSDs;
		static int numRegions;
		SensorDisplay *SDs[NUMREGIONS] = {nullptr} ;

	public:

		/** other classes need access to print stuff
		 * this saves having to make a whole new interface
		 * that just duplicates and calls the underlying device */
		static Adafruit_ILI9340 *device;
		static Adafruit_STMPE610 *touch;

		Display(Adafruit_ILI9340 *tft, Adafruit_STMPE610 *ts);

		~Display();

		static int hasSpace();

		int remove(SensorDisplay *sd);

		void updateDisplayLayout();

		int add(SensorDisplay *sd);

};
#endif
