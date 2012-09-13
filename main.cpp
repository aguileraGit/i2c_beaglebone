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
    
    bool return_bool = 0;
    int return_int;
    char return_char = 0;
    unsigned char return_char_un = 0;
    int i;
    unsigned char buffer[14];
    
    //Initialize doesn't return anything
    //MPU_1 -> initialize();
    
    MPU_1 -> setClockSource(MPU6050_CLOCK_PLL_XGYRO);
    MPU_1 -> getClockSource();
    printf("getClockSource: %X\n", buffer[0]);
    
    MPU_1 -> setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    MPU_1 -> getFullScaleGyroRange();
    printf("getFullScaleGyroRange: %X\n", buffer[0]);
    
    MPU_1 -> setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    MPU_1 -> getFullScaleAccelRange();
    printf("getFullScaleAccelRange: %X\n", buffer[0]);
    
    MPU_1 -> setSleepEnabled(false);
    MPU_1 -> getSleepEnabled();
    printf("getSleepEnabled: %X\n", return_bool);
    
    return_char_un = MPU_1 -> getDeviceID();
    printf("getDeviceID: %X\n", buffer[0]);
    
    for (i = 0; i < 20; i++) {
        
        return_int = MPU_1 -> getAccelerationX();
        printf("X_accel: %d\n", return_int);
        sleep(1);
        
        return_int = MPU_1 -> getAccelerationY();
        printf("Y_accel: %d\n", return_int);
        sleep(1);
        
        return_int = MPU_1 -> getAccelerationZ();
        printf("Z_accel: %d\n\n", return_int);
        sleep(1);
    
        
    }
    
    return 0;
}






