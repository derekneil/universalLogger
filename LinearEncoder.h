#ifndef LINEARENCODER_H
#define LINEARENCODER_H

#include "universalLogger.h"
#include <Encoder.h>
#include "SensorInput.h"

/** differential linear encoder supported by hardware interrupts */
class LinearEncoder : public SensorInput {
	protected:
		Encoder probe;

	public:
		
		LinearEncoder(int pinA, int pinB) :
			SensorInput {-1, -1},
			probe {pinA, pinB}
		{
			#ifdef DEBUG
				if (Serial) {
					Serial.println("LinearEncoder(...)");
				}
			#endif
			SensorInput::label = "LinearEncoder";
		}

		short poll() {
			#ifdef DEBUG
				if (Serial) {
					Serial.println("LinearEncoder::poll()");
				}
			#endif
			return probe.read();
		}
		void reset() {
			#ifdef DEBUG
				if (Serial) {
					Serial.println("LinearEncoder::reset()");
				}
			#endif
			calibrate();
			SensorInput::reset();
		}
		void calibrate() {
			#ifdef DEBUG
				if (Serial) {
					Serial.println("LinearEncoder::calibrate()");
				}
			#endif
			probe.write(0);
		}

};
#endif
