#include "mbed.h"

DigitalOut led1(PB_1);
BufferedSerial serial_port(USBTX, USBRX, 115200);

// main() runs in its own thread in the OS
//samme kode basically som assignment_1
//var for å teste led og sånt
int main()
{
    char buf[32] = {0};
    while (true) {
        if (int num = serial_port.read(buf, sizeof(buf))){
        
        if(strstr(buf, "1")){ //sjekker om 1 (eller 0 på neste) er substrings av det som ligger i bufferen
                led1 = true;
            }
            else if(strstr(buf, "0")){
                led1 = false;
            }
        serial_port.write(buf, num);

        }
    }
}

