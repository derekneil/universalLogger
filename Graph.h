#import "Data.h"

//graph types
#define GRAPHSCROLLING 0
#define GRAPHHIGHLIGHT 1
#define GRAPH....

class Graph {
  private:
    int x; // center
    int y; // coordinates
    int w;
    int h;
    int type;
  public:
    Graph(int x, int y, int w, int h) {
      this->x = x;
      this->y = y;
      this->w = w;
      this->h = h;
      this->type = GRAPHSCROLLING;
    }
    int isPushed(int tx, int ty) {
      return tx > x - w/2  &&  tx < x + w/2
             && ty > y - h/2  &&  ty < y + h/2;
    }

    int setSize(int size) {
      if (size==60 || size==120) {
        this->size = size;
        return 1;
      }
      else {
        return 0;
      }
    }

    void draw(Data data) {
      // #ifdef DEBUG
      //   Serial.print(F("graph draw() "));
      //   Serial.println(F(type));
      // #endif

      if (type==GRAPHSCROLLING) {
        redrawScrollingGraph(x-w/2, y-h/2, w, h, data);
      }
      else if(type==GRAPHHIGHLIGHT) {
        redrawHighlightGraph(x-w/2, y-h/2, w, h, data);
      }
      else { //draw after screen cleared
        //graph side bars
        tft.drawFastVLine(  0, 0, GRAPHHEIGHT, TEXTCOLOUR);
        tft.drawFastVLine(121, 0, GRAPHHEIGHT, TEXTCOLOUR);
        tft.drawFastVLine(198, 0, GRAPHHEIGHT, TEXTCOLOUR);
        tft.drawFastVLine(319, 0, GRAPHHEIGHT, TEXTCOLOUR);

        redrawFromScratchGraph(x-w/2, y-h/2, w, h, data);
      }
    }

    void clearGraph(int graphStartX) {
      tft.fillRect(graphStartX, 0, GRAPHWIDTH, GRAPHHEIGHT, BACKGROUNDCOLOUR);
    }

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

    void redrawHighlightGraph(int x, int y, int w, int h, Data *data) {

    }

    void drawGraphFromScratch(int x, int y, int w, int h, Data *data) {
      
    }

    void redrawScrollingGraph(int x, int y, int w, int h, Data *data) { 
      //int array[], const int arrayIndex, const int arraySize, const int bumped) {

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

        drawDoubleGraphLines(&graphStartX, last, temp);

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
      drawDoubleGraphLines(&graphStartX, last, temp);

    }
};