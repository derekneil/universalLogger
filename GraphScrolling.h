#import "Data.h"

class GraphScrolling : public DisplayElement{

  private:
    void drawDoubleGraphLines(int *graphStartX, int last, int temp) {
      if (last > temp) {
        //erase, aka draw background
        tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT-last, last-temp, BACKGROUNDCOLOUR);
        tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT-last, last-temp, BACKGROUNDCOLOUR);
      }
      else if (last < temp) {
        //add, aka draw white
        tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT-temp, temp-last, TEXTCOLOUR);
        tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT-temp, temp-last, TEXTCOLOUR);
      }
      else {
        (*graphStartX)++;
        (*graphStartX)++;
      }
    }
    void drawSingleGraphLines(int *graphStartX, int last, int temp) {
      if (last > temp) {
        //erase, aka draw background
        tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT-last, last-temp, BACKGROUNDCOLOUR);
      }
      else if (last < temp) {
        //add, aka draw white
        tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT-temp, temp-last, TEXTCOLOUR);
      }
      else {
        (*graphStartX)++;
      }
    }
    void drawFromScratch(int x, int y, int w, int h, Data *data) {
      int i = data->index + 1;
      if (i==data->size) { i=0; } //avoids % operation

      //setup values and use bumped for graphing oldest value in graph
      int temp = data->array[i]/divider; //TODO divider....
      if(temp < 0) { temp=0; }
      // int lastI = -1;
      // int last = data->bumped / divider;
      // if(last < 0) { last=0; }

      //loop through all the middle values in the graph
      while( i!=data->index ) {

        tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT, temp, TEXTCOLOUR);
        if (doubleWidth) {
          tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT, temp, TEXTCOLOUR);
        }
        i++;
        if (i==arraySize) { i=0; } //avoids % operation
        temp = array[i]/divider;
        if(temp < 0) { temp=0; }
        lastI = i-1;
        if (lastI <0) { lastI = arraySize-1; }
        last = array[lastI]/divider;
        if(last < 0) { last=0; }
      }

      //draw latest value added to graph
      if (doubleWidth) {
        tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT, temp, TEXTCOLOUR);
      }
      else {
        tft.drawFastVLine((*graphStartX)++, GRAPHHEIGHT, temp, TEXTCOLOUR);
      }
    }

  protected:

  public:
    int doubleWidth = 0;
    GraphScrolling(int x, int y, int w, int h) : 
    DisplayElement {x,y,w,h}
    {

    }

    void draw(Data data) {
      DisplayElement::clear();

      //draw graph borders
      tft.drawFastVLine(  0, 0, h, TEXTCOLOUR);
      tft.drawFastVLine(121, 0, h, TEXTCOLOUR);
      tft.drawFastVLine(198, 0, h, TEXTCOLOUR);
      tft.drawFastVLine(319, 0, h, TEXTCOLOUR);

      drawFromScratch(x-w/2, y-h/2, w, h, data);

    }

    void redraw(int x, int y, int w, int h, Data *data) { 

      int i = data->index + 1;
      if (i==data->size) { i=0; } //avoids % operation

      //setup values and use bumped for graphing oldest value in graph
      int temp = data->array[i]/divider; //TODO divider....
      if(temp < 0) { temp=0; }
      int lastI = -1;
      int last = data->bumped / divider;
      if(last < 0) { last=0; }

      //loop through all the middle values in the graph
      while( i!=data->index ) {

        if (doubleWidth) {
          drawDoubleGraphLines(&x, last, temp);
        }
        else {
          drawSingleGraphLines(&x, last, temp);
        }
        i++;
        if (i==arraySize) { i=0; } //avoids % operation
        temp = array[i]/divider;
        if(temp < 0) { temp=0; }
        lastI = i-1;
        if (lastI <0) { lastI = arraySize-1; }
        last = array[lastI]/divider;
        if(last < 0) { last=0; }
      }

      //draw latest value added to graph
      if (doubleWidth) {
        drawDoubleGraphLines(&x, last, temp);
      }
      else {
        drawSingleGraphLines(&x, last, temp);
      }
    }
};