class TouchButton {
  private:
    int x; // center
    int y; // coordinates
    int w;
    int h;
    char *label;
  public:
    TouchButton(int x, int y, int w, int h, char* label) {
      this->x = x;
      this->y = y;
      this->w = w;
      this->h = h;
      this->label = label;
    }
    int isPushed(int tx, int ty) {
      return tx > x - w/2  &&  tx < x + w/2
             && ty > y - h/2  &&  ty < y + h/2;
    }
    void setLabel(char *newLabel) {
      #ifdef DEBUG
        Serial.print(F("setLabel() "));
        Serial.print(F(label));
        Serial.print(F(" to "));
        Serial.println(F(newLabel));
      #endif
      this->label = newLabel;
    }
    void draw() {
      #ifdef DEBUG
        Serial.print(F("draw() "));
        Serial.println(F(label));
      #endif
      drawButton(x-w/2, y-h/2, w, h, label);
    }
    void push() {
      #ifdef DEBUG
        Serial.print(F("push() "));
        Serial.println(F(label));
      #endif
      pushButton(x-w/2, y-h/2, w, h, label);
    }

  private:
    void drawButton(int x, int y, int w, int h, char* str) {
      int r = 4;
      tft.fillRect(x-shaddow, y-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);
      tft.drawFastHLine(x+r -shaddow, y -shaddow  , w-2*r, SHADDOWCOLOUR); // Top
      tft.drawFastVLine(x -shaddow  , y+r -shaddow, h-2*r, SHADDOWCOLOUR); // Left
      tft.drawCircleHelper(x+r -shaddow, y+r -shaddow, r, 1, SHADDOWCOLOUR); //top left corner
      tft.drawCircleHelper(x+w-r-1 -shaddow, y+r -shaddow, r, 2, SHADDOWCOLOUR); //top right corner
      tft.drawCircleHelper(x+r -shaddow, y+h-r-1 -shaddow, r, 8, SHADDOWCOLOUR); //bottom left corner
      tft.drawRoundRect(x, y, w, h, BUTTONRADIUS,BUTTONCOLOUR);
      int newY = y + (h - CHARHEIGHT)/2;
      int newX = x + (w - (strlen(str) * CHARWIDTH))/2;
      tft.setCursor(newX, newY);
      tft.print(str);
    }

    void pushButton(int x, int y, int w, int h, char* str) {
      tft.fillRect(x-shaddow, y-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);
      tft.drawRoundRect(x, y, w, h, BUTTONRADIUS, SHADDOWCOLOUR);
      int newY = y + (h - CHARHEIGHT)/2;
      int newX = x + (w - (strlen(str) * CHARWIDTH))/2;
      tft.setCursor(newX, newY);
      tft.print(str);
    }

};