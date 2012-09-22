#include </usr/include/linux/i2c-dev.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "MPU6050.h"
#include "i2CDev.h"

/*
 
 Diego Aguilera
 3-Sep-2012
 
 Attempt to use MPU6050 and i2CDev to get back a value from the gyroscope/accel
 Must be compiled as such: g++ MPU6050_tmp.cpp i2CDev.cpp main.cpp -o "program name"
 
 */

int main() {
    
    MPU6050 *MPU_1 = new MPU6050();
    int i;
    unsigned char buffer[14];
    
    //Run init functions 
    
    //Set clock
    printf("\nSet/get clock\n");
    MPU_1 -> setClockSource(MPU6050_CLOCK_PLL_XGYRO);
    MPU_1 -> getClockSource();
    printf("getClockSource: %X\n", buffer[0]);
    
    //Set Gyroscale
    printf("\nSet/get gyro range\n");
    MPU_1 -> setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    MPU_1 -> getFullScaleGyroRange();
    printf("getFullScaleGyroRange: %X\n", buffer[0]);
    
    //Set Accel range
    printf("\nSet/get accel range\n");
    MPU_1 -> setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    MPU_1 -> getFullScaleAccelRange();
    printf("getFullScaleAccelRange: %X\n", buffer[0]);
    
    //Wake it up
    printf("\nWake up\n");
    MPU_1 -> setSleepEnabled(false);
    MPU_1 -> getSleepEnabled();
    printf("getSleepEnabled: %X\n", buffer[0]);

    //Get device ID
    printf("\nGet Device ID\n");
    MPU_1 -> getDeviceID();
    printf("getDeviceID: %X\n", buffer[0]);
    
    //Read 10 X,Y,A Accel values
    printf("\nAccel Values\n");
    for (i= 0; i< 10; i++) {
        
        //Test - Read Accel - Passes
        int reInt1 = MPU_1 -> getAccelerationX();
        printf("Accel_X: %d\n", reInt1);
        
        int reInt2 = MPU_1 -> getAccelerationY();
        printf("Accel_Y: %d\n", reInt2);
        
        int reInt3 = MPU_1 -> getAccelerationZ();
        printf("Accel_Z: %d\n", reInt3);
        
        
        sleep(1);
        
    }
    
    return 0;
}






