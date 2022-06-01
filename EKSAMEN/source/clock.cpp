#include "mbed.h"
#include "clock.h"
#include <iostream>
#include <ctime>
#include <chrono>

void setClock(data *datainstance){

while (true)
{
    time_t klokke = datainstance->epochTime.c_str();

    char buff[32];

    strftime(buff, 32, "%a %d %b %H:%M\n", datainstance->epochTime);
    printf("%s", buff);
    ThisThread::sleep_for(20s);
}


}

void setAlarm(){


}