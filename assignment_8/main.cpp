#include "mbed.h"
#include <chrono>
#include <iostream>

struct Data {

  Mutex mutex;
  Timer timer;
  int minutes = 0;
  int seconds = 0;
};

void minuteCounter(Data *data) {

  while (true) {
    data->mutex.lock();
    data->minutes =
        std::chrono::duration_cast<chrono::minutes>(data->timer.elapsed_time())
            .count();
    data->mutex.unlock();
    ThisThread::sleep_for(734ms);
  }
}
void secondCounter(Data *data) {

  while (true) {
    data->mutex.lock();
    data->seconds =
        std::chrono::duration_cast<chrono::seconds>(data->timer.elapsed_time())
            .count();
    data->mutex.unlock();
    ThisThread::sleep_for(283ms);
  }
}
void printClock(Data *data) {
  while (true) {
    data->mutex.lock();
    printf("%i : %i\n", data->minutes, data->seconds);
    data->mutex.unlock();
    ThisThread::sleep_for(89ms);
  }
}
int main() {

  Data data;

  Thread minuteThread;
  Thread secondThread;
  Thread printThread;

  data.timer.start();

  minuteThread.start(callback(minuteCounter, &data));
  secondThread.start(callback(secondCounter, &data));
  printThread.start(callback(printClock, &data));

  /*minuteThread.set_priority(osPriorityAboveNormal);
  secondThread.set_priority(osPriorityAboveNormal);
  printThread.set_priority(osPriorityHigh7);*/
}