#ifndef GRAPHHIGHLIGH_H
#define GRAPHHIGHLIGH_H

#include "universalLogger.h"
#include "Visualization.h"
#include "Display.h"
#include "SensorData.h"

class GraphHighlight : public Visualization {

  private:
    void drawDoubleGraphLines(int *startX, int last, int temp) {
      if (last > temp) {
        //erase, aka draw background
        Display::device->drawFastVLine((*startX)++, GRAPHHEIGHT-last, last-temp, BACKGROUNDCOLOUR);
        Display::device->drawFastVLine((*startX)++, GRAPHHEIGHT-last, last-temp, BACKGROUNDCOLOUR);
      }
      else if (last < temp) {
        //add, aka draw white
        Display::device->drawFastVLine((*startX)++, GRAPHHEIGHT-temp, temp-last, TEXTCOLOUR);
        Display::device->drawFastVLine((*startX)++, GRAPHHEIGHT-temp, temp-last, TEXTCOLOUR);
      }
      else {
        (*startX)++;
        (*startX)++;
      }
    }
    void drawSingleGraphLines(int *startX, int last, int temp) {
      if (last > temp) {
        //erase, aka draw background
        Display::device->drawFastVLine((*startX)++, GRAPHHEIGHT-last, last-temp, BACKGROUNDCOLOUR);
      }
      else if (last < temp) {
        //add, aka draw white
        Display::device->drawFastVLine((*startX)++, GRAPHHEIGHT-temp, temp-last, TEXTCOLOUR);
      }
      else {
        (*startX)++;
      }
    }

  protected:

  public:
    GraphHighlight(int x, int y, int w, int h) : Visualization{x,y,w,h}
    {
		#ifdef DEBUG
		  if (Serial) {
        Serial.println(F("GraphHighlighted(....) "));
			}
    #endif

    }

    void draw(SensorData *data) {
       #ifdef DEBUG
         if (Serial) {
			     Serial.println(F("GraphHighlighted::draw(....) "));
       	}
	     #endif

        //graph side bars
        Display::device->drawFastVLine(  0, 0, h, TEXTCOLOUR); //XXX make these parametric, see GraphScrolling
        Display::device->drawFastVLine(121, 0, h, TEXTCOLOUR);
        Display::device->drawFastVLine(198, 0, h, TEXTCOLOUR);
        Display::device->drawFastVLine(319, 0, h, TEXTCOLOUR);

    }

    void redraw(SensorData *data) {
		#ifdef DEBUG
		  if (Serial) {
        Serial.println(F("GraphHighlighted::redraw(....) "));
			}
    #endif

      //int data->array[], const int arrayIndex, const int data->size, const int bumped) {

      int i = data->index + 1;
      if (i==data->size) { i=0; } //avoids % operation

      //setup values and use bumped for graphing oldest value in graph
      int temp = data->array[i]/divider; //FIXME divider....
      if(temp < 0) { temp=0; }
      int lastI = -1;
      int last = data->bumped / divider;
      if(last < 0) { last=0; }

      //loop through all the middle values in the graph
      while( i!=data->index ) {

        drawDoubleGraphLines(&centerX, last, temp);

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
      drawDoubleGraphLines(&centerX, last, temp);

    }
};
#endif
