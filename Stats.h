#include "DisplayElement.h"

class Stats : public DisplayElement{
protected:

public:
	Stat min;
  Stat max;
  Stat interval;
  Stat avg;
  Stat last10avg;
  Stat latest;

  Stats() : DisplayElement {} {}

  /** this is setup for a 158x24 stats display area */
  Stats(int x, int y, int w, int h) : 

  DisplayElement {x,y,w,h},

  min       {x+40,  y+4,  w/2, h/3},
  max       {x+40,  y+12, w/2, h/3},
  interval  {x+40,  y+20, w/2, h/3},

  avg       {x+119, y+4,  w/2, h/3},
  last10avg {x+119, y+12, w/2, h/3},
  latest    {x+119, y+20, w/2, h/3} 
  {

  }

  void locate(int x, int y, int w, int h){
    DisplayElement::locate(x,y);
    min       .locate(x+40,  y+4,);
    max       .locate(x+40,  y+12);
    interval  .locate(x+40,  y+20);
    avg       .locate(x+119, y+4,);
    last10avg .locate(x+119, y+12);
    latest    .locate(x+119, y+20);
  }

  void locateAndSize(int x, int y, int w, int h){
    DisplayElement::locateAndSize(x,y,w,h);
    min       .locateAndSize(x+40,  y+4,  w/2, h/3);
    max       .locateAndSize(x+40,  y+12, w/2, h/3);
    interval  .locateAndSize(x+40,  y+20, w/2, h/3);
    avg       .locateAndSize(x+119, y+4,  w/2, h/3);
    last10avg .locateAndSize(x+119, y+12, w/2, h/3);
    latest    .locateAndSize(x+119, y+20, w/2, h/3);
  }

  void draw() {
    min.draw();
    max.draw();
    interval.draw();
    avg.draw();
    last10avg.draw();
    latest.draw();
  }

  void redraw() {
    draw();
  }
}: