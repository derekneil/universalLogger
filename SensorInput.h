#import "Data.h"

#define UNLOCK 0
#define LOCK 1

#define STATIC 0
#define DYNAMIC 1

#define AVGDETECTION 0
#define PEAKDETECTION 1
#define MINDETECTION 2

class SensorInput{
  private:
    int type;
    int enabled;
    int mode; // STATIC | DYNAMIC
    int low
    int high;
    int dynamicLock;
    int interval;
    int cycles;
    int filter; // MIN | AVG | MAX
    Data *raw;
    Data *shortTerm;
    Data *longTerm;

  public:

    void SensorInput() {
      raw = new Data();
      shortTerm = new Data();
      longTerm = new Data();
    }

    void toggleMode() {
      if (mode==DYNAMIC) {
        mode = STATIC;
        modeBtn->setLabel("STATIC");
      }
      else {
        mode = DYNAMIC;
        modeBtn->setLabel("DYNAMIC");
      }
    }

    void poll() {

    }

    int shouldCalculate(int val) {
      int proceed = 0;
      if( mode == DYNAMIC ) {
        if (dynamicLock==LOCK && val > high) {
          dynamicLock = UNLOCK;
        }
        if (dynamicLock==UNLOCK && val < low) {
          dynamicLock = LOCK;
          proceed = 1;
        }
      }   
      else { //mode == STATIC
        if(index == size) {
          proceed=1;
        }
      }
      return proceed;
    }
    int intervalLapsed() {
      int lapsed = 0;
      int check = micros() - lastIntervalTime;
      if (check > interval){
        #ifdef DEBUG
          Serial.println("interval lapsed - time up");
        #endif
        lapsed = 1;
      }
      else if (raw->index == raw->size) {
        #ifdef DEBUG
          Serial.println("interval lapsed - data full");
        #endif
        lapsed = 1;
      }
      return lapsed;
    }

    void updateDataAndStats(int newReading) {
      raw->add(newReading);
      //see if interval is up or raw data is full
      if (intervalLapsed()) {

        shortTerm->add();
      }

// void updatePeaks(char newLoadCellReading[]) {
  
  int newPeak = shouldCalculatePeakForce(); // need to catch array being full
  if(arrayForceIndex==arrayForceSize) {
    arrayForceIndex=0;
  }
  
  char loadCellString[7];
  sprintf(loadCellString,"%6d", loadCellReadingInt);
  rePrint(280, TXTROW3, 6, loadCellString);
  
  // -- see if it's time to calculate new peak force --
  if ( newPeak==1 ) {

    currentPeakForce = -998;
    for (int i=0; i<arrayForceSize; i++) {
      if(arrayForce[i] > currentPeakForce) {
        currentPeakForce = arrayForce[i];
      }
      arrayForce[i] = -998;
    }
    cycles++;
    rePrint(158, TXTROW1, 6, cycles);
    
    rePrint(292, TXTROW2, 4, currentPeakForce);
    
    // -- update max and avgPeaks --
    if (currentPeakForce > minMaxForce) {
      minMaxForce = currentPeakForce;
      
      char minMaxForceString[5];
      sprintf(minMaxForceString,"%4d", minMaxForce);
      rePrint(30, TXTROW3, 4, minMaxForceString);
    }
    avgForce = (avgForce*(cycles-1) + currentPeakForce) / cycles;
    char avgForceString[5];
    sprintf(avgForceString,"%4.0f", avgForce);
    rePrint(30, TXTROW2, 4, avgForceString);
    
    // -- save peak value -- 
    int bumped2min = arrayShortTermForce[arrayShortTermIndex];
    arrayShortTermForce[arrayShortTermIndex] = currentPeakForce;
    
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

// } //end old updatePeaks function

    }

    void reset() {
      if (mode==DYNAMIC) {
        dynamicLock = LOCK;
      }
      raw->reset();
      shortTerm->reset();
      longTerm->reset();
    }

};