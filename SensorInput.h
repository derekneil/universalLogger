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

#define MINDETECTION 0
#define AVGDETECTION 1
#define PEAKDETECTION 2

#define RAWDATASIZE 5000 // divided by ~65 readings per second = up to 70 seconds of raw data for an interval

class SensorInput {
  public:
	int mode             = STATIC; /** STATIC | DYNAMIC */
	int interval         = 1000000; /** default to 1 second */
	int filter           = AVGDETECTION; /** MIN | AVG | MAX */

  protected:
	short lastReading    = 0;
	int type             = -1; /** ANALOG | DIGITAL */
	int pin              = -1;
	int low              = 10;
	int high             = 50;
	int dynamicLock      = LOCK;
	int lastIntervalTime = 0;
	char intervalStr[6];
	int cycles           = 0;
	char output[16];
	char newValStr[6];
	SensorData rawData;
	SensorData shortTermData;
	SensorData longTermData;

    virtual void updateStat(Stat *stat, int newVal) {
//		#ifdef DEBUG
//			if (Serial) {
                // Serial.println("SensorInput::redrawStat()");
//			}
	   // #endif

		sprintf(newValStr, "%4d", newVal);
		if (strcmp(newValStr, stat->getValue()) != 0) {
			stat->setValue(newValStr);
			stat->needsRedraw = true;
		}
    }

    virtual void updateStats(SensorData *sensorData, SensorDisplay *sensorDisplay, int newVal) {
		#ifdef DEBUG
			if (Serial) {
    			Serial.print(F("SensorInput::updateStats(... "));
				Serial.print(newVal);
				Serial.println(F(" )"));
    			}
    	#endif
        updateStat(&(sensorDisplay->stats.latest),    newVal);
        updateStat(&(sensorDisplay->stats.min),       sensorData->min);
        updateStat(&(sensorDisplay->stats.avg),       sensorData->avg);
        updateStat(&(sensorDisplay->stats.max),       sensorData->max);
        updateStat(&(sensorDisplay->stats.last10avg), sensorData->last10avg);
    }

  	void checkDivider(int val, SensorDisplay *sd) {
		#ifdef DEBUG
			if (Serial) {
				Serial.print(F("SensorInput::checkDivider( "));
				Serial.print(val);
				Serial.print(F(" )  val/divider "));
				Serial.print(val);
				Serial.print(F(" / "));
				Serial.print(sd->divider);
				Serial.print(F(" = "));
				Serial.print(val/sd->divider);
				Serial.print(F(" < h: "));
				Serial.println(sd->viz->getH());
			}
		#endif
  		while (val/sd->divider > sd->viz->getH() ) {
  			(sd->divider)++;
			#ifdef DEBUG
				if (Serial) {
					Serial.print(F("val/divider "));
					Serial.print(val);
					Serial.print(F(" / "));
					Serial.print(sd->divider);
					Serial.print(F(" = "));
					Serial.println(val/sd->divider);
				}
			#endif
  		}
  	}

  public:
	SensorDisplay shortTermDisplay;
	SensorDisplay longTermDisplay;
	char *label = nullptr;

