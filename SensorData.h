#ifndef SENSORDATA_H
#define SENSORDATA_H

#include "universalLogger.h"
#include "limits.h"

class SensorData {

  public:
    int *array; //XXX need to add more accessor methods that don't take O(n) access time to hide the data structure from other classes
    int size   = STDWIDTH;
    int index  = 0;
    int min    = INT_MIN;
    double avg = 0;
    int max    = INT_MAX;
    int bumped = 0;
    int count  = 0;
    double last10avg = 0;

    SensorData() {
		#ifdef DEBUG
			Serial.println(F("SensorData()"));
		#endif
		array = new int[size];
		resetStorageAndAvgAndCount();
    }
 
    SensorData(int size) {
		#ifdef DEBUG
			Serial.println(F("SensorData(...)"));
		#endif
		this->size = size;
		array = new int[size];
		resetStorageAndAvgAndCount();
	}
    virtual ~SensorData() {
		#ifdef DEBUG
			Serial.println(F("~SensorData()"));
		#endif
    	delete array;
    }

    virtual void insert(int val) {
		#ifdef DEBUG
			Serial.println(F("SensorData::insert(...)"));
		#endif
		bumped = array[index];
		array[index] = val;
		index++;

		//update avg
		if (count<=size) {
			avg = avg*count + val;
			count++; // we don't care about updating this after count==size
			avg /= count;
		}
		else if (count>size) {
			avg = (avg*size - bumped + val) / size;
		}

		//update last10avg
		if (count < 11) {
			last10avg = avg;
		}
		else {
			last10avg = (last10avg*10 - bumped + val) / 10;
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
			Serial.println(F("SensorData::latest()"));
		#endif
    	return array[index];
    }

    virtual int checkAndResetIndex() {
		#ifdef DEBUG
			Serial.println(F("SensorData::checkAndResetIndex()"));
		#endif

		if (index==size) {
			index = 0;
			return true;
		}
		return false;
    }

    virtual void resetAvg() {
		#ifdef DEBUG
			Serial.println(F("SensorData::resetAvg()"));
		#endif
    	last10avg = avg = count = 0;
    }

    /** as values are pushed out of the data storage, we need to update values */
    virtual void findMinMax() {
		#ifdef DEBUG
			Serial.println(F("SensorData::FindMinMax()"));
		#endif
		findMin();
		findMax();
    }

    virtual void findMin() {
		#ifdef DEBUG
			Serial.println(F("SensorData::FindMin()"));
		#endif
		if (bumped == min) {
			min = INT_MAX;
			for (int i=0; i<size; i++) {
				if (array[i] < min) {
					min = array[i];
				}
			}
		}
    }
    virtual void findMax() {
		#ifdef DEBUG
			Serial.println(F("SensorData::FindMax()"));
		#endif
		if (bumped == max) {
			max = INT_MIN;
			for (int i=0; i<size; i++) {
				if (array[i] > max) {
					max = array[i];
				}
			}
		}
    }

    virtual void resetMinMax() {
		#ifdef DEBUG
			Serial.println(F("SensorData::resetMinMax()"));
		#endif
		min = INT_MIN;
		max = INT_MAX;
    }

    virtual void resetStorageAndAvgAndCount(int val=0) {
		#ifdef DEBUG
			Serial.println(F("SensorData::resetStorage(...)"));
		#endif
  		for(int i=0; i<size; i++){
  			array[i] = val;
  		}
  		index = last10avg = avg = count = bumped = 0;
    }

    virtual void reset(int val=0) {
		#ifdef DEBUG
			Serial.println(F("SensorData::reset(...)"));
		#endif
		resetStorageAndAvgAndCount(val);
		bumped = 0;
		resetMinMax();
    }

};
#endif
