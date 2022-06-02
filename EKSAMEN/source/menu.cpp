#include "menu.h"
#include "DFRobot_RGBLCD.h"
#include "iostream"
#include "mbed.h"
#include <cstdint>
#include <cstdio>
#include <cstring>

DigitalIn button1(D0, PullUp);
DigitalIn buttonToggleAlarm(D4, PullUp);

void menuSwitch(data *datainstance) {

  int strlenght1 = datainstance->news1.length();
  char newschar1[strlenght1 + 1];
  strcpy(newschar1, datainstance->news1.c_str());

  int strlenght2 = datainstance->news2.length();
  char newschar2[strlenght2 + 1];
  strcpy(newschar2, datainstance->news2.c_str());

  int strlenght3 = datainstance->news3.length();
  char newschar3[strlenght3 + 1];
  strcpy(newschar3, datainstance->news3.c_str());

  int stringCounter = 0;

  datainstance->toggleAlarmButtonPressed = false;
  datainstance->pressCounterToggleAlarmButton = -1;

  datainstance->calls = 100;

  while (true) {

    switch (datainstance->buttonState) {
    case 1:

      datainstance->calls++;
      if (datainstance->calls < 30) {
        break;
      } else {

        datainstance->lcd->clear();
        datainstance->lcd->printf("%s", datainstance->clockBuff);

        if (datainstance->pressCounterToggleAlarmButton == 0) {
          datainstance->lcd->clear();
          datainstance->lcd->printf("%s", datainstance->clockBuff);
          datainstance->lcd->setCursor(0, 1);
          datainstance->lcd->printf("Alarm OFF %i:%i",
                                    datainstance->alarm.tm_hour,
                                    datainstance->alarm.tm_min);

          datainstance->alarm.tm_sec = -2;
        }

        if (datainstance->pressCounterToggleAlarmButton == 1) {
          datainstance->lcd->clear();
          datainstance->lcd->printf("%s", datainstance->clockBuff);
          datainstance->lcd->setCursor(0, 1);
          datainstance->lcd->printf("Alarm     %i:%i",
                                    datainstance->alarm.tm_hour,
                                    datainstance->alarm.tm_min);
          datainstance->alarm.tm_sec = 0;
        }

        datainstance->calls = 0;
      }

      std::cout << datainstance->pressCounterToggleAlarmButton << std::endl;

      break;

    case 2:
      datainstance->calls++;

      if (datainstance->calls < 2) {
        break;
      } else {
        datainstance->lcd->clear();
        datainstance->lcd->printf("Set alarm: ");
        if (datainstance->alarm.tm_hour < 10) {
          datainstance->lcd->printf("0");
        }
        datainstance->lcd->printf("%i:", datainstance->alarm.tm_hour);
        if (datainstance->alarm.tm_min < 10) {
          datainstance->lcd->printf("0");
        }
        datainstance->lcd->printf("%i", datainstance->alarm.tm_min);
      }
      datainstance->calls = 0;

      break;
    case 3:

      datainstance->calls++;
      if (datainstance->calls < 50) {
        break;
      } else {
        datainstance->lcd->clear();
        datainstance->lcd->printf("Temp: %.2f C", datainstance->temp);
        datainstance->lcd->setCursor(0, 1);
        datainstance->lcd->printf("Humidity: %.0f%%", datainstance->hum);
        datainstance->calls = 0;
      }

      break;
    case 4:

      datainstance->calls++;
      if (datainstance->calls < 100) {
        break;
      } else {
        datainstance->lcd->clear();
        datainstance->lcd->printf("%s", datainstance->weather.c_str());
        datainstance->lcd->setCursor(0, 1);
        datainstance->lcd->printf("%.1f Degrees", datainstance->tempMet);
        datainstance->calls = 0;
      }

      break;
    case 5:

      datainstance->breakButton = false;

      char *newschar;

      if (stringCounter > 2) {

        stringCounter = 0;
      }

      if (stringCounter == 2) {

        newschar = newschar3;

      } else if (stringCounter == 1) {

        newschar = newschar2;

      } else if (stringCounter == 0) {

        newschar = newschar1;
      }

      char newsStr[] = "                ";

      strcat(newsStr, newschar);

      size_t foo = strlen(newsStr);
      int LCDCursor = 0;

      int strlenght = strlen(newsStr) + 1;
      datainstance->lcd->clear();
      datainstance->calls++;
      if (datainstance->calls < 30) {
          if (datainstance->breakButton){
              datainstance->buttonState = 1;
          }
        break;
      } else {

        stringCounter++;

        datainstance->lcd->clear();

        while (foo != 0) {
          ThisThread::sleep_for(200ms);
          if (!button1) {
            datainstance->buttonState = 1;
            break;
          }

          if (LCDCursor == (strlenght - 1)) {
            LCDCursor = 0;
          }
          datainstance->lcd->setCursor(0, 0);
          if (LCDCursor < (strlenght - 16)) {
            for (int strChar = LCDCursor; strChar < (LCDCursor + 16);
                 strChar++) {
              if (datainstance->breakButton) {
                  datainstance->buttonState = 1;
                break;
              }
              datainstance->lcd->printf("%c", newsStr[strChar]);
            }
          } else {
            for (int strChar = LCDCursor; strChar < (strlenght - 1);
                 strChar++) {
              if (datainstance->breakButton) {
                  datainstance->buttonState = 1;
                break;
              }
              datainstance->lcd->printf("%c", newsStr[strChar]);
            }
            for (int strChar = 0; strChar <= 16 - (strlenght - LCDCursor);
                 strChar++) {
              if (datainstance->breakButton) {
                  datainstance->buttonState = 1;
                break;
              }
              datainstance->lcd->printf("%c", newsStr[strChar]);
            }
          }
          LCDCursor++;
          foo--;
          ThisThread::sleep_for(100ms);
        }

        datainstance->calls = 0;
        break;
      }
    }
    ThisThread::sleep_for(100ms);
  }
}

void menuFunc(data *datainstance) {

  datainstance->buttonState = 1;
  bool buttonDown = false;
  while (true) {
    datainstance->mutex.lock();

    if (datainstance->buttonState > 5) {

      datainstance->buttonState = 1;
    }

    if (!button1) {
      if (!buttonDown) {

        datainstance->breakButton = true;
        datainstance->buttonState++;
        datainstance->calls = 100;
        buttonDown = true;
      }
    } else {
      buttonDown = false;
    }

    if (!buttonToggleAlarm) {
      if (!datainstance->toggleAlarmButtonPressed) {

        datainstance->pressCounterToggleAlarmButton++;
        datainstance->toggleAlarmButtonPressed = true;

        if (datainstance->pressCounterToggleAlarmButton >= 2) {

          datainstance->pressCounterToggleAlarmButton = 0;
        }
      }

    } else {
      datainstance->toggleAlarmButtonPressed = false;
    }

    datainstance->mutex.unlock();
    ThisThread::sleep_for(10ms);
  }
}