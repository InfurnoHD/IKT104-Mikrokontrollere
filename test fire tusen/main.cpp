#include "DFrobot_RGBLCD.h"
#include "Timer.h"
#include "Watchdog.h"
#include "mbed.h"
#include <chrono>
#include <ratio>

Timer timer;

DigitalIn button1(D0);
DigitalIn button2(D1);
DigitalIn button3(D2);
DigitalIn button4(D3);

PwmOut buzzer(D6);

DFRobot_RGBLCD lcd(16, 2, D14, D15);
 
 
 
//TextLCD(PinName rs, PinName e, PinName d4, PinName d5, PinName d6, PinName d7, LCDType type = LCD16x2);
// Note: VCC=5V, V0=0V, via 330 resistor)

 
Ticker flipper;
Timeout wait_time;
 
volatile int start = 0;
volatile int t1s = 0;
 
volatile unsigned char a = 0;
volatile unsigned char b = 0;
volatile unsigned char c = 0;
 
volatile char seconds = 0;
volatile char minutes = 25;
volatile char hours = 0;
 
void update_lcd() {
    lcd.setCursor(0, 0);
    lcd.printf("%02d:%02d:%02d",seconds);
    lcd.printf("%02d:%02d:%02d  %2x%2x%2x",hours, minutes, seconds,a,b,c);
}
 
void flip() {
      return;    
  }    
 
    
 
 
// Buzzer beep for t seconds
void beep(float t) {
    buzzer = 1;
    wait(t);
    buzzer = 0;
}
 
void beepn(int i, float t) {
    while(i --){
        buzzer = 1;
        wait(t);
        buzzer = 0;
        wait(t);
    }
}
 
// Callback routine is interrupt activated by a debounced pb2 hit
void pb1_hit_callback (void) {
    minutes++;
}
// Callback routine is interrupt activated by a debounced pb2 hit
void pb2_hit_callback (void) {
    minutes--;
}
// Callback routine is interrupt activated by a debounced pb3 hit
void pb3_hit_callback (void) {
    start = (start + 1) & 0x01;
}
 
 
int main () {   
        
    // Clean screen
    lcd.cls();
    lcd.printf("Countdown");
 
    beep(0.1); // init buzzer
    
    update_lcd(); 
          
                  
    flipper.attach(&flip, 1.0); // interval 1 second
  
    button1.attach_deasserted(&pb1_hit_callback);
    pb2.attach_deasserted(&pb2_hit_callback);
    pb3.attach_deasserted(&pb3_hit_callback);
    
    pb1.setSampleFrequency();
    pb2.setSampleFrequency();
    pb3.setSampleFrequency();
    
    while(1) {
        if (t1s){
          t1s = 0;
          myled = !myled;
          count_down();
          update_lcd(); 
        }
          
          if(start)
              count_down();
              
          update_lcd();
          
          // Last 5 seconds
          if(seconds > 0 && seconds <= 5 && minutes == 0 && hours == 0) {
              beep(0.2);
          }
          // Countdown finished
          else if(seconds == 0 && minutes == 0 && hours == 0) {
            flipper.detach();
            beepn(5, 0.2);
          }
         }           
   }
}