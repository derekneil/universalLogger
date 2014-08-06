#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "universalLogger.h"
#include "DisplayElement.h"
#include "SensorData.h"

/** wrapper class for sensor visualizations such as graphs, dials, and on/off etc..  */
class Visualization : public DisplayElement {
  	
  public:
	int doubleWidth = 0; /** used to toggle between 1:1 and 2:1 data to screen pixel ratios for graphs*/

	Visualization();

	Visualization(int centerX, int centerY, int w, int h);

	virtual ~Visualization();

	virtual void draw(SensorData *data);

	virtual void redraw(SensorData *data);

	virtual void clear(int colour=BLACK);

};
#endif
