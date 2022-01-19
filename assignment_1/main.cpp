/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

DigitalOut led1(LED1);
DigitalOut led2(PC_5);

BufferedSerial pc(USBTX, USBRX, 115200);


int main()
{
pc.set_baud(115200);
pc.set_format(8, BufferedSerial::None, 1);
    int toggle;
    
    while (true){
    scanf("%i", &toggle);
    printf("%i", toggle);
    led2.write(toggle);
    }
    
}