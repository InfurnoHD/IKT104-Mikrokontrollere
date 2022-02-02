#include "mbed.h"
#include "DFRobot_RGBLCD.h"
#include "HTS221Sensor.h"
#include "HTS221_driver.h"

#define WAIT_TIME_MS 1000ms

DigitalIn button(D0, PullUp);
DFRobot_RGBLCD lcd(16, 2, D14, D15);
DevI2C i2c(PB_11, PB_10);
HTS221Sensor sensor(&i2c); 

void init(){
    sensor.init(NULL);
    sensor.enable();
    lcd.init();
    lcd.clear();
}

void getHum(float& hum){
    sensor.get_humidity(&hum);
    printf("Humidity: %.2f\n", hum);
}

void getTemp(float& temp){
    sensor.get_temperature(&temp);
    printf("Temperature: %.2f\n\n", temp);
}

void knapp(int& count){
    if (button == 0){
        count++;
    }
}

void print(int& count, float& temp, float& hum){
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

int main(){

    int count = 0;
    float temp = 0;
    float hum = 0;

    init();

    while (true){
        getHum(hum);
        getTemp(temp);
        knapp(count);
        print(count, temp, hum);
    }
}