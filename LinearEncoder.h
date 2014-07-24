#include <Encoder.h>
#include "SensorInput.h"

// differential linear encoder supported by hardware interrupts
class LinearEncoder : public SensorInput {
	protected:
		Encoder probe;

	public:
		int value;
		
		LinearEncoder(int pinA, int pinB) :
		probe {pinA, pinB}, 
		sensorInput {-1, -1}
		{

		}

		int poll() {
			return probe.read();

			//TODO put this.... somewhere..
			char probeString[6];
			sprintf(probeString,"%5d", valProbe);
			rePrint(158, TXTROW3, 5, probeString);
		}
		void reset() {
			calibrate();
			SensorInput::reset();
		}
		void calibrate() {
			probe.write(0);
		}

}: