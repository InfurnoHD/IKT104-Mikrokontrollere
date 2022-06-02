#include "clock.h"
#include "mbed.h"
#include <chrono>
#include <ctime>
#include <iostream>

PwmOut buzzer(D6);

DigitalIn button2(D1, PullUp);
DigitalIn button3(D2, PullUp);
DigitalIn button4(D3, PullUp);

DigitalIn blueButton(PC_13, PullUp);

void setInitialClock(data *datainstance) {

  set_time(0);

  time_t klokke = time(NULL);

  strftime(datainstance->clockBuff, 32, "%H:%M", localtime(&klokke));

  datainstance->lcd->printf("%s", datainstance->clockBuff);
}

void setClockAndAlarm(data *datainstance) {

  set_time(datainstance->epochTime);

  bool button2Down = false;
  bool button3Down = false;
  bool userButtonDown = false;

  datainstance->soundAlarm = false;

  datainstance->alarm.tm_sec = -1;

  while (true) {

    time_t klokke = time(NULL);

    datainstance->Clock = *localtime(&klokke);

    strftime(datainstance->clockBuff, 32, "%a %d %b %H:%M",
             &datainstance->Clock);

    if (datainstance->buttonState == 2) {
      if (!button2) {
        if (!button2Down) {

          if (datainstance->alarm.tm_sec == -1) {
            datainstance->alarm.tm_sec = 0;
          }

          if (datainstance->alarm.tm_hour == 23) {
            datainstance->alarm.tm_hour = -1;
          }
          datainstance->alarm.tm_hour += 1;

          button2Down = true;
        }
      } else {
        button2Down = false;
      }

      if (!button3) {
        if (!button3Down) {

          if (datainstance->alarm.tm_sec == -1) {
            datainstance->alarm.tm_sec = 0;
          }

          if (datainstance->alarm.tm_min == 59) {
            datainstance->alarm.tm_min = -1;
          }
          datainstance->alarm.tm_min += 1;

          button3Down = true;
        }
      } else {
        button3Down = false;
      }
    }
    if (datainstance->alarm.tm_hour == datainstance->Clock.tm_hour &&
        datainstance->alarm.tm_min == datainstance->Clock.tm_min &&
        datainstance->Clock.tm_sec == datainstance->alarm.tm_sec) {

      datainstance->soundAlarm = true;
    }
    if (datainstance->soundAlarm == true) {
      buzzer.period(1.0 / 659.0);
      buzzer = 0.5;
    }

    if (!blueButton) {
      if (!userButtonDown) {

        datainstance->soundAlarm = false;
        datainstance->snoozeButtonPressed = false;
        datainstance->timer.reset();
        datainstance->timer.stop();
        buzzer = 0.0;

        userButtonDown = true;
      }
    } else {
      userButtonDown = false;
    }

    bool button4Pressed = false;

    if (!button4) {
      if (!button4Pressed) {

        datainstance->snoozeButtonPressed = true;
        buzzer.suspend();
        snooze(datainstance);

        button4Pressed = true;
      }
    } else {
      button4Pressed = false;
    }

    ThisThread::sleep_for(50ms);
  }
}

void snooze(data *datainstance) {

  int chronoCount = 0;
  datainstance->timer.start();

  while (datainstance->snoozeButtonPressed) {

    chronoCount = std::chrono::duration_cast<chrono::seconds>(
                      datainstance->timer.elapsed_time())
                      .count();

    if (chronoCount >= 300) {
      datainstance->soundAlarm = true;
      datainstance->timer.reset();
      datainstance->timer.stop();
      buzzer.resume();
      datainstance->snoozeButtonPressed = false;
    }
  }
}
