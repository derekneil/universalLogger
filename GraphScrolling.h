#ifndef GRAPHSCROLLING_H
#define GRAPHSCROLLING_H

#include "universalLogger.h"
#include "Visualization.h"
#include "SensorData.h"

class GraphScrolling : public Visualization {

  private:

    void redrawSingleGraphLines(int *startX, int last, int temp);

  public:

    int doubleWidth = 0;
    int divider = 1;

    GraphScrolling();

    GraphScrolling(int centerX, int centerY, int w, int h);

	/** this should only be called once to draw after a screen change,
	 * otherwise use redraw for better performance */
    void draw(SensorData *data);

    void draw(SensorData *data, int newDivider);

    void redraw(SensorData *data);
};
#endif
