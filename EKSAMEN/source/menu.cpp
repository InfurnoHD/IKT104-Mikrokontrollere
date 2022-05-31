#include "menu.h"
#include "DFRobot_RGBLCD.h"
#include "iostream"
#include "mbed.h"

DigitalIn button1(D0, PullUp);
DigitalIn button2(D1, PullDown);
DigitalIn button3(D2, PullDown);

void menuSwitch(data *datainstance) {

  int temp = 0;

  while (true) {

    datainstance->mutex.lock();

    switch (datainstance->buttonState) {
    case 1:
      datainstance->lcd->clear();
      datainstance->lcd->printf("Case 1");
      break;
    case 2:
      datainstance->lcd->clear();
      datainstance->lcd->printf("Case 2");
      break;
    case 3:
      datainstance->lcd->clear();
      datainstance->lcd->printf("Temp: %.2f C", datainstance->temp);
      datainstance->lcd->setCursor(0, 1);
      datainstance->lcd->printf("Humidity: %.0f%%", datainstance->hum);
      break;
    case 4:
      datainstance->lcd->clear();
      datainstance->lcd->printf("%s", datainstance->weather.c_str());
      datainstance->lcd->setCursor(0, 1);
      datainstance->lcd->printf("%.1f Degrees", datainstance->tempMet);
      break;
    case 5:
      datainstance->lcd->clear();
    }

    datainstance->mutex.unlock();
    ThisThread::sleep_for(500ms);
  }
}

void menuFunc(data *datainstance) {

  datainstance->buttonState = 1;

  while (true) {
    datainstance->mutex.lock();

    if (datainstance->buttonState == 6) {
      datainstance->buttonState = 1;
    }

    if (button1.read() == 0) {

      datainstance->buttonState++;
    }

    datainstance->mutex.unlock();
    ThisThread::sleep_for(150ms);
  }
}
