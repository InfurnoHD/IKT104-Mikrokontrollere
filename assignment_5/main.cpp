#include "DFRobot_RGBLCD.h"
#include "Timer.h"
#include "mbed.h"
#include <chrono>
#define wait 1000

InterruptIn button1(D0, PullUp);
DigitalIn button2(D1, PullUp);
DigitalIn button3(D2, PullUp);
DigitalIn button4(D3, PullUp);

PwmOut buzzer(D6);

DFRobot_RGBLCD lcd(16, 2, D14, D15);

Timer timer;

unsigned int seconds = 60;
unsigned long long chronoCount = 0;

bool trykk = false;

void initLCD() {

  lcd.init();
  lcd.clear();
}

void buttonClicked() { trykk = !trykk; }

int main() {
  int count = 0;
  initLCD();
  uint8_t new_time = 0;
  timer.stop();

  while (true) {

    chronoCount =
        std::chrono::duration_cast<chrono::seconds>(timer.elapsed_time())
            .count();

    new_time = seconds - chronoCount;

    lcd.printf("%02d", new_time);

    lcd.setCursor(0, 0);

    if (!button1) {
      buttonClicked();
    }

    if (trykk == true) {

      timer.start();
    }
    if (trykk == false) {

      timer.stop();
    }

    if (button2.read() == 0) {
      if (trykk == true) {

        buttonClicked();
      }
      timer.reset();
      seconds = 60;
      timer.stop();
    }
    if (button3.read() == 0) {
      seconds += 5;
    }
    if (button4.read() == 0) {
      seconds -= 5;
    }
    if (new_time < 1) {
      buzzer.period(1.0 / 659.0);
      buzzer = 0.5;
    } else {
      buzzer = 0.0;
    }
    if (new_time <= 0) {
      timer.stop();
    } else {
    }
    ThisThread::sleep_for(200ms);
  }
}