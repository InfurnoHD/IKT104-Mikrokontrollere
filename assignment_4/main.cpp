#include "DFRobot_RGBLCD.h"
#include "Timer.h"
#include "Watchdog.h"
#include "mbed.h"
#include <chrono>
#include <ratio>

#define WAIT_TIME 1000

InterruptIn button1(D1, PullUp);
InterruptIn button2(D0, PullUp);
DFRobot_RGBLCD lcd(16, 2, D14, D15);
bool paused = false;

Timer timer;

void changeState(bool *var) { *var = !*var; }

void counter() {

  lcd.printf("%.2f",
             std::chrono::duration<float>{timer.elapsed_time()}.count());
  lcd.setCursor(0, 0);
}

void init() {

  lcd.init();
  lcd.clear();
}

void kick() { Watchdog::get_instance().kick(); }

void pause() { timer.stop(); }

int main() {
  init();
  Watchdog &watchdog = Watchdog::get_instance();
  watchdog.start(10000);
  button2.fall(callback(&changeState, &paused));
  button1.fall(&kick);

  while (true) {

    counter();

    if (!paused) {
      printf("Running\n");
      timer.start();
    } else {
      printf("Paused\n");
      timer.stop();
      watchdog.get_instance().kick();
    }

    ThisThread::sleep_for(100ms);
  }
}