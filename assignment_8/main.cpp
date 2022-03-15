#include "Callback.h"
#include "mbed.h"
#include <chrono>

struct Data {

  Mutex mutex;
  int minutes = 0;
  int seconds = 0;
};

struct secondCounterArgs {
  Data *data;
  Timer *timer;
};

struct minuteCounterArgs {
  Data *data;
  Timer *timer;
};

void secondCounter(secondCounterArgs *args) {
  const auto data = args->data;
  const auto timer = args->timer;
  data->mutex.lock();
  int newValueSeconds =
      std::chrono::duration_cast<chrono::seconds>(timer->elapsed_time())
          .count();
  data->seconds = newValueSeconds;
  data->mutex.unlock();
}

void minuteCounter(minuteCounterArgs *args) {
  const auto data = args->data;
  const auto timer = args->timer;
  data->mutex.lock();
  int newValueMinutes =
      std::chrono::duration_cast<chrono::minutes>(timer->elapsed_time())
          .count();
  data->minutes = newValueMinutes;
  data->mutex.unlock();
}

void printClock(Data *data) {
  data->mutex.lock();

  printf("%i : %i\n", data->minutes, data->seconds);
  data->mutex.unlock();
}

int main() {
  Data data;
  Timer timer;

  Thread minuteThread;
  Thread secondThread;
  Thread printThread;

  secondCounterArgs sca;
  minuteCounterArgs mca;

  timer.start();

  minuteThread.start(callback(secondCounter, &sca));
  secondThread.start(callback(minuteCounter, &mca));
  printThread.start(callback(printClock, &data));

  while (true) {

    secondCounter(&sca);
    minuteCounter(&mca);
    printClock(&data);

    ThisThread::sleep_for(500ms);
  }
}
