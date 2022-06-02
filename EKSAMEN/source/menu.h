#pragma once

#include "DFRobot_RGBLCD.h"
#include "HTS221Sensor.h"
#include "string"

struct data {
  Mutex mutex;
  tm Clock;
  tm alarm;
  Timer timer;
  int buttonState, calls, pressCounterToggleAlarmButton;
  DFRobot_RGBLCD *lcd;
  float temp, hum, tempMet;
  std::string weather, news1, news2, news3;
  Thread menuswitch;
  time_t epochTime;
  char clockBuff[32];
  bool soundAlarm, snoozeButtonPressed, toggleAlarmButtonPressed, breakButton;
};

void menuFunc(data *datainstance);

void menuSwitch(data *datainstance);
