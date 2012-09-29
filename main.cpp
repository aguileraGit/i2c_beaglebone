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
    unsigned char *buffer_ptr = NULL;
    unsigned char buffer[14];
    
    buffer_ptr = &buffer[0];
    
    
    //Run init functions
    
    //Full reset
    printf("Full reset\n");
    MPU_1 -> reset();
    sleep(1);
    
    //Reset Sensors
    printf("\nReset Sensors\n");
    MPU_1 -> resetSensors();
    sleep(1);
    
    //Set AuxVDDIO
    printf("\nSet AuxVDD IO Level to 0\n");
    MPU_1 -> setAuxVDDIOLevel(0);
    
    //Set WakeCycleEnabled
    printf("\nSet Wake Cycle to enabled\n");
    MPU_1 -> setWakeCycleEnabled(0);
    
    //Set Wake Frequency
    printf("\nSet Wake Frequency to 3\n");
    MPU_1 -> setWakeFrequency(3);

    //Set FIFO enabled
    printf("\nSet FIFO to enabled\n");
    MPU_1 -> setFIFOEnabled(1);
    
    //Set clock
    printf("\nSet/get clock\n");
    //MPU_1 -> setClockSource(MPU6050_CLOCK_PLL_XGYRO);
    MPU_1 -> setClockSource(0);
    MPU_1 -> getClockSource(buffer_ptr);
    printf("getClockSource: %X\n", *buffer_ptr);
    
    //Set Gyroscale
    printf("\nSet/get gyro range\n");
    MPU_1 -> setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    MPU_1 -> getFullScaleGyroRange(buffer_ptr);
    printf("getFullScaleGyroRange: %X\n", *buffer_ptr);
    
    //Set Accel range
    printf("\nSet/get accel range\n");
    MPU_1 -> setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    MPU_1 -> getFullScaleAccelRange(buffer_ptr);
    printf("getFullScaleAccelRange: %X\n", *buffer_ptr);
    
    //Wake it up
    printf("\nWake up\n");
    MPU_1 -> setSleepEnabled(false);
    MPU_1 -> getSleepEnabled(buffer_ptr);
    printf("getSleepEnabled: %X\n", *buffer_ptr);

    //Get device ID
    printf("\nGet Device ID\n");
    MPU_1 -> getDeviceID(buffer_ptr);
    printf("getDeviceID: %X\n", *buffer_ptr);

    //Read 10 X,Y,A Accel values
    printf("\nAccel Values\n");
    for (i= 0; i< 10; i++) {
        
        //Test - Read Accel - Passes
        MPU_1 -> getAccelerationX(buffer_ptr);
        //(((int)buffer[0]) << 8) | buffer[1];
        printf("Accel_X: %d\n", *buffer_ptr);
        
        MPU_1 -> getAccelerationY(buffer_ptr);
        printf("Accel_Y: %d\n", *buffer_ptr);
        
        MPU_1 -> getAccelerationZ(buffer_ptr);
        printf("Accel_Z: %d\n", *buffer_ptr);
        
        
        sleep(1);
        
    }
    
    return 0;
}






