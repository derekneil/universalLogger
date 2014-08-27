#ifndef SENSORDATA_H
#define SENSORDATA_H

#include "universalLogger.h"
#include "limits.h"

class SensorData {

  public:
    short *array; //XXX need to add more accessor methods that don't take O(n) access time to hide the data structure from other classes
    int size     = STDWIDTH;
    int index    = 0;
    short min    = SHRT_MAX;
    double avg   = 0;
    short max    = SHRT_MIN;
    short bumped = 0;
    short count  = 0;

    short last10[10] = {0};
    int last10Index  = 0;
    int last10bumped = 0;
    double last10avg = 0;

    SensorData() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("SensorData()"));
			}
		#endif
		array = new short[size];
		resetStorageAndAvgAndCount();
    }
 
    SensorData(int size) {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("SensorData( "));
				Serial.print(size);
				Serial.println(F(" )"));
			}
		#endif
		this->size = size;
		array = new short[size];
		resetStorageAndAvgAndCount();
	}
    virtual ~SensorData() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("~SensorData()"));
			}
		#endif
    	delete array;
    }

    virtual void insert(short val) {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("SensorData::insert( "));
				Serial.print(val);
				Serial.println(F(" )"));
			}
		#endif
		bumped = array[index];
		array[index++] = val;

		last10bumped = last10[last10Index];
		last10[last10Index++] = val;
		if (last10Index > 9) {
			last10Index = 0;
		}

		//update avg
		if (count<size) {
			avg = avg*count + val;
			count++;
			avg /= count;
		}
		else { // if (count>=size) {
			avg = (avg*size - bumped + val) / size;
			count++;
		}

		//update last10avg
		if (count < 11) {
			last10avg = avg;
		}
		else {
			last10avg = (last10avg*10.0 - last10bumped + val) / 10.0;
		}

		//update max
		if (val>max) {
			max = val;
		}
		else if (count>=size) {
			findMax();
		}

		//update min
		if (val<min) {
			min = val;
		}
		else if (count>=size) {
			findMin();
		}
    }

    virtual int latest() {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("SensorData::latest() "));
				Serial.println(array[index]);
			}
		#endif
    	return array[index];
    }

    virtual int checkAndResetIndex() {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("SensorData::checkAndResetIndex() "));
			}
		#endif

		if (index==size) {
			index = 0;
			#ifdef DEBUG
				if (Serial) {
					Serial.println(F(" index==size -> index=0;"));
				}
			#endif
			return true;
		}
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F(" index != size"));
			}
		#endif
		return false;
    }

    virtual void resetAvg() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("SensorData::resetAvg()"));
			}
		#endif
    	last10avg = avg = count = 0;
    }

    /** as values are pushed out of the data storage, we need to update values */
    virtual void findMinMax() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("SensorData::FindMinMax()"));
			}
		#endif
		findMin();
		findMax();
    }

    virtual void findMin() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("SensorData::FindMin()"));
			}
		#endif
		if (bumped == min) {
			min = SHRT_MAX;
			for (int i=0; i<size; i++) {
				if (array[i] < min) {
					min = array[i];
				}
			}
		}
    }
    virtual void findMax() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("SensorData::FindMax()"));
			}
		#endif
		if (bumped == max) { //unless we just removed the max, it's still the max, no need to rescan
			max = SHRT_MIN;
			for (int i=0; i<size; i++) {
				if (array[i] > max) {
					max = array[i];
				}
			}
		}
    }

    virtual void resetMinMax() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println(F("SensorData::resetMinMax()"));
			}
		#endif
		min = SHRT_MAX;
		max = SHRT_MIN;
    }

    virtual void resetStorageAndAvgAndCount(short val=0) {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("SensorData::resetStorage( "));
				Serial.print(val);
				Serial.println(F(" )"));
			}
		#endif
  		for(int i=0; i<size; i++){
  			array[i] = val;
  		}
  		last10avg = avg = index = count = bumped = 0;
    }

    virtual void reset(int val=0) {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("SensorData::reset( "));
				Serial.print(val);
				Serial.println(F(" )"));
			}
		#endif
		resetStorageAndAvgAndCount(val);
		bumped = 0;
		resetMinMax();
    }

};
#endif
