class DisplayElement {
	protected:
	    int x=0; // center
	    int y=0; // coordinates
	    int w=0;
	    int h=0;
	public:
		DisplayElement(){}
		DisplayElement(int x=0, int y=0, int w=0, int h=0)
		{
			this->x=x;
			this->y=y;
			this->w=w;
			this->h=h;
		}

		void locate(int x, int y)
		{
			this->x=x;
			this->y=y;
		}

		void locateAndSize(int x, int y, int w, int h)
		{
			this->x=x;
			this->y=y;
			this->w=w;
			this->h=h;
		}

		virtual void draw() {
			#ifdef DEBUG
        		Serial.print(F("draw() not implemented yet for this class"));
        	#endif

		}

		virtual void redraw() {
			#ifdef DEBUG
        		Serial.print(F("redraw() not implemented yet for this class"));
        	#endif
		}

		/** assumes upper left 0,0 start point */
		virtual void clear(int backgroundColour) {
			int startX = x - w/2;
			int startY = y - h/2;
			tft.fillRect(startX , startY, w, h, backgroundColour);
		}
}: