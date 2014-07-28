#ifndef GRAPHSCROLLING_H
#define GRAPHSCROLLING_H

#include "Visualization.h"
#include "Display.h"
#include "SensorData.h"

class GraphScrolling : public Visualization {

  private:
    void drawDoubleGraphLines(int *startX, int last, int temp) {
      if (last > temp) {
        //erase, aka draw background
        Display::device->drawFastVLine((*startX)++, h-last, last-temp, BACKGROUNDCOLOUR);
        Display::device->drawFastVLine((*startX)++, h-last, last-temp, BACKGROUNDCOLOUR);
      }
      else if (last < temp) {
        //add, aka draw white
        Display::device->drawFastVLine((*startX)++, h-temp, temp-last, TEXTCOLOUR);
        Display::device->drawFastVLine((*startX)++, h-temp, temp-last, TEXTCOLOUR);
      }
      else {
        (*startX)++;
        (*startX)++;
      }
    }
    void drawSingleGraphLines(int *startX, int last, int temp) {
      if (last > temp) {
        //erase, aka draw background
        Display::device->drawFastVLine((*startX)++, h-last, last-temp, BACKGROUNDCOLOUR);
      }
      else if (last < temp) {
        //add, aka draw white
        Display::device->drawFastVLine((*startX)++, h-temp, temp-last, TEXTCOLOUR);
      }
      else {
        (*startX)++;
      }
    }
    void drawFromScratch(int x, int y, int w, int h, SensorData *data) {
      int i = data->index + 1;
      if (i==data->size) { i=0; } //avoids % operation

      //setup values and use bumped for graphing oldest value in graph
      int temp = data->array[i]/divider; //TODO divider....
      if(temp < 0) { temp=0; }
//       int lastI = -1;
//       int last = data->bumped / divider;
//       if(last < 0) { last=0; }

      //loop through all the middle values in the graph
      while( i!=data->index ) {

        Display::device->drawFastVLine(x++, h, temp, TEXTCOLOUR);
        if (doubleWidth) {
          Display::device->drawFastVLine(x++, h, temp, TEXTCOLOUR);
        }
        i++;
        if (i==data->size) { i=0; } //avoids % operation
        temp = data->array[i]/divider;
        if(temp < 0) { temp=0; }
//        int lastI = i-1;
//        if (lastI <0) { lastI = data->size-1; }
//        last = data->array[lastI]/divider;
//        if(last < 0) { last=0; }
      }

      //draw latest value added to graph
      Display::device->drawFastVLine(x++, h, temp, TEXTCOLOUR);
      if (doubleWidth) {
        Display::device->drawFastVLine(x++, h, temp, TEXTCOLOUR);
      }
    }

  protected:

  public:
    int doubleWidth = 0;
    int divider     = 5;

    GraphScrolling(int x, int y, int w, int h) : 
    	Visualization {x,y,w,h}
    {

    }

    void draw(SensorData *data) {
	#ifdef DEBUG
		Serial.print(F("GraphScrolling::draw()"));
	#endif
      Visualization::clear();

      //draw graph borders
      Display::device->drawFastVLine(  0, 0, h, TEXTCOLOUR);
      Display::device->drawFastVLine(121, 0, h, TEXTCOLOUR);
      Display::device->drawFastVLine(198, 0, h, TEXTCOLOUR);
      Display::device->drawFastVLine(319, 0, h, TEXTCOLOUR);

      drawFromScratch(x-w/2, y-h/2, w, h, data);

    }

    void redraw(SensorData *data) {
	#ifdef DEBUG
		Serial.print(F("GraphScrolling::redraw()"));
	#endif

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
    	drawSingleGraphLines(&x, last, temp);
		if (doubleWidth) {
		  drawSingleGraphLines(&x, last, temp);
		}
        i++;
        if (i==data->size) { i=0; } //avoids % operation
        temp = data->array[i]/divider;
        if(temp < 0) { temp=0; }
        lastI = i-1;
        if (lastI <0) { lastI = data->size-1; }
        last = data->array[lastI]/divider;
        if(last < 0) { last=0; }
      }

      //draw latest value added to graph
      drawSingleGraphLines(&x, last, temp);
      if (doubleWidth) {
        drawSingleGraphLines(&x, last, temp);
      }
    }
};
#endif
