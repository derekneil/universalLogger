#ifndef GRAPHSCROLLING
#define GRAPHSCROLLING

#include "GraphScrolling.h"
#include "universalLogger.h"
#include "Visualization.h"
#include "Display.h"
#include "SensorData.h"

void GraphScrolling::redrawSingleGraphLines(int *graphStartX, int last, int temp) {
  if (last > temp) {
    //erase, aka draw background
    Display::device->drawFastVLine((*graphStartX)++, startY+(h-last), last-temp, BLACK);
  }
  else if (last < temp) {
    //add, aka draw white
    Display::device->drawFastVLine((*graphStartX)++, startY+(h-temp), temp-last, TEXTCOLOUR);
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
  int i = data->index + 1;
  if (i==data->size) { i=0; } //avoids % operation

  //setup values and use bumped for graphing oldest value in graph
  int temp = data->array[i]/divider;
  if(temp < 0) { temp=0; }

  int graphStartX = startX;
  //loop through all the middle values in the graph
  while( i!=data->index ) {
     Display::device->drawFastVLine(graphStartX++, startY+h-temp, temp, TEXTCOLOUR);
     if (doubleWidth) {
       Display::device->drawFastVLine(graphStartX++, startY+h-temp, temp, TEXTCOLOUR);
     }
     i++;
  	if (i==data->size) { i=0; } //avoids % operation
  	temp = data->array[i]/divider;
  	if(temp < 0) { temp=0; }
  }

  //draw latest value added to graph
  Display::device->drawFastVLine(graphStartX++, startY+h-temp, temp, TEXTCOLOUR);
  if (doubleWidth) {
  	Display::device->drawFastVLine(graphStartX++, startY+h-temp, temp, TEXTCOLOUR);
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

  int i = data->index + 1;
  if (i==data->size) { i=0; }

  //TODO what is drawing that rogue tall bar to the right of a graph (you have to wait for one full graph cycle to be filled

  //setup values and use bumped for graphing oldest value in graph
  int temp = data->array[i]/divider;
  if(temp < 0) { temp=0; }
  int lastI = -1;
  int last = data->bumped / divider;
  if(last < 0) { last=0; }

  int graphStartX = startX;

  //loop through all the middle values in the graph
  while( i!=data->index ) {
	redrawSingleGraphLines(&graphStartX, last, temp);
	if (doubleWidth) {
	  redrawSingleGraphLines(&graphStartX, last, temp);
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
  redrawSingleGraphLines(&graphStartX, last, temp);
  if (doubleWidth) {
    redrawSingleGraphLines(&graphStartX, last, temp);
  }
}
#endif
