#include "Data.h"

class SensorDisplay : public DisplayElement {
	protected:
		int enabled=0;
		Data *data;
		DisplayElement viz; // GRAPH | DIAL | ONOFF 
		Stats stats;
		
	public:

		SensorDisplay(Data* data) : 
		{
			this->data=data;
		}

		SensorDisplay(int x=0, int y=0, int w=0, int h=0) : 
		DisplayElement {x,y,w,h},
		stats {x,y}
		{

		}

		void setViz(DisplayElement *viz) {
			delete viz;
			this->viz = viz;
		}

		virtual void draw() {
			clear();
			//something special to draw some of them from scratch
			#ifdef DEBUG
        		Serial.print(F("draw() not implemented yet for this class"));
        	#endif
        	viz->draw();
		}

		virtual void redraw() {
			//minimal code to change the graph from t-1 state to t state
			#ifdef DEBUG
        		Serial.print(F("redraw() not implemented yet for this class"));
        	#endif
        	viz->redraw();
        	stats.redraw();
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