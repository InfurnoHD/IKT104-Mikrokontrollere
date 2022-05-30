#include "mbed.h"
#include "DFRobot_RGBLCD.h"
#include "HTS221Sensor.h"
#include "HTS221_driver.h"


#define WAIT_TIME_MS 1000ms

void gethum(HTS221Sensor senor, DigitalIn button, DFRobot_RGBLCD lcd, float &hum);
void gettemp(HTS221Sensor sensor, DigitalIn button, DFRobot_RGBLCD lcd, float &temp);
void printtemphum (int &count, float &temp, float &hum, DFRobot_RGBLCD lcd);
void knapp (int &count, DigitalIn button);