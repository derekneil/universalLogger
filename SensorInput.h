#include "Data.h"
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

class SensorInput{
    protected:
        int type; // ANALOG | DIGITAL
        int pin;
        int enabled;
        int mode; // STATIC | DYNAMIC
        int low;
        int high;
        int dynamicLock = LOCK;
        int interval=1000000; //default to 1 second
        int lastIntervalTime = 0;
        int cycles;
        int filter; // MIN | AVG | MAX
        Data raw;
        Data shortTerm;
        Data longTerm;
        SensorDisplay shortTermDisplay;
        SensorDisplay longTermDisplay;

public:
    SensorInput(int pin, int type) : 
    raw              {RAWDATASIZE},
    shortTermDisplay {STDWIDTH},
    longTermDisplay  {STDWIDTH}
    {
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

    virtual void toggleMode() {
        if (mode==DYNAMIC) {
            mode = STATIC;
            // modeBtn->setLabel("STATIC"); //TODO make second level menu read this
        }
        else {
            mode = DYNAMIC;
        }
    }

    /** Poll sensor input for latest value
    use type parameter 
    */
    virtual int poll() {
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
    if (raw->checkAndResetIndex()) {
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
    virtual void updateDataAndStats(int newReading) {
        raw.insert(newReading);

        //see if interval is up or raw data is full
        if (intervalLapsed(newReading)) {
            cycles++;
            int currentFiltered;
            int found = false;

            if (filter==MINDETECTION) {
                currentFiltered = max();
                for (int i=0; i<RAWDATASIZE; i++) {
                    //find smallest value
                    if(raw.array[i] > currentFiltered) {
                        currentFiltered = raw.array[i];
                    }
                    //reset array to flag value as we go
                    raw.array[i] = max();
                }
                if (currentFiltered!=max()) {
                    found = true;
                }
            }
            else if (filter==AVGDETECTION) {
                currentFiltered = lowest();
                for (int i=0; i<RAWDATASIZE; i++) {
                    //find largest value recorded
                    if(raw.array[i] > currentFiltered) {
                        currentFiltered += raw.array[i];
                    }
                    raw.array[i] = lowest();
                }
                if (currentFiltered!=lowest()) {
                    currentFiltered /= RAWDATASIZE;
                    found = true;
                }
            }
            else if (filter==PEAKDETECTION) {
                currentFiltered = lowest();
                for (int i=0; i<RAWDATASIZE; i++) {
                    //find largest value recorded
                    if(raw.array[i] > currentFiltered) {
                        currentFiltered = raw.array[i];
                    }
                    raw.array[i] = lowest();
                }
                if (currentFiltered!=lowest()) {
                    found = true;
                }
            }

            if (found==true) {
                // do some short term stuff
                shortTermDisplay.stats.latest = currentFiltered;
                // rePrint(292, TXTROW2, 4, currentFiltered);



                shortTermDisplay.stats.redraw();
            }

// void updatePeaks(char newLoadCellReading[]) { ------------- old method pasted in

// -- update max and avgPeaks --
        if (currentFiltered > minMaxForce) {
            minMaxForce = currentFiltered;

            char minMaxForceString[5];
            sprintf(minMaxForceString,"%4d", minMaxForce);
            rePrint(30, TXTROW3, 4, minMaxForceString);
        }
        avgForce = (avgForce*(cycles-1) + currentFiltered) / cycles;
        char avgForceString[5];
        sprintf(avgForceString,"%4.0f", avgForce);
        rePrint(30, TXTROW2, 4, avgForceString);

// -- save peak value -- 
        int bumped2min = arrayShortTermForce[arrayShortTermIndex];
        arrayShortTermForce[arrayShortTermIndex] = currentFiltered;

// -- redraw short term graph --
        redrawScrollingGraph(RIGHTGRAPHPOS, arrayShortTermForce, arrayShortTermIndex, arrayShortTermSize, bumped2min);

        arrayShortTermIndex++;

//see if it's time to calculate new short term peak avg
        if (arrayShortTermIndex==arrayShortTermSize) {
            arrayShortTermIndex=0;

            int nonZero = 0;
            avgShortTermForce = 0;
            for (int i=0; i<arrayShortTermSize; i++) {
                if(arrayShortTermForce[i] > 0) {
                    nonZero++; 
                    avgShortTermForce += arrayShortTermForce[i];
                }
            }
            if (nonZero > 0) { 
                avgShortTermForce /= nonZero;
            }

            char avgShortTermForceString[9];
            sprintf(avgShortTermForceString,"%4d", avgShortTermForce);
            rePrint(280, TXTROW1, 4, avgShortTermForceString);

// -- save short term peak avg --
            int bumped2hr = arrayLongtermForce[arrayLongtermIndex];
            arrayLongtermForce[arrayLongtermIndex] = avgShortTermForce;      

// -- redraw long term graph --
            redrawScrollingGraph(LEFTGRAPHPOS, arrayLongtermForce, arrayLongtermIndex, arrayLongtermSize, bumped2hr);

            arrayLongtermIndex++;

//see if it's time to calculate new long term peak avg
            if (arrayLongtermIndex==arrayLongtermSize) {
                arrayLongtermIndex = 0;
            }

            nonZero = 0;
            avgLongtermForce = 0;
            for (int i=0; i<arrayLongtermSize; i++) {   
                if(arrayLongtermForce[i] > 0) {
                    nonZero++;
                    avgLongtermForce += arrayLongtermForce[i];
                }
            }
            if (nonZero > 0) { 
                avgLongtermForce /= nonZero;
            }

            char avgLongtermForceString[9];
            sprintf(avgLongtermForceString,"%4d", avgLongtermForce);
            rePrint(66, TXTROW1, 4, avgLongtermForceString);

        }

    }

// } ---------------- end old updatePeaks function

}

    virtual void updateDisplay() {
        if (shortTermDisplay.enabled) {
            shortTermDisplay.redDraw();
        }
        if (longTermDisplay.enabled) {
            longTermDisplay.redDraw();
        }
    }

    virtual void reset() {
        if (mode==DYNAMIC) {
            dynamicLock = LOCK;
        }
        raw.reset();
        shortTerm.reset();
        longTerm.reset();
        lastIntervalTime = micros();
    }

};