// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _universalLogger_H_
#define _universalLogger_H_
#include "Arduino.h"
//add your includes for the project universalLogger here

#define DEBUG

#define NUMINPUTS 4
#define NUMREGIONS NUMINPUTS

#define LOADCELL Serial1

#define STDWIDTH 158
#define STDHEIGHT 118

#define ANALOG 0
#define DIGITAL 1

#define CHARHEIGHT 8
#define CHARWIDTH 6

// for reference only
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#ifdef DEBUG
	#define ERASECOLOUR RED
#endif

#ifndef ERASECOLOUR
	#define ERASECOLOUR BLACK
#endif

#define BACKGROUNDCOLOUR BLACK
#define TEXTCOLOUR WHITE

#define SCREENWIDTH 320
#define SCREENHEIGHT 240

#define CENTER_X SCREENWIDTH/2-1
#define CENTER_X1 80
#define CENTER_X2 239

#define CENTER_Y SCREENHEIGHT/2-1
#define CENTER_Y1 59
#define CENTER_Y2 177

#define HALFHEIGHT STDHEIGHT // = CENTER_Y-1
#define FULLHEIGHT 238 // = 118 * 2 + 2

#define HALFWIDTH STDWIDTH // = CENTER_X-1
#define FULLWIDTH 318 // = 158 * 2 + 2


//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project universalLogger here


//Do not add code below this line
#endif /* _universalLogger_H_ */
