#ifndef SENSORDATA_H
#define SENSORDATA_H

#include "universalLogger.h"
#include "limits.h"

class SensorData {
  protected:

  public:
    int *array;
    int size;
    int index;
    int min = INT_MIN;
    double avg = 0;
    int max = INT_MAX;
    int bumped = 0;
    int count = 0;
 
    SensorData(int size=STDWIDTH) {
		#ifdef DEBUG
			Serial.print(F("SensorData(...)"));
		#endif
		this->size = size;
		array = new int[size];
		resetStorage();
	}
    virtual ~SensorData() {
    	delete array;
    }

    virtual void insert(int val) {
		#ifdef DEBUG
			Serial.print(F("SensorData::insert(...)"));
		#endif
		bumped = array[index];
		array[index] = val;
		index++;
		avg = avg*count+val;
		count++;
		avg /= count;
		if (val>max) {
			max = val;
		}
		if (val<min) {
			min = val;
		}
    }

    virtual int latest() {
		#ifdef DEBUG
			Serial.print(F("SensorData::latest()"));
		#endif
    	return array[index];
    }

    virtual int checkAndResetIndex() {
		#ifdef DEBUG
			Serial.print(F("SensorData::checkAndResetIndex()"));
		#endif

		if (index==size) {
			index = 0;
			return true;
		}
		return false;
    }

    virtual void resetAvg() {
		#ifdef DEBUG
			Serial.print(F("SensorData::resetAvg()"));
		#endif
    	avg = count = 0;
    }

    virtual void resetMinMax() {
		#ifdef DEBUG
			Serial.print(F("SensorData::resetMinMax()"));
		#endif
		min = INT_MIN;
		max = INT_MAX;
    }

    virtual void resetStorage(int val=0) {
		#ifdef DEBUG
			Serial.print(F("SensorData::resetStorage(...)"));
		#endif
  		for(int i=0; i<size; i++){
  			array[i] = val;
  		}
  		index = avg = count = 0;
    }

    virtual void reset(int val=0) {
		#ifdef DEBUG
			Serial.print(F("SensorData::reset(...)"));
		#endif
		resetStorage(val);
		bumped = 0;
		resetMinMax();
    }

};
#endif
