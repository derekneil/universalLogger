#define LOADCELL Serial1
#define DIGITS    1
#define END       2
#define COMPLETE  3

class ForceMeter : public SensorInput {
	private:
		int loadCellReadingState = COMPLETE;
		char loadCellReading[] = {' ','0','0','0','0','0'};
		int loadCellReadingInt = 0;
		int loadCellReadingIndex = 0;

	public:

		void init(){
			LOADCELL.begin(19200, SERIAL_8N1_RXINV_TXINV);			
		}

		int pollForceMeter() {
			if (LOADCELL.available() > 0) {

				char incomingByte = LOADCELL.read();

				// -- debugging bytes from loadcell --
				//    char temp[16];
				//    sprintf(temp, "index %d, byte >%c<", loadCellReadingIndex, incomingByte);
				//    Serial.println(temp);

				// -- parseLoadCell incomingByte --
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

			return loadCellReadingInt = atoi(newLoadCellReading)

		}
		void reset() {
			sensorInput->reset();
			loadCellReadingState = DIGITS;
			loadCellReadingInt = 0;
			loadCellReadingIndex = 0;
		}
};