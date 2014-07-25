#ifndef GRAPH_H
#define GRAPH_H

#include "Visualization.h"
#include "SensorData.h"
#include "DisplayElement.h"
#include "Display.h"

//graph types
#define GRAPHSCROLLING 0
#define GRAPHHIGHLIGHT 1

class Graph : public Visualization {
  protected:

  public:
	int type = GRAPHSCROLLING;
    Graph(int x, int y, int w, int h) :
      DisplayElement{x,y,w,h}
     {
      this->x = x;
      this->y = y;
      this->w = w;
      this->h = h;
//      this->type = GRAPHSCROLLING;
    }
    int isPushed(int tx, int ty) {
      return tx > x - w/2  &&  tx < x + w/2
             && ty > y - h/2  &&  ty < y + h/2;
    }

    void draw(SensorData *data) {
       #ifdef DEBUG
         Serial.print(F("Graph::draw(...) "));
         Serial.println(F(type));
       #endif

      if (type==GRAPHSCROLLING) {
        redrawScrollingGraph(x-w/2, y-h/2, w, h, data);
      }
      else if(type==GRAPHHIGHLIGHT) {
        redrawHighlightGraph(x-w/2, y-h/2, w, h, data);
      }
      else { //draw after screen cleared
        //graph side bars
        Display::device->drawFastVLine(  0, 0, h, TEXTCOLOUR);
        Display::device->drawFastVLine(121, 0, h, TEXTCOLOUR);
        Display::device->drawFastVLine(198, 0, h, TEXTCOLOUR);
        Display::device->drawFastVLine(319, 0, h, TEXTCOLOUR);

        redrawFromScratchGraph(x-w/2, y-h/2, w, h, data);
      }
    }

    void redrawFromScratchGraph(int x, int y, int w, int h, SensorData *data) {

    }

    void clearGraph(int graphStartX) {
      Display::device->fillRect(graphStartX, 0, w, h, BACKGROUNDCOLOUR);
    }

    void drawDoubleGraphLines(int *graphStartX, int last, int temp) {
        if (last > temp) {
          //erase, aka draw background
          Display::device->drawFastVLine((*graphStartX)++, h-last, last-temp, BACKGROUNDCOLOUR);
          Display::device->drawFastVLine((*graphStartX)++, h-last, last-temp, BACKGROUNDCOLOUR);
        }
        else if (last < temp) {
          //add, aka draw white
          Display::device->drawFastVLine((*graphStartX)++, h-temp, temp-last, TEXTCOLOUR);
          Display::device->drawFastVLine((*graphStartX)++, h-temp, temp-last, TEXTCOLOUR);
        }
        else {
          (*graphStartX)++;
          (*graphStartX)++;
        }
    }

    void drawSingleGraphLines(int *graphStartX, int last, int temp) {
        if (last > temp) {
          //erase, aka draw background
          Display::device->drawFastVLine((*graphStartX)++, h-last, last-temp, BACKGROUNDCOLOUR);
        }
        else if (last < temp) {
          //add, aka draw white
          Display::device->drawFastVLine((*graphStartX)++, h-temp, temp-last, TEXTCOLOUR);
        }
        else {
          (*graphStartX)++;
        }
    }

    void redrawHighlightGraph(int x, int y, int w, int h, SensorData *data) {

    }

    void drawGraphFromScratch(int x, int y, int w, int h, SensorData *data) {
      
    }

    void redrawScrollingGraph(int x, int y, int w, int h, SensorData *data) {
      //int array[], const int arrayIndex, const int data->size, const int bumped) {

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

        drawDoubleGraphLines(&x, last, temp);

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
      drawDoubleGraphLines(&x, last, temp);

    }
};
#endif
