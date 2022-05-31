#include "humidtemp.h"
#include <cstdio>
#include <memory>


void gethum(HTS221Sensor sensor, DigitalIn button, DFRobot_RGBLCD lcd, float &hum)
{
    sensor.get_humidity(&hum);
    printf("Humidity: %.2f\n", hum);
}


void gettemp(HTS221Sensor sensor, DigitalIn button, DFRobot_RGBLCD lcd, float &temp)
{
sensor.get_temperature(&temp);
printf("Temperature: %.2f\n\n", temp);
}

void printtemphum (int count, float temp, float hum, DFRobot_RGBLCD lcd)
{
    if (count % 2 == 0){
        lcd.printf("Temp: %.2f     ", temp);
        if (temp < 20){
                lcd.setRGB(0,0,255);
        }
        else if (temp > 24){
            lcd.setRGB(255,0,0);
        }
        else{
            lcd.setRGB(255,140,0);
        }
        ThisThread::sleep_for(WAIT_TIME_MS);
        lcd.setCursor(0,0);
    }
    else{
        lcd.printf("Humidity: %.2f ", hum);
        lcd.setRGB(255-(hum*2.55),255-(hum*2.55),255);
        ThisThread::sleep_for(WAIT_TIME_MS);
        lcd.setCursor(0,0);
    }   
}


void knapp(int& count, DigitalIn button){
    if (button == 0){
        count++;
    }
}


