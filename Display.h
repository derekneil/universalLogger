#include "SensorDisplay.h"
#include "UniversalLogger.h"

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
		static int numSDs;
		static int lastNumSds;
		static SensorDisplay *sds;
		static int numRegions;

	public:
		Display(int size) : numRegions {size}
		{
			numSDs = 0;
			lastNumSds = -1;
			sds[size];
		}

		static int hasSpace() {
			return numSDs<numRegions;
		}

		int remove(SensorDisplay sd) {
			//loop through array of active SensorDisplays
			for (int i=0; i<numRegions; i++ ) {
				//match the one we're looking for
				if (sds[i] == sd sd.enabled == 1) {

					//disable it
					sd.enabled = 0;
					
					//set this memory address in the array as blank
					sds[i] = null;
					lastNumSds = numSDs;
					numSDs--;
					changeDisplayLayout();
					return true;
				}
			}
			return false;
		}

		void changeDisplayLayout() {

			if (lastNumSds<numSDs){
				//removed one, update remaining SDs with new W and H
				switch (numSDs) {
					case 0:
						sd.locateAndSize(CENTERX,CENTERY, FULLWIDTH, FULLHEIGHT);
						sd.locateAndSize()
						break;

					case 1:
						// split screen
						break;

					case 2:
						//
						break;

					case 3:
						// 
						break;
				}
			}
			else if (lastNumSds>numSDs) {
				//added one, update all SDs with new W and H
				switch (numSDs) {
					case 0:
						//this will be the only sensor displayed
						if (lastNumSds<numSDs){
							// it's actually the first one!
						}
						else {
								
						}
						sd.locateAndSize(CENTERX,CENTERY, FULLWIDTH, FULLHEIGHT);
						sd.locateAndSize()
						break;

					case 1:
						// split screen
						if (lastNumSds<numSDs){

						}
						else {

						}
						break;

					case 2:
						// 
						if (lastNumSds<numSDs){
							
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

		int add(SensorDisplay sd) {
			if (!hasSpace()) {
				return false;
			}
			for (int i=0; i<numRegions; i++ ) {
				// see if there's space
				if (sds[i] == NULL) {

					//disable it
					sd.enabled = 1;
					
					//set this memory address in the array
					sds[i] = sd;
					lastNumSds = numSDs;
					numSDs++;
					changeDisplayLayout();
					return true;
				}
			}

			return true;
		}

		void menu() {
			//here???
		}
};