#ifndef DISPLAY_H
#define DISPLAY_H

#include "SensorDisplay.h"
#include "UniversalLogger.h"
#include <Adafruit_ILI9340.h>

#define SCREENWIDTH 320
#define SCREENHEIGHT 240

#define CENTERX 159
#define CENTERY 119

#define HALFHEIGHT 118
#define FULLHEIGHT 238 // = 118 * 2 + 2

#define HALFWIDTH STDWIDTH
#define FULLWIDTH 318 // = 158 * 2 + 2

class Display {
	protected:
		static int numSDs = 0;
		static int lastNumSDs = -1;
		static SensorDisplay *SDs;
		static int numRegions;

	public:
		/** other classes need access to print stuff
		 * this saves having to make a whole new interface
		 * that just duplicates and calls the underlying device */
		static Adafruit_ILI9340 *device;

		Display(Adafruit_ILI9340 *tft, int size) :
			device {tft},
			numRegions {size}
		{
			#ifdef DEBUG
				Serial.println(F("Display(...)"));
			#endif
			SDs = new SensorDisplay[size];
		}

		~Display() {
			#ifdef DEBUG
				Serial.println(F("~Display()"));
			#endif
			delete SDs;
		}

		static int hasSpace() {
			#ifdef DEBUG
				Serial.print(F("Display::hasSpace()"));
			#endif
			return numSDs<numRegions;
		}

		int remove(SensorDisplay *sd) {
			#ifdef DEBUG
				Serial.println(F("Display::remove(...)"));
			#endif
			//loop through array of active SensorDisplays
			for (int i=0; i<numRegions; i++ ) {
				//match the one we're looking for
				if (SDs[i] == sd sd.enabled == 1) {

					//disable it
					sd.enabled = 0;
					
					//set this memory address in the array as blank
					SDs[i] = NULL; //TODO figure out best way to have empty spots in the array of SensorDisplay's
					lastNumSDs = numSDs;
					numSDs--;
					changeDisplayLayout();
					return true;
				}
			}
			return false;
		}

		void changeDisplayLayout() {
			#ifdef DEBUG
				Serial.println(F("Display::changeDisplayLayout()"));
			#endif

			if (lastNumSDs<numSDs){
				// added a SensorDisplay, update all remaining SDs with new W and H
				// and centerX and centerY if required
				switch (numSDs) {
					case 0:
						#ifdef DEBUG
							Serial.println(F("unreachable case 0"));
						#endif
						//no SDs to update
						break;

					case 1:
						// single
						SDs[1].locateAndSize(CENTERX,CENTERY, FULLWIDTH, FULLHEIGHT);
						SDs[1].redraw();
						break;

					case 2:
						//
						break;

					case 3:
						// 
						break;
				}
			}
			else if (lastNumSDs>numSDs) {
				// removed a SensorDisplay, update all remaining SDs with new W and H
				// and centerX and centerY if required
				switch (numSDs) {
					case 0:
						//this will be the only sensor displayed

						break;

					case 1:
						SDs[].locateAndSize(CENTERX,CENTERY, FULLWIDTH, FULLHEIGHT);
						SDs[].locateAndSize()
						break;

					case 2:
						// 
						if (lastNumSDs<numSDs){
							
						}
						else {
							
						}
						break;

					case 3:
						// 
						break;
				}	
			}
		}

		int add(SensorDisplay *sd) {
			#ifdef DEBUG
				Serial.println(F("Display::add(...)"));
			#endif
			if (!hasSpace()) {
				return false;
			}
			for (int i=0; i<numRegions; i++ ) {
				// see if there's space
				if (SDs[i] == NULL) {

					//disable it
					sd->enabled = 1;
					
					//set this memory address in the array
					SDs[i] = sd;
					lastNumSDs = numSDs;
					numSDs++;
					changeDisplayLayout();
					return true;
				}
			}

			return true;
		}

		void menu() {
			#ifdef DEBUG
				Serial.println(F("Display::menu()"));
			#endif
			//here???
		}
};
#endif
