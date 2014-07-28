#ifndef SENSORDATA_H
#define SENSORDATA_H

#include "universalLogger.h"
#include "limits.h"

class SensorData {
  protected:

  public:
    int *array;
    int size   = STDWIDTH;
    int index  = 0 ;
    int min    = INT_MIN;
    double avg = 0;
    int max    = INT_MAX;
    int bumped = 0;
    int count  = 0;

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
    	avg = count = 0;
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
  		index = avg = count = 0; //TODO do we need to set bumped=0 here??
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
