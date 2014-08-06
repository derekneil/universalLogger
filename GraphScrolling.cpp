#ifndef GRAPHSCROLLING
#define GRAPHSCROLLING

#include "GraphScrolling.h"
#include "universalLogger.h"
#include "Visualization.h"
#include "Display.h"
#include "SensorData.h"

void GraphScrolling::redrawSingleGraphLines(int *startX, int last, int temp) {
  if (last > temp) {
    //erase, aka draw background
    Display::device->drawFastVLine((*startX)++, h-last, last-temp, BLACK);
  }
  else if (last < temp) {
    //add, aka draw white
    Display::device->drawFastVLine((*startX)++, h-temp, temp-last, TEXTCOLOUR);
  }
  else {
    (*startX)++;
  }
}

GraphScrolling::GraphScrolling() {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("GraphScrolling()"));
		}
	#endif

}

GraphScrolling::GraphScrolling(int centerX, int centerY, int w, int h) :
	Visualization {centerX,centerY,w,h}
{
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("GraphScrolling(...)"));
		}
	#endif

}

/** this should only be called once to draw after a screen change,
* otherwise use redraw for better performance */
void GraphScrolling::draw(SensorData *data) {
  #ifdef DEBUG
    if (Serial) {
      Serial.println(F("GraphScrolling::draw()"));
    }
	#endif
  Visualization::clear();

 int startX = centerX-w/2;
 int startY = centerY-h/2;

  //draw graph side bars, these might get redrawn by adjacent sensor display, but that's ok
// Display::device->drawFastVLine(startX-1, startY-1, h+2, TEXTCOLOUR);
// Display::device->drawFastVLine(startX+w+1, startY-1, h+2, TEXTCOLOUR);

  //draw graph from scratch
 {
  int i = data->index + 1;
  if (i==data->size) { i=0; } //avoids % operation

  //setup values and use bumped for graphing oldest value in graph
  int temp = data->array[i]/divider;
  if(temp < 0) { temp=0; }

  //loop through all the middle values in the graph
  while( i!=data->index ) {

     Display::device->drawFastVLine(startX++, h, temp, TEXTCOLOUR);
     if (doubleWidth) {
       Display::device->drawFastVLine(startX++, h, temp, TEXTCOLOUR);
     }
     i++;
  	if (i==data->size) { i=0; } //avoids % operation
  	temp = data->array[i]/divider;
  	if(temp < 0) { temp=0; }
  }

  //draw latest value added to graph
  Display::device->drawFastVLine(startX++, h, temp, TEXTCOLOUR);
  if (doubleWidth) {
  	Display::device->drawFastVLine(startX++, h, temp, TEXTCOLOUR);
  }
 }

}

void GraphScrolling::redraw(SensorData *data) {
  #ifdef DEBUG
    if (Serial) {
			Serial.println(F("GraphScrolling::redraw()"));
    }
	#endif

  int i = data->index + 1;
  if (i==data->size) { i=0; }

  //setup values and use bumped for graphing oldest value in graph
  int temp = data->array[i]/divider;
  if(temp < 0) { temp=0; }
  int lastI = -1;
  int last = data->bumped / divider;
  if(last < 0) { last=0; }

  int startX = centerX-w/2;

  //loop through all the middle values in the graph
  while( i!=data->index ) {
	redrawSingleGraphLines(&startX, last, temp);
	if (doubleWidth) {
	  redrawSingleGraphLines(&startX, last, temp);
	}
    i++;
    if (i==data->size) { i=0; } //avoids % operation
    temp = data->array[i]/divider;
    if(temp < 0) { temp=0; }
    lastI = i-1;
    if (lastI <0) { lastI = data->size-1; }
    last = data->array[lastI]/divider;
    if(last < 0) { last=0; }  //avoids % operation
  }

  //draw latest value added to graph with function that determines how to do minimal line draw
  redrawSingleGraphLines(&startX, last, temp);
  if (doubleWidth) {
    redrawSingleGraphLines(&startX, last, temp);
  }
}
#endif
