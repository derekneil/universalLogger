#include <Encoder.h>
#include "SensorInput.h"

// differential linear encoder supported by hardware interrupts
class LinearEncoder : public SensorInput {
	private: // -----------------------------------------------


	public: // -----------------------------------------------
		Encoder probe;
		int pinA;
		int pinB;
		int loadCellReadingState = COMPLETE;
		char loadCellReading[] = {' ','0','0','0','0','0'};
		int loadCellReadingInt = 0;
		int loadCellReadingIndex = 0;
		int value;
		SensorInput *sensorInput;
		void LinearEncoder(int pinA, int pinB){
			this->pinA = pinA;
			this->pinB = pinB;
		}
		void init() {
			probe = new Encoder(pinB, pinA);
			sensorInput = new SensorInput();
		}
		void pollProbe() {
			sensorInput->add(probe.read());

			//TODO put this.... somewhere..
			char probeString[6];
			sprintf(probeString,"%5d", valProbe);
			rePrint(158, TXTROW3, 5, probeString);
		}
		void reset() {
			calibrate();
			sensorInput->reset();
			loadCellReadingState = DIGITS;
			loadCellReadingInt = 0;
			loadCellReadingIndex = 0;
		}
		void calibrate() {
			probe.write(0);
		}

}: