#ifndef SENSORDISPLAY_H
#define SENSORDISPLAY_H

#include "DisplayElement.h"
#include "Visualization.h"
#include "GraphScrolling.h"
#include "Stats.h"
#include "SensorData.h"

class SensorDisplay : public DisplayElement {
	protected:

	public:
		int enabled = 0;
		SensorData *data;   /** reference to externally owned data */
		Visualization *viz; /** GRAPH | DIAL | ONOFF set by menu control, but owned by this class*/
		Stats stats;        /** block of numerical outputs */

		SensorDisplay(SensorData *dataSource) :
			DisplayElement          {0,0,STDWIDTH,STDHEIGHT},
			viz { new GraphScrolling(0,0,STDWIDTH,STDHEIGHT) },
			stats                   {0,0,STDWIDTH,STDHEIGHT}
		{
			#ifdef DEBUG
				Serial.println(F("SensorDisplay(...)"));
			#endif
			data = dataSource;
		}

		virtual ~SensorDisplay() {
			delete viz;
			delete stats;
			//external owner responsible for freeing data
		}

		virtual void setData(SensorData *data) {
			#ifdef DEBUG
				Serial.println(F("SensorDisplay::setData(...)"));
			#endif
			this->data = data;
		}

		virtual void setViz(Visualization *viz) {
			#ifdef DEBUG
				Serial.println(F("SensorDisplay::setViz(...)"));
			#endif
			delete viz;
			this->viz = viz;
		}

		virtual void draw() {
			#ifdef DEBUG
        		Serial.println(F("SensorDisplay::draw()"));
        	#endif
        	viz->draw();
        	stats.draw();
		}

		virtual void redraw() {
			#ifdef DEBUG
        		Serial.println(F("SensorDisplay::redraw(...)"));
        	#endif
        	viz->redraw();
        	//individual stats.stat 's are redrawn by SensorInput::updateDataAndRedrawStat(...) as their values change
		}

		/** the calling code is responsible for calling redraw after calling this function */
		virtual void locate(int x, int y) {
			#ifdef DEBUG
				Serial.println(F("SensorDisplay::locate(...)"));
			#endif
			DisplayElement::locate(x,y);
			viz->locate(x, y); //TODO i think we need to adjust the coordinates for each here
			stats.locate(x, y);

	    }

		/** the calling code is responsible for calling redraw after calling this function */
	    virtual void locateAndSize(int x, int y, int w, int h) {
			#ifdef DEBUG
				Serial.println(F("SensorDisplay::locate(...)"));
			#endif
			DisplayElement::locateAndSize(x,y,w,h);
			viz->locateAndSize(x, y, w, h); //TODO i think we need to adjust the coordinates for each here
			stats.locateAndSize(x, y, w, h);
	    }

};
#endif