    SensorInput() :
		rawData          {RAWDATASIZE},
		shortTermData    {STDWIDTH},
		longTermData     {STDWIDTH},
		shortTermDisplay {&shortTermData},
		longTermDisplay  {&longTermData}
    {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput()");
    		}
    	#endif
		setInterval();
    }

    SensorInput(int pin, int type) : 
		rawData          {RAWDATASIZE},
		shortTermData    {STDWIDTH},
		longTermData     {STDWIDTH},
		shortTermDisplay {&shortTermData},
		longTermDisplay  {&longTermData}
    {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println(F("SensorInput( "));
				Serial.print(pin);
				Serial.print(F(", "));
				Serial.print(type);
				Serial.println(F(" )"));
    		}
    	#endif
		setInterval();
        this->pin = pin;
        this->type = type;
        if (pin > 0) {
            if (type==DIGITAL) {
                pinMode(pin, INPUT_PULLUP);
                label = "digital";
            }
            else if (type==ANALOG) {
                pinMode(pin, INPUT);
                label = "analog";
            }
        }
    }

    virtual ~SensorInput() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("~SensorInput()");
    		}
    	#endif
    }

    virtual void toggleMode() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput::toggleMode()");
    		}
    	#endif
        if (mode==DYNAMIC) {
            mode = STATIC;
			#ifdef DEBUG
				if (Serial) {
					Serial.println(F("mode = STATIC"));
				}
			#endif
        }
        else {
            mode = DYNAMIC;
			#ifdef DEBUG
				if (Serial) {
					Serial.println(F("mode = DYNAMIC"));
				}
			#endif
        }
    }

    /** Poll sensor input for latest value
    use type parameter to determine read function used
    */
    virtual short poll() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput::poll()");
    		}
    	#endif
        short newReading = SHORT_MIN;
		if (pin > -1) {
			if (type==ANALOG) {
				newReading = analogRead(pin-14); // 0-1024
			}
			else if (type==DIGITAL) {
				newReading = digitalRead(pin); // 0-1
			}
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
    virtual int checkInterval(int newReading) {
		#ifdef DEBUG
			if (Serial) {
    			Serial.print(F("SensorInput::checkInterval( "));
    			Serial.print(newReading);
    			Serial.println(F(" )"));
    		}
    	#endif
        int lapsed = 0;

        if( mode == DYNAMIC ) {
            if (dynamicLock==LOCK && newReading > high) {
                dynamicLock = UNLOCK;
                #ifdef DEBUG
                    if (Serial) {
            			Serial.println("interval - dynamicLock UNLOCKED");
                    }
            	#endif
            }
            else if (dynamicLock==UNLOCK && newReading < low) {
                dynamicLock = LOCK;
                #ifdef DEBUG
                    if (Serial) {
            			Serial.println("interval lapsed - dynamicLock LOCKed");
                    }
            	#endif
                lapsed = 1;
            }
        }
        else { // mode == STATIC
        	unsigned long tmp = micros();
            unsigned long timeCheck = tmp - lastIntervalTime;
            if (timeCheck > interval){
            	lastIntervalTime = tmp;
                lapsed = 1;
            }
        }

        //make sure storage isn't overflowing
        if (rawData.checkAndResetIndex()) {
            #ifdef DEBUG
                if (Serial) {
        			Serial.println("interval still going - wrapping around data storage");
                }
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
    virtual void updateDataAndStats(short newReading) {
		#ifdef DEBUG
			if (Serial) {
    			Serial.print(F("SensorInput::updateDataAndStats( "));
				Serial.print(newReading);
				Serial.println(F(" )  "));
    		}
    	#endif
		lastReading = newReading;
        rawData.insert(newReading);

        //see if interval is up or raw data is full
        if (checkInterval(newReading)) {
            cycles++;
            short currentFiltered = SHORT_MIN;

            if (filter==MINDETECTION) {
				#ifdef DEBUG
					if (Serial) {
						Serial.print("MINDETECTION");
					}
				#endif
                currentFiltered = rawData.min;
            }
            else if (filter==AVGDETECTION) {
				#ifdef DEBUG
					if (Serial) {
						Serial.print("AVGDETECTION");
					}
				#endif
                currentFiltered = rawData.avg;
            }
            else if (filter==PEAKDETECTION) {
				#ifdef DEBUG
					if (Serial) {
						Serial.print("PEAKDETECTION");
					}
				#endif
                currentFiltered = rawData.max;
            }
			#ifdef DEBUG
				if (Serial) {
					Serial.print(F(" currentFiltered: "));
					Serial.println(currentFiltered);
				}
			#endif
            rawData.reset();

            //see if we have new value
            if (currentFiltered!=SHORT_MAX && currentFiltered!=SHORT_MIN) {
                shortTermData.insert(currentFiltered);
				checkDivider(currentFiltered, &shortTermDisplay);
                if (shortTermDisplay.enabled) {
                	updateStats(&shortTermData, &shortTermDisplay, currentFiltered);
                	shortTermDisplay.needsRedraw = true;
                }

                // see if we're done a cycle through the short term
                if (shortTermData.checkAndResetIndex()) {

                	//save last shortTermData cycle average to long term
                	short shortAvg = shortTermData.avg;
                    longTermData.insert(shortAvg);
                    checkDivider(shortAvg, &longTermDisplay);
                    if (longTermDisplay.enabled) {
                    	updateStats(&longTermData, &longTermDisplay, shortAvg);
                    	longTermDisplay.needsRedraw = true;
                    }

                    //reset the index in the long term data after a cycle complete
                    longTermData.checkAndResetIndex();

                }

            }
        }
    }

    virtual void draw() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput::draw()");
    		}
    	#endif
        if (shortTermDisplay.enabled) {
        	shortTermDisplay.draw();
        }
        if (longTermDisplay.enabled) {
        	longTermDisplay.draw();
        }
    }

    virtual void redraw() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println("SensorInput::redraw()");
			}
		#endif
		shortTermDisplay.redraw();
		longTermDisplay.redraw();
    }

    virtual void redrawViz() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput::redrawViz()");
    		}
    	#endif
		shortTermDisplay.redrawViz();
		longTermDisplay.redrawViz();
    }


    virtual void redrawStats() {
		#ifdef DEBUG
			if (Serial) {
				Serial.println("SensorInput::redrawStats()");
			}
		#endif
    	shortTermDisplay.redrawStats();
    	longTermDisplay.redrawStats();
    }

    virtual int isEnabled() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput::isEnabled()");
    		}
    	#endif
        return shortTermDisplay.enabled || longTermDisplay.enabled;
    }

    /** calling method responsible for freeing memory */
    virtual char* logout() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput::logout()");
    		}
    	#endif
    	if (isEnabled()) {
			sprintf(output, ", %d, %d", shortTermData.count, lastReading); //change headers in
    	}
        return output;
    }

    virtual void reset() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput::reset()");
    		}
    	#endif
        if (mode==DYNAMIC) {
            dynamicLock = LOCK;
        }
        rawData.reset();
        shortTermData.reset();
        longTermData.reset();

        shortTermDisplay.reset();
        longTermDisplay.reset();

        cycles = 0;
        lastIntervalTime = micros();

    }

    virtual void calibrate() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput::calibrate() not implemented");
    		}
    	#endif
    }

	int getInterval() const {
		#ifdef DEBUG
			if (Serial) {
				Serial.println("SensorInput::getInterval()");
			}
		#endif
		return interval;
	}

	void setInterval(int interval = 1000000) {
		#ifdef DEBUG
			if (Serial) {
				Serial.println("SensorInput::setInterval(...)");
			}
		#endif
		this->interval = interval;
		sprintf(intervalStr, "%0.1f s", interval/1000000.0);
		shortTermDisplay.stats.interval.setValue(intervalStr);
		#ifdef DEBUG
			if (Serial) {
				Serial.print("interval stat: ");
				Serial.println(intervalStr);
			}
		#endif

		//IMP what about the longTermDisplay? interval * shortTermdataStorageSize?
		//what about if we switch to linked list variable length "timed experiment" storage?
	}
};
#endif
