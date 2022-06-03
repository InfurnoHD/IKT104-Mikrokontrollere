#include "menu.h"
#include "DFRobot_RGBLCD.h"
#include "iostream"
#include "mbed.h"
#include <cstdint>
#include <cstdio>
#include <cstring>

// Declaring breadboard objects
DigitalIn button1(D0, PullUp);
DigitalIn buttonToggleAlarm(D4, PullUp);

void menuSwitch(data *datainstance) {

  // Fetching news and converting them to char arrays
  int strlenght1 = datainstance->news1.length();
  char newschar1[strlenght1 + 1];
  strcpy(newschar1, datainstance->news1.c_str());
  int strlenght2 = datainstance->news2.length();
  char newschar2[strlenght2 + 1];
  strcpy(newschar2, datainstance->news2.c_str());
  int strlenght3 = datainstance->news3.length();
  char newschar3[strlenght3 + 1];
  strcpy(newschar3, datainstance->news3.c_str());

  // Variable to keep track of which news title is to be printed
  int stringCounter = 0;

  // Initializing variables which will be used to turn alarm on or off
  datainstance->toggleAlarmButtonPressed = false;
  datainstance->pressCounterToggleAlarmButton = -1;

  // Initializing variable used in custom sleep logic
  datainstance->calls = 100;

  while (true) {

    // Switch menu with a case for each screen
    switch (datainstance->buttonState) {

      // Case 1 is the default screen with a full clock and date, as well as the
      // or or off state of the alarm, should it have been initialized
    case 1:

      // Custom sleep logic. This is redundant for all cases
      datainstance->calls++;
      if (datainstance->calls < 30) {
        break;
      } else {

        datainstance->lcd->clear();
        datainstance->lcd->printf("%s", datainstance->clockBuff);

        // Logic for turning alarm on or off
        if (datainstance->pressCounterToggleAlarmButton == 0) {
          datainstance->lcd->clear();
          datainstance->lcd->printf("%s", datainstance->clockBuff);
          datainstance->lcd->setCursor(0, 1);
          datainstance->lcd->printf("Alarm OFF ");
          if (datainstance->alarm.tm_hour < 10) {
            datainstance->lcd->printf("0");
          }
          datainstance->lcd->printf("%i:", datainstance->alarm.tm_hour);
          if (datainstance->alarm.tm_min < 10) {
            datainstance->lcd->printf("0");
          }
          datainstance->lcd->printf("%i", datainstance->alarm.tm_min);

          datainstance->alarm.tm_sec = 0;
        }
        datainstance->alarm.tm_sec = -2;

        if (datainstance->pressCounterToggleAlarmButton == 1) {
          datainstance->lcd->clear();
          datainstance->lcd->printf("%s", datainstance->clockBuff);
          datainstance->lcd->setCursor(0, 1);
          datainstance->lcd->printf("Alarm     ");
          if (datainstance->alarm.tm_hour < 10) {
            datainstance->lcd->printf("0");
          }
          datainstance->lcd->printf("%i:", datainstance->alarm.tm_hour);
          if (datainstance->alarm.tm_min < 10) {
            datainstance->lcd->printf("0");
          }
          datainstance->lcd->printf("%i", datainstance->alarm.tm_min);

          datainstance->alarm.tm_sec = 0;
        }

        datainstance->calls = 0;
      }

      break;

      // Case 2 is the screen where the alarm is set and printed to the lcd
    case 2:
      datainstance->calls++;
      datainstance->breakButton = false;

      if (datainstance->calls < 2) {
        break;
      } else {
        if (datainstance->breakButton) {
          goto exit2;
        }
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
        datainstance->calls = 0;
      exit2:
        break;
      }

      break;

      // Case 3 is where the ambient humidity and temperature is printed to the
      // lcd
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

      // Case 4 is where the weather is printed to the screen
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

      // Case 5 is where the news titles are being run across the LCD
    case 5:

      // Logic for choosing which title to print
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

      // Char consisting of spaces which will be concatenated to the title char
      // to give room between each news title
      char newsStr[] = "                ";
      strcat(newsStr, newschar);

      size_t foo = strlen(newsStr);
      int LCDCursor = 0;

      int strlenght = strlen(newsStr) + 1;
      datainstance->lcd->clear();
      datainstance->calls++;
      if (datainstance->calls < 1) {
        if (datainstance->breakButton) {
          datainstance->buttonState = 1;
        }
        break;
      } else {

        // Logic for scrolling each news title from right to left. The
        // remaining code in Case 5 is all part of this logic
        stringCounter++;
        datainstance->lcd->clear();
        while (foo != 0) {
          ThisThread::sleep_for(200ms);
          if (datainstance->breakButton) {
            goto exit5;
          }

          if (LCDCursor == (strlenght - 1)) {
            LCDCursor = 0;
          }
          datainstance->lcd->setCursor(0, 0);
          if (LCDCursor < (strlenght - 16)) {
            for (int strChar = LCDCursor; strChar < (LCDCursor + 16);
                 strChar++) {
              if (datainstance->breakButton) {
                goto exit5;
              }

              datainstance->lcd->printf("%c", newsStr[strChar]);
            }
          } else {
            for (int strChar = LCDCursor; strChar < (strlenght - 1);
                 strChar++) {
              if (datainstance->breakButton) {
                goto exit5;
              }

              datainstance->lcd->printf("%c", newsStr[strChar]);
            }
            for (int strChar = 0; strChar <= 16 - (strlenght - LCDCursor);
                 strChar++) {
              if (datainstance->breakButton) {
                goto exit5;
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
    exit5:
      break;
    }

    ThisThread::sleep_for(100ms);
  }
}

// Function that runs in its own thread to keep track of button presses
void menuFunc(data *datainstance) {

  datainstance->buttonState = 1;
  bool buttonDown = false;
  while (true) {

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

    ThisThread::sleep_for(10ms);
  }
}