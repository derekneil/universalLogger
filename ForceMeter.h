#ifndef FORCEMETER_H
#define FORCEMETER_H

#include "universalLogger.h"
#include "SensorInput.h"

#ifndef LOADCELL
	#define LOADCELL Serial1
#endif

#define DIGITS    1
#define END       2
#define COMPLETE  3

/** does not  implement calibrate in software!!! */
class ForceMeter : public SensorInput {
	protected:
		int loadCellReadingState = COMPLETE;
		char loadCellReading[6]  = {' ','0','0','0','0','0'};
		int loadCellReadingInt   = 0;
		int loadCellReadingIndex = 0;

	public:

		ForceMeter(int pin=0, int type=-1) :
			SensorInput{pin, type}
		{
			#ifdef DEBUG
				if (Serial) {
					Serial.print(F("ForceMeter()"));
				}
			#endif
			LOADCELL.begin(19200, SERIAL_8N1_RXINV_TXINV);	
		}

		int poll() {
			#ifdef DEBUG
				if (Serial) {
					Serial.print(F("ForceMeter::poll()"));
				}
			#endif
			if (LOADCELL.available() > 0) {

				char incomingByte = LOADCELL.read();

				// -- parseLoadCell incomingByte --s
				switch (incomingByte) {
				  case 32: // leading space ' ' represents positive value
				  case 45: // leading minus sign '-'
				    loadCellReadingIndex=0;
				    loadCellReading[loadCellReadingIndex] = incomingByte;
				    loadCellReadingIndex++;
				    loadCellReadingState = DIGITS;
				    break;
				  case 46: // ending '.'
				    if (loadCellReadingState == END && loadCellReadingIndex == 6) {
				      loadCellReadingState = COMPLETE;
				    }
				    break;
				  case 13:
				    if (loadCellReadingState == COMPLETE) {
				      loadCellReadingState = DIGITS; // must change or you'll output twice
				    }
				    break;
				  case 48: // == zero '0'
				  case 49:
				  case 50:
				  case 51:
				  case 52:
				  case 53:
				  case 54:
				  case 55:
				  case 56:
				  case 57: // == nine '9'
				    if (loadCellReadingState == DIGITS && loadCellReadingIndex < 6) {
				      loadCellReading[loadCellReadingIndex] = incomingByte;
				      loadCellReadingIndex++;
				      if(loadCellReadingIndex == 6) {
				        loadCellReadingState = END;
				      }
				    }
				    break;
				}

			}

			//if there's a new value, update the value to be returned
			if (loadCellReadingState == COMPLETE) {
				loadCellReadingInt = atoi(loadCellReading);
			}

			return loadCellReadingInt;
		}

		void reset() {
			#ifdef DEBUG
				if (Serial) {
					Serial.print(F("ForceMeter::reset()"));
				}
			#endif
			loadCellReadingState = DIGITS;
			loadCellReadingInt = 0;
			loadCellReadingIndex = 0;
			SensorInput::reset();
		}
};
#endif
