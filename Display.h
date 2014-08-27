#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>
#include "SensorDisplay.h"
#include "universalLogger.h"
#include <Adafruit_ILI9340.h>
#include <Adafruit_STMPE610.h>

class Display {
	protected:
		int numSDs = 0;
		int numRegions = NUMREGIONS;
		std::vector<SensorDisplay*> SDs;

	public:

		/** other classes need access to print stuff
		 * this saves having to make a whole new interface
		 * that just duplicates and calls the underlying device */
		static Adafruit_ILI9340 *device;
		static Adafruit_STMPE610 *touch;

		Display(Adafruit_ILI9340 *tft, Adafruit_STMPE610 *ts);

		~Display();

		int isEmpty();

		int hasSpace();

		int remove(SensorDisplay *sd);

		void updateDisplayLayout();

		int add(SensorDisplay *sd);

};
#endif
