/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 https://github.com/Eriobis/serial_port_plotter/releases/tag/v1.3.0
 */


#include "mbed.h"
#include "LSM6DS33_GR1.h"
#include <cstring>
#include "all_includes.h"
#include "IHM.h"



#define WAIT_TIME_MS 500 
DigitalOut led1(LED1);
UnbufferedSerial pc(PA_2, PA_3);  // I/O terminal PC
DigitalOut led(PA_15);  // led
LSM6DS33 DOF6(PB_7, PB_6);
Ticker FUSION;
CAN can(PA_11,PA_12 );      //canRX canTX
//
CANMessage msg;            // message recu can
CANMessage can_MsgRx[SIZE_FIFO], can_MsgTx;
char ligne[100];
int flag_fusion=0;
char data[8];
union float2int
    {float f;
    char c[4];
    };
union float2int acc;
//
void fusion()
{
    flag_fusion=1;
    
}
// fonction d'envoi trame de test can
void sendx(char msg,char* data,char size)
{
    //pc.printf("send()\n");
    while(!can.write(CANMessage(msg, data, size)));
}
int main()
{

    printf("This is the bare metal blinky example running on Mbed OS %d.%d.%d.\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
can.frequency(250000);
    
    pc.baud(9600);
    sprintf(ligne,"Hello Accel\r\n");
    pc.write(ligne,strlen(ligne));
   
        led = !led;// init IMU
    DOF6.begin(LSM6DS33::G_SCALE_500DPS,LSM6DS33::A_SCALE_8G,LSM6DS33::G_ODR_104,LSM6DS33::A_ODR_104);
    DOF6.calibration(1000);
    // calcul des offsets gyro tracteur
    
    FUSION.attach(&fusion,0.1);
    
    
    wait_us(1000000);
    float time=0;
    IHM_printf("Hello le monde");
    while(1) {
        
        // lecture accelero / gyro
        if(flag_fusion==1) {
            time=time+1;
            DOF6.readAccel();
            //DOF6.readGyro();
            //DOF6.readTemp();
            sprintf(ligne,"$%f %f %f; ",DOF6.ax,DOF6.ay,DOF6.az);
            pc.write(ligne,strlen(ligne));
            data[0]=0x44;
            data[1]=0x55;
            //sendx(10,data,2);
            acc.f=DOF6.ax;
            data[0]=acc.c[0];
            data[1]=acc.c[1];
            data[2]=acc.c[2];
            data[3]=acc.c[3];
            sendx(11,data,4);
            acc.f=DOF6.ay;
            data[0]=acc.c[0];
            data[1]=acc.c[1];
            data[2]=acc.c[2];
            data[3]=acc.c[3];
            sendx(12,data,4);
            acc.f=DOF6.az;
            data[0]=acc.c[0];
            data[1]=acc.c[1];
            data[2]=acc.c[2];
            data[3]=acc.c[3];
            sendx(13,data,4);
            
        //thread_sleep_for(WAIT_TIME_MS);
        }
    }
}