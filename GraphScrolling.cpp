#ifndef GRAPHSCROLLING
#define GRAPHSCROLLING

#include "GraphScrolling.h"
#include "universalLogger.h"
#include "Visualization.h"
#include "Display.h"
#include "SensorData.h"

void GraphScrolling::redrawSingleGraphLines(int *graphStartX, int oldVal, int newVal) {
  if (oldVal > newVal) {
    //erase, aka draw background
    Display::device->drawFastVLine((*graphStartX)++, startY+(h-oldVal), oldVal-newVal, BLACK);
  }
  else if (oldVal < newVal) {
    //add, aka draw white
    Display::device->drawFastVLine((*graphStartX)++, startY+(h-newVal), newVal-oldVal, TEXTCOLOUR);
  }
  else {
    (*graphStartX)++;
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
			Serial.println(F("GraphScrolling( "));
			Serial.print(centerX);
			Serial.print(F(", "));
			Serial.print(centerY);
			Serial.print(F(", "));
			Serial.print(w);
			Serial.print(F(", "));
			Serial.print(h);
			Serial.println(F(" )"));
		}
	#endif

}

/** this should only be called once to draw after a screen change,
* otherwise use redraw for better performance */
void GraphScrolling::draw(SensorData *data) {
  #ifdef DEBUG
    if (Serial) {
      Serial.println(F("GraphScrolling::draw(...)"));
    }
	#endif
  Visualization::clear();

  //draw graph from scratch
  int i = data->index;

  //setup values and use bumped for graphing oldest value in graph
  int newVal = data->array[i]/divider;
  if(newVal < 0) { newVal=0; }                               //XXX change this to support graphs that can handle negative values

  int graphStartX = startX;
  //loop through all the middle values in the graph

  int stop = i - 1; //index latest value was inserted into
  if (stop < 0) { stop=data->size-1; } //avoids % operation
  while( i!=stop) {
     Display::device->drawFastVLine(graphStartX++, startY+h-newVal, newVal, TEXTCOLOUR);
     if (doubleWidth) {
       Display::device->drawFastVLine(graphStartX++, startY+h-newVal, newVal, TEXTCOLOUR);
     }
     i++;
  	if (i==data->size) { i=0; } //avoids % operation
  	newVal = data->array[i]/divider;
  	if(newVal < 0) { newVal=0; }                             //XXX change this to support graphs that can handle negative values
  }

  //draw latest value added to graph (i==stop)
  Display::device->drawFastVLine(graphStartX++, startY+h-newVal, newVal, TEXTCOLOUR);
  if (doubleWidth) {
  	Display::device->drawFastVLine(graphStartX++, startY+h-newVal, newVal, TEXTCOLOUR);
  }

}

void GraphScrolling::draw(SensorData *data, int newDivider) {
	#ifdef DEBUG
	  if (Serial) {
		Serial.print(F("GraphScrolling::draw(..., "));
		Serial.print(newDivider);
		Serial.println(F(" )"));
	  }
	#endif
	divider = newDivider;
	draw(data);
}

void GraphScrolling::redraw(SensorData *data) {
	#ifdef DEBUG
		if (Serial) {
			Serial.println(F("GraphScrolling::redraw(...)"));
		}
	#endif

  int i = data->index;

  //setup values and use bumped for graphing oldest value in graph
  int newVal = data->array[i]/divider;
  if(newVal < 0) { newVal=0; }                              //XXX change this to support graphs that can handle negative values
  int lastI = -1;
  int oldVal = data->bumped / divider;
  if(oldVal < 0) { oldVal=0; }                              //XXX change this to support graphs that can handle negative values

  int graphStartX = startX;

  //loop through all the middle values in the graph
  int stop = i - 1; //index latest value was inserted into
  if (stop < 0) { stop=data->size-1; } //avoids % operation
  while( i!=stop ) {
	redrawSingleGraphLines(&graphStartX, oldVal, newVal);
	if (doubleWidth) {
	  redrawSingleGraphLines(&graphStartX, oldVal, newVal);
	}
    i++;
    if (i==data->size) { i=0; } //avoids % operation
    newVal = data->array[i]/divider;
    if(newVal < 0) { newVal=0; }                             //XXX change this to support graphs that can handle negative values
    lastI = i-1;
    if (lastI <0) { lastI = data->size-1; }
    oldVal = data->array[lastI]/divider;
    if(oldVal < 0) { oldVal=0; }  //avoids % operation
  }

  //draw latest value added to graph (i==stop), with function that determines how to do minimal line draw
  redrawSingleGraphLines(&graphStartX, oldVal, newVal);
  if (doubleWidth) {
    redrawSingleGraphLines(&graphStartX, oldVal, newVal);
  }
}
#endif
