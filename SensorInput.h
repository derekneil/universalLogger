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

#define MINDETECTION 1
#define AVGDETECTION 2
#define PEAKDETECTION 3

#define RAWDATASIZE STDWIDTH*5 //allows for 158*5 = 790 / ~65 readings per second = up to 10 seconds of draw data for an interval

class SensorInput {
  protected:
	int type             = -1; /** ANALOG | DIGITAL */
	int pin              = -1;
	int mode             = STATIC; /** STATIC | DYNAMIC */
	int low              = 10;
	int high             = 50;
	int dynamicLock      = LOCK;
	int interval         = 1000000; /** default to 1 second */
	int lastIntervalTime = 0;
	int cycles           = 0;
	int filter           = AVGDETECTION; /** MIN | AVG | MAX */
	SensorData rawData;
	SensorData shortTermData;
	SensorData longTermData;

    virtual void redrawStat(Stat *stat, int newVal) {
//		#ifdef DEBUG
//			if (Serial) {
                // Serial.println("SensorInput::redrawStat()");
//			}
	   // #endif
		char *newValStr = (char*) malloc(6*sizeof(char));
		sprintf(newValStr, "%4d", newVal);
		if (strcmp(newValStr, stat->getValue()) != 0) {
			stat->setValue(newValStr);
			stat->redraw();
		}
		else {
			free(newValStr);
		}
    }

    virtual void redrawStats(SensorData *sensorData, SensorDisplay *sensorDisplay, int newVal) {
		#ifdef DEBUG
			if (Serial) {
    			Serial.print(F("SensorInput::redrawStats(... "));
				Serial.print(newVal);
				Serial.println(F(" )"));
    			}
    	#endif
        redrawStat(&(sensorDisplay->stats.latest),    newVal);
        redrawStat(&(sensorDisplay->stats.min),       sensorData->min);
        redrawStat(&(sensorDisplay->stats.avg),       sensorData->avg);
        redrawStat(&(sensorDisplay->stats.max),       sensorData->max);
        redrawStat(&(sensorDisplay->stats.last10avg), sensorData->last10avg);
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
    virtual int poll() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput::poll()");
    		}
    	#endif
        int newReading = INT_MIN;
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
            int timeCheck = micros() - lastIntervalTime;
            if (timeCheck > interval){
                #ifdef DEBUG
                    if (Serial) {
            			Serial.println("interval lapsed - time up");
                    }
            	#endif
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
    virtual void updateDataAndRedrawStats(int newReading) {
		#ifdef DEBUG
			if (Serial) {
    			Serial.print(F("SensorInput::updateDataAndStats( "));
				Serial.print(newReading);
				Serial.println(F(" )  "));
    		}
    	#endif
        rawData.insert(newReading);

        //see if interval is up or raw data is full
        if (checkInterval(newReading)) {
            cycles++;
            int currentFiltered = INT_MIN;

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
            if (currentFiltered!=INT_MAX && currentFiltered!=INT_MIN) {
                shortTermData.insert(currentFiltered);
				checkDivider(currentFiltered, &shortTermDisplay);
                if (shortTermDisplay.enabled) {
                	redrawStats(&shortTermData, &shortTermDisplay, currentFiltered);
                	shortTermDisplay.needsRedraw = true;
                }

                // see if we're done a cycle through the short term
                if (shortTermData.checkAndResetIndex()) {

                	//save last shortTermData cycle average to long term
                	int shortAvg = shortTermData.avg;
                    longTermData.insert(shortAvg);
                    checkDivider(shortAvg, &longTermDisplay);
                    if (longTermDisplay.enabled) {
                    	redrawStats(&longTermData, &longTermDisplay, shortAvg);
                    	longTermDisplay.needsRedraw = true;
                    }

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

    virtual void updateViz() {
		#ifdef DEBUG
			if (Serial) {
    			Serial.println("SensorInput::updateViz()");
    		}
    	#endif
		shortTermDisplay.redraw();
		longTermDisplay.redraw();
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
		char *output = (char*) malloc(16*sizeof(char));
    	if (isEnabled()) {
			sprintf(output, ", %d, %d", shortTermData.count, rawData.latest()); //change headers in
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
		char *intervalStr = (char*) malloc(6*sizeof(char));
		sprintf(intervalStr, "%0.1f s", interval/1000000.0);
		shortTermDisplay.stats.interval.setValue(intervalStr);
//		shortTermDisplay.stats.interval.redraw(); //FIXME should calling method be responsible for this???
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
