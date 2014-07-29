#ifndef STAT_H
#define STAT_H

#include "universalLogger.h"
#include "DisplayElement.h"

class Stat : public DisplayElement {
  protected:

  public:
    int colour = TEXTCOLOUR;
    char *label;
    char *lastValue;

    Stat(int centerX=0, int centerY=0, int w=0, int h=0, char* label="");

    ~Stat();

    int operator== (const Stat param);

    /** erase entire space for stat
     * print label
     * print lastValue */
    void draw();

    /** assumes label is already on screen
     * erases lastValue located after the label
     * prints new lastValue in it's place */
    void redraw();

    void reset();

};
#endif
