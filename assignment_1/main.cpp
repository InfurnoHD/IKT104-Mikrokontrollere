#include "mbed.h"

DigitalOut led(PC_5); //Hvilken kobling på kortet vi bruker

BufferedSerial serial_port(USBTX, USBRX, 115200); //Baud rate



int main(){
    char buf[32] = {0}; //Bufferen som skal inneholde input på mikrokontrolleren

    while (true) //Uendelig while loop
    {
        if (int num = serial_port.read(buf, sizeof(buf))) { //read funksjon for å lese tastetrykk. Liten failsafe med 'int num'
            if(strstr(buf, "1")){ //sjekker om 1 (eller 0 på neste) er substrings av det som ligger i bufferen
                led = true;
            }
            else if(strstr(buf, "0")){
                led = false;
            }
            serial_port.write(buf, num); //printer det som står i bufferen
        }
    }
}