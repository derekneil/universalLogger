class Data{
  private:
    int array[];
    int size;
    int index;
    int min;
    int avg;
    int max;
    int bumped;
  public:
    void add(int val) {
      bumped = array[index];
      array[index] = val;
      index++;
    }
    void reset() {
      zeroArray(array, size, 0);
      index=0;
      bumped=0;
      min=9999;
      avg=0;
      max=-9999;
    }
    void zeroArray(int array[], int len, int val) {
		for(int i=0; i<len; i++){
			array[i] = val;
		}
	}
};