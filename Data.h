class Data{
  protected:
    int array[];
    int size;
    int index;
    int min;
    int avg;
    int max;
    int bumped;
    int count;
  public:
 
    Data(int size) {
      array[size] = {0};
      index = bumped = avg = count = 0;
      min = max();
      max = lowest();
    }

    virtual void insert(int val) {
      bumped = array[index];
      array[index] = val;
      index++;
      avg = avg*count+val;
      count++:
      avg /= count;
      if (val>max) {
        max = val;
      }
      if (val<min) {
        min = val;
      }

    }
    virtual int checkAndResetIndex() {
      if (index==size) {
        index=0;
        return true;
      }
      return false;
    }

    virtual void zeroArray(int array[], int len, int val) {
  		for(int i=0; i<len; i++){
  			array[i] = val;
  		}
  	}

    virtual void reset() {
      zeroArray(array, size, 0);
      index = bumped = avg = count = 0;
      min=9999;
      max=-9999;
    }

};