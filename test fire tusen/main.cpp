#include "DFrobot_RGBLCD.h"
#include "Timer.h"
#include "Watchdog.h"
#include "mbed.h"
#include <chrono>
#include <ratio>

DFRobot_RGBLCD lcd(16, 2, D14, D15);
Timer timer;



void watchdog() {


}

void stopwatch() {

  timer.start();

  lcd.printf("%.2f",
             std::chrono::duration<float>{timer.elapsed_time()}.count());
  lcd.setCursor(0, 0);
}

int main() {

  lcd.init();
  lcd.clear();

  while (true) {
    stopwatch();
  }
}