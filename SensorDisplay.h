#ifndef SENSORDISPLAY_H
#define SENSORDISPLAY_H

#include "universalLogger.h"
#include "DisplayElement.h"
#include "Visualization.h"
#include "SensorData.h"
#include "Stats.h"

class SensorDisplay : public DisplayElement {

	public:
		int enabled     = 0;
		int divider     = 1;
		int dividerChk  = divider;
		SensorData *data;   /** reference to externally owned data */
		Visualization *viz; /** GRAPH | DIAL | ONOFF set by menu control, but owned by this class*/
		Stats stats;        /** block of numerical outputs */

		SensorDisplay();
		SensorDisplay(SensorData *dataSource);

		virtual ~SensorDisplay();

		virtual SensorDisplay& operator= (SensorDisplay *param);

		virtual int operator== (const SensorDisplay *param);

		/** deletes old viz it points to, assumes ownership of new viz being passed to it */
		virtual void setViz(Visualization *viz);

		virtual void draw();

		virtual void redraw();

		virtual void redrawViz();

		virtual void redrawStats();

		/** the calling code is responsible for calling redraw after calling this function */
//		virtual void locate(int x, int y);

		/** the calling code is responsible for calling redraw after calling this function */
	    virtual void locateCenterAndSize(int centerX, int centerY, int w, int h);

	    virtual void reset();

	    /** this seems redudant, but it's so that buttons can check if a SensorInput or SensorDisplay is enabled s*/
	    virtual int isEnabled();

};
#endif
