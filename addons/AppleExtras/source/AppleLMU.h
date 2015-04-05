#include <mach/mach.h>
//#include <Carbon/Carbon.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

void getLightSensors(float *left, float *right);

float getDisplayBrightness(void);
void setDisplayBrightness(float brightness);

float getKeyboardBrightness(void);
void setKeyboardBrightness(float in);

