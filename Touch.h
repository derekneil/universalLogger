#include "DisplayElement.h"

class Touch : public DisplayElement{
  protected:

  public:
    Touch(int x, int y, int w, int h, char* label) : DisplayElement {x,y,w,h}
    {

    }

    virtual int isPushed(int tx, int ty) {
      return tx > x - w/2  &&  tx < x + w/2
             && ty > y - h/2  &&  ty < y + h/2;
    }

    virtual void push() {
      #ifdef DEBUG
        Serial.print(F("push() not implemented yet for this class"));
      #endif
    }

  private:

};