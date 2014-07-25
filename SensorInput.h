#ifndef SENSORINPUT_H
#define SENSORINPUT_H

#include "universalLogger.h"

#include "SensorData.h"
#include "SensorDisplay.h"
#include "limits.h"

//SensorInput.h
#define UNLOCK 0
#define LOCK 1

#define STATIC 0
#define DYNAMIC 1

#define AVGDETECTION 0
#define PEAKDETECTION 1
#define MINDETECTION 2

#define RAWDATASIZE STDWIDTH*4

// TODO add defs to break out .h

class SensorInput {
  protected:
	int type; /** ANALOG | DIGITAL */
	int pin;
	int mode = STATIC; /** STATIC | DYNAMIC */
	int low = 10;
	int high = 50;
	int dynamicLock = LOCK;
	int interval = 1000000; /** default to 1 second */
	int lastIntervalTime = 0;
	int cycles = 0;
	int filter = MINDETECTION; /** MIN | AVG | MAX */
	SensorData rawData;
	SensorData shortTermData;
	SensorData longTermData;
	SensorDisplay shortTermDisplay;
	SensorDisplay longTermDisplay;

    virtual void redrawStat(Stat *stat, int newVal) {
        {
            char tempString[5];
            sprintf(tempString, "%4d", newVal);
            stat->lastValue = tempString;
            stat->redraw();
        }
    }

    virtual void redrawStats(SensorData *data, SensorDisplay *display, int newVal) {
		#ifdef DEBUG
			Serial.println("SensorInput::redrawStats(...)");
		#endif
        redrawStat(&(display->stats.latest), newVal);
        redrawStat(&(display->stats.min), data->min);
        redrawStat(&(display->stats.avg), data->avg);
        redrawStat(&(display->stats.max), data->max);
    }

  public:
	int enabled = 1;

    SensorInput(int pin, int type) : 
    rawData          {RAWDATASIZE},
    shortTermData    {STDWIDTH},
    longTermData     {STDWIDTH},
    shortTermDisplay {STDWIDTH, &shortTermData},
    longTermDisplay  {STDWIDTH, &longTermData}
    {
		#ifdef DEBUG
			Serial.println("SensorInput(...)");
		#endif
        this->pin = pin;
        this->type = type;
        if (pin > 0) {
            if (type==DIGITAL) {
                pinMode(pin, INPUT_PULLUP);
            }
            else if (type==ANALOG) {
                pinMode(pin, INPUT);
            }
        }
    }

    virtual ~SensorInput() {
		#ifdef DEBUG
			Serial.println("~SensorInput()");
		#endif
    }

    virtual void toggleMode() {
		#ifdef DEBUG
			Serial.println("SensorInput::toggleMode(...)");
		#endif
        if (mode==DYNAMIC) {
            mode = STATIC;
        }
        else {
            mode = DYNAMIC;
        }
    }

    /** Poll sensor input for latest value
    use type parameter to determine read function used
    */
    virtual int poll() {
		#ifdef DEBUG
			Serial.println("SensorInput::poll()");
		#endif
        int newReading;
        if (type==ANALOG) {
            newReading = analogRead(pin-14); // 0-1024
        }
        else if (type==DIGITAL) {
            newReading = pinMode(pin, INPUT_PULLUP); // 0-1
        }
        return newReading;
    }

    /** Apply logic for exclusive dynamic, and static modes.
    Intervals can be defined as completing:
    - one dynamic cycle through the high, then back down to low,
    dynamic mode will cycle through the data storage until this
    happens
    - time interval lapsed, can also loop through data storage multiple times
    waiting for time interval to pass
    - NOTE: the results of an interval in either mode will be limited
    to the size of the data storage for the min, avg, max observed
    during that cycle
    */
    virtual int intervalLapsed(int newReading) {
		#ifdef DEBUG
			Serial.println("SensorInput::intervalLapsed(...)");
		#endif
        int lapsed = 0;

        if( mode == DYNAMIC ) {
            if (dynamicLock==LOCK && newReading > high) {
                dynamicLock = UNLOCK;
                #ifdef DEBUG
                    Serial.println("interval - dynamicLock UNLOCKED");
                #endif
            }
            else if (dynamicLock==UNLOCK && newReading < low) {
                dynamicLock = LOCK;
                #ifdef DEBUG
                    Serial.println("interval lapsed - dynamicLock LOCKed");
                #endif
                lapsed = 1;
            }
        }
        else { // mode == STATIC
            int timeCheck = micros() - lastIntervalTime;
            if (timeCheck > interval){
                #ifdef DEBUG
                    Serial.println("interval lapsed - time up");
                #endif
                lapsed = 1;
            }
        }

        //make sure storage isn't overflowing
        if (rawData.checkAndResetIndex()) {
            #ifdef DEBUG
                Serial.println("interval still going - wrapping around data storage");
            #endif
        }
        return lapsed;
    }

    /** Update saved data and stats for SensorInput
    Operates in three user selectable modes, the Min|Avg|Max of
    the raw data is saved to short term data.
    Once short term data cycles through the storage, the Avg is
    saved to the long term data, and the oldest data point is replaced
    with the newest one.
    Once long term data cycles through the storage, it replaces the
    oldest value with the new one.
    */
    virtual void updateDataAndRedrawStats(int newReading) {
		#ifdef DEBUG
			Serial.println("SensorInput::updateDataAndStats(...)");
		#endif
        rawData.insert(newReading);

        //see if interval is up or raw data is full
        if (intervalLapsed(newReading)) {
            cycles++;
            int currentFiltered = INT_MIN;

            if (filter==MINDETECTION) {
                currentFiltered = rawData.min;
            }
            else if (filter==AVGDETECTION) {
                currentFiltered = rawData.avg;
            }
            else if (filter==PEAKDETECTION) {
                currentFiltered = rawData.max;
            }
            rawData.reset();

            //see if we have new value
            if (currentFiltered!=INT_MAX && currentFiltered!=INT_MIN) {
                shortTermData.insert(currentFiltered);
                if (shortTermDisplay.enabled) {
                	redrawStats(&shortTermData, &shortTermDisplay, currentFiltered);
                }

                // see if we're done a cycle through the short term
                if (shortTermData.checkAndResetIndex()) {

                	//save last shortTermData cycle avgerage to long term
                    longTermData.insert(shortTermData.avg);
                    if (longTermDisplay.enabled) {
                    	redrawStats(&longTermData, &longTermDisplay, shortTermData.avg);
                    }
                    shortTermData.resetAvg();
                    shortTermData.resetMinMax();

                    //see if it's time to calculate new long term peak avg
                    if (longTermData.checkAndResetIndex()) {
                    	longTermData.resetAvg(); //TODO do we really want the avg to be of the displayed value
                    							 //or of every value the longTermData has seen?
                    }

                }

            }
        }
    }

    virtual void updateViz() {
		#ifdef DEBUG
			Serial.println("SensorInput::updateViz()");
		#endif
        if (shortTermDisplay.enabled) {
            shortTermDisplay.viz->redraw();
        }
        if (longTermDisplay.enabled) {
            longTermDisplay.viz->redraw();
        }
    }

    virtual void reset() {
		#ifdef DEBUG
			Serial.println("SensorInput::reset()");
		#endif
        if (mode==DYNAMIC) {
            dynamicLock = LOCK;
        }
        rawData.reset();
        shortTermData.reset();
        longTermData.reset();
        lastIntervalTime = micros();
    }

    virtual void calibrate() {
		#ifdef DEBUG
			Serial.println("SensorInput::calibrate() not implemented");
		#endif
    }

};
#endif
