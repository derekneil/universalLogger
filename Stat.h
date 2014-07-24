class Stat : public DisplayElement {
  protected:
    char *label;
    char *lastValue;
    int shaddow;
    int shaddowColour;
    int radius;
    int colour;
  public:
    Stat(int x, int y, int w, int h, char* label, int shaddow, int radius, int colour) : DisplayElement {x,y,w,h} 
    {
        this->label = label;
        this->shaddow = shaddow;
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
        tft.drawFastHLine(x+r -shaddow, y -shaddow  , w-2*r, shaddowColour); // Top
        tft.drawFastVLine(x -shaddow  , y+r -shaddow, h-2*r, shaddowColour); // Left
        tft.drawCircleHelper(x+r -shaddow, y+r -shaddow, r, 1, shaddowColour); //top left corner
        tft.drawCircleHelper(x+w-r-1 -shaddow, y+r -shaddow, r, 2, shaddowColour); //top right corner
        tft.drawCircleHelper(x+r -shaddow, y+h-r-1 -shaddow, r, 8, shaddowColour); //bottom left corner
        tft.drawRoundRect(x, y, w, h, radius, colour);
        int newY = y + (h - CHARHEIGHT)/2;
        int newX = x + (w - (strlen(str) * CHARWIDTH))/2;
        tft.setCursor(newX, newY);
        tft.print(str);
    }

    void pushButton(int x, int y, int w, int h, char* str) {
        tft.fillRect(x-shaddow, y-shaddow, w+shaddow, h+shaddow, ERASECOLOUR);
        tft.drawRoundRect(x, y, w, h, radius, shaddowColour);
        int newY = y + (h - CHARHEIGHT)/2;
        int newX = x + (w - (strlen(str) * CHARWIDTH))/2;
        tft.setCursor(newX, newY);
        tft.print(str);
    }

};