#pragma once

#include "DFRobot_RGBLCD.h"
#include "HTS221Sensor.h"
#include "string"

struct data {
  Mutex mutex;
  int buttonState, calls;
  DFRobot_RGBLCD *lcd;
  float temp, hum, tempMet;
  std::string weather, news1, news2, news3;
  Thread menuswitch;
  std::string epochTime;
};

void menuFunc(data *datainstance);

void menuSwitch(data *datainstance);

