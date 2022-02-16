/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DFRobot_RGBLCD.h"
#include "Timer.h"
#include "mbed.h"
#include <cstdint>

DigitalIn button1(D0, PullUp);
DFRobot_RGBLCD lcd(16, 2, D14, D15);

Timer timer;

#define WAIT_TIME_MS 500
DigitalOut led1(LED1);

int main() {
  uint16_t temp;
  lcd.init();
  lcd.clear();
  timer.start();
  while (true) {
    temp = std::chrono::duration_cast<chrono::seconds>(timer.elapsed_time())
               .count();
    if (button1 == 0) {
      timer.stop();
    }
    lcd.printf("%02d", temp);
    lcd.setCursor(0, 0);
  }
}
