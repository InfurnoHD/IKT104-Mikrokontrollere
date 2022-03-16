#include "mbed.h"
#include <chrono>
#include <iostream>

struct Data {

  Mutex mutex;
  Timer timerSeconds;
  Timer timerMinutes;
  int minutes = 0;
  int seconds = 0;
};

void minuteCounter(Data *data) {

  while (true) {
    data->mutex.lock();
    data->minutes = std::chrono::duration_cast<chrono::minutes>(
                        data->timerMinutes.elapsed_time())
                        .count();
    data->mutex.unlock();
    ThisThread::sleep_for(100ms);
  }
}
void secondCounter(Data *data) {

  while (true) {
    data->mutex.lock();
    data->seconds = std::chrono::duration_cast<chrono::seconds>(data->timerSeconds.elapsed_time()).count();
    if (data->seconds == 60) {
      data->timerSeconds.reset();
    }
    data->mutex.unlock();
    ThisThread::sleep_for(100ms);
  }
}
void printClock(Data *data) {
  while (true) {
    data->mutex.lock();
    printf("%i : %i\n", data->minutes, data->seconds);
    data->mutex.unlock();
    ThisThread::sleep_for(500ms);
  }
}
int main() {

  Data data;

  Thread minuteThread;
  Thread secondThread;
  Thread printThread;

  data.timerSeconds.start();
  data.timerMinutes.start();

  minuteThread.start(callback(minuteCounter, &data));
  secondThread.start(callback(secondCounter, &data));
  printThread.start(callback(printClock, &data));

  minuteThread.set_priority(osPriorityAboveNormal);
  secondThread.set_priority(osPriorityAboveNormal);

  while (true) {
    ThisThread::sleep_for(1s);
  }
}