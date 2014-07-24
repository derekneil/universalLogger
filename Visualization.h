#include "Data.h"

class SensorDisplay : public DisplayElement {
	protected:
		int enabled;
		Data *data;
		DisplayElement viz;
		Stats stats;
		
	public:
		SensorDisplay(int x, int y, int w, int h) : 
		DisplayElement {x,y,w,h},
		viz {x,y,w,h}, 
		stats {x,y}
		{

		}

		virtual void draw() {
			clear();
			//something special to draw some of them from scratch
			#ifdef DEBUG
        		Serial.print(F("draw() not implemented yet for this class"));
        	#endif
		}

		virtual void redraw() {
			//minimal code to change the graph from t-1 state to t state
			#ifdef DEBUG
        		Serial.print(F("redraw() not implemented yet for this class"));
        	#endif
		}

		virtual void locate(int x, int y) {
	      viz.locate(x, y);
	      stats.locate(x, y);
	    }

	    virtual void locateAndSize(int x, int y, int w, int h) {
	      viz.locateAndSize(x, y, w, h);
	      stats.locateAndSize(x, y, w, h);
	    }

};