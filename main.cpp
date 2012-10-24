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
    unsigned char* buffer_read;
    
    //Contsants
    
    //Gyroscope - Must match MPU6050_GYRO_FS_XXX
    //float FS_SEL = 131; //250
    float FS_SEL = 65.5;  //500
    //float FS_SEL = 32.8;  //1000
    //float FS_SEL = 16.4;  //2000
    
    //Accelerometer - Must match MPU6050_ACCEL_FS_YYY
    //int AFS_SEL = 16384; //2G
    int AFS_SEL = 8192;  //4G
    //int AFS_SEL = 4096;  //8G
    //int AFS_SEL = 2048;  //16G
    
    int interval = 20000; //in microseconds
    
    //Run init functions
    //Full reset
    printf("Full reset\n");
    MPU_1 -> reset();
    usleep(200000);
    
    //Reset Sensors
    printf("\nReset Sensors\n");
    MPU_1 -> resetSensors();
    usleep(200000);
    
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
    MPU_1 -> setClockSource(MPU6050_CLOCK_PLL_XGYRO);
    MPU_1 -> getClockSource();
    buffer_read = MPU_1 -> getBuffer();
    printf("getClockSource: %X\n", *buffer_read);
    
    //Set Gyroscale
    printf("\nSet/get gyro range\n");
    MPU_1 -> setFullScaleGyroRange(MPU6050_GYRO_FS_500);
    MPU_1 -> getFullScaleGyroRange();
    buffer_read = MPU_1 -> getBuffer();
    printf("getFullScaleGyroRange: %X\n", *buffer_read);
    
    //Set Accel range
    printf("\nSet/get accel range\n");
    MPU_1 -> setFullScaleAccelRange(MPU6050_ACCEL_FS_4);
    MPU_1 -> getFullScaleAccelRange();
    buffer_read = MPU_1 -> getBuffer();
    printf("getFullScaleAccelRange: %X\n", *buffer_read);
    
    //Wake it up
    printf("\nWake up\n");
    MPU_1 -> setSleepEnabled(false);
    MPU_1 -> getSleepEnabled();
    buffer_read = MPU_1 -> getBuffer();
    printf("getSleepEnabled: %X\n", *buffer_read);
    
    //Get device ID
    printf("\nGet Device ID\n");
    MPU_1 -> getDeviceID();
    buffer_read = MPU_1 -> getBuffer();
    printf("getDeviceID: %X\n", *buffer_read);
    
    //Start reading values
    printf("\nGyro Values\n");
    for (i= 0; i< 200; i++) {
        
        //Read Gyroscope Values
        MPU_1 -> getRotationX();
        //Return buffer from MPU6050 using getBuffer(). Save it as buffer_read
        buffer_read = MPU_1 -> getBuffer();
        //Bit shift and convert to integer.
        float Gyro_X = (short(buffer_read[0] << 8 | buffer_read[1])) / FS_SEL;
        printf("Gyro_X: %4.5f \t", Gyro_X);
        
        MPU_1 -> getRotationY();
        buffer_read = MPU_1 -> getBuffer();
        float Gyro_Y = (short(buffer_read[0] << 8 | buffer_read[1])) / FS_SEL;
        printf("Gyro_Y: %4.5f \t", Gyro_Y);
        
        MPU_1 -> getRotationZ();
        buffer_read = MPU_1 -> getBuffer();
        float Gyro_Z = (short(buffer_read[0] << 8 | buffer_read[1])) / FS_SEL;
        printf("Gyro_Z: %4.5f \t", Gyro_Z);

        //Read Acceleration Values
        MPU_1 -> getAccelerationX();
        buffer_read = MPU_1 -> getBuffer();
        float Accel_X = (short(buffer_read[0] << 8 | buffer_read[1])) / AFS_SEL;
        printf("Accel_X: %4.5f \t", Accel_X);
        
        MPU_1 -> getAccelerationY();
        buffer_read = MPU_1 -> getBuffer();
        float Accel_Y = (short(buffer_read[0] << 8 | buffer_read[1])) / AFS_SEL;
        printf("Accel_Y: %4.5f \t", Accel_Y);
        
        MPU_1 -> getAccelerationZ();
        buffer_read = MPU_1 -> getBuffer();
        float Accel_Z = (short(buffer_read[0] << 8 | buffer_read[1])) / AFS_SEL;
        printf("Accel_Z: %4.5f \t\n", Accel_Z);
        
        usleep(interval);
        
    }
    
    MPU_1 -> getTemperature();
    buffer_read = MPU_1 -> getBuffer();
    float Temperature = (short(buffer_read[0] << 8 | buffer_read[1])) / 340 + 36.53;
    printf("Temperature: %4.5f\n", Temperature);
    
    
    return 0;
}

















