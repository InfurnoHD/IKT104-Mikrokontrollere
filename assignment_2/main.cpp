#include "mbed.h"
#define WAIT_TIME_MS 100ms


//Define our connections according to what pins they are connected to
DigitalIn button(D0, PullUp);
PwmOut led1(PB_0);
PwmOut led2(PB_4);
PwmOut led3(PB_1);
AnalogIn dimmer(PC_5, 3.3f);


int main()
{

float potValue;

while(1){//Endless while loop
    potValue = dimmer.read();

    if (button.read() == 1){ //If statement to check if button is pushed

        led1 = led2 = led3 = 0.0f;
        
        if (potValue < 0.3f){ //If statement to check potentiometer value

            led1 = potValue*3; //Scale brightness of first LED according to potentiometer resistance

        }
        else if (potValue > 0.3f){ //If statement to check potentiometer value

            led1 = 1; //Led 1 always on
            led2 = (potValue - 0.3) * 3; //Scale led 2

        }
        if (potValue > 0.6f){ //If statement to check potentiometer value

            led1 = 1;
            led2 = 1;
            led3 = (potValue - 0.6) * 3; //Scale led 3

        }
    }
    
    else{

        led1 = led2 = led3 = 1.0f; //If button is pressed, turn all lights on
    }

    ThisThread::sleep_for(WAIT_TIME_MS); //Sleep for 100 ms, meaning 10 loops every second

    }

    
}  

