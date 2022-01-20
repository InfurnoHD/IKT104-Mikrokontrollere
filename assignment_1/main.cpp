#include "mbed.h"
#include <cstdio>

DigitalOut led(PC_5);

BufferedSerial serial_port(USBTX, USBRX, 115200);

int main(){
    char buf[32] = {0};

    while (true)
    {
        if (int num = serial_port.read(buf, sizeof(buf))) {
            if(strstr(buf, "1")){
                led = true;
            }
            else if(strstr(buf, "0")){
                led = false;
            }
            serial_port.write(buf, num);
        }
    }
}