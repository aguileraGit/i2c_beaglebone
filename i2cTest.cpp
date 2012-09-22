
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
7-Sep-2012
 i2cTest - Find a way to test the actual i2cDev file.
*/


int main() {
    
    i2Cdev *temp_sensor = new i2Cdev();
    
    #define MPU6050_RA_GYRO_CONFIG      0x1B
    #define MPU6050_RA_ACCEL_CONFIG     0x1C
    #define MPU6050_RA_INT_ENABLE       0x38
    
    unsigned enabled = 1;
    unsigned devAddr = 0x68;
    unsigned char buffer[14];
    
    //Test writeByte
    /** Set full interrupt enabled status.
     * Full register byte for all interrupts, for quick reading. Each bit should be
     * set 0 for disabled, 1 for enabled.
     * @param enabled New interrupt enabled status
     * @see getIntFreefallEnabled()
     * @see MPU6050_RA_INT_ENABLE -> 0x38
     * @see MPU6050_INTERRUPT_FF_BIT
     **/
    
    temp_sensor -> writeByte(devAddr, MPU6050_RA_INT_ENABLE, enabled);
    temp_sensor -> readByte(devAddr, MPU6050_RA_INT_ENABLE, buffer);
    printf("MPU6050_RA_INT_ENABLE is: %X\n", buffer[0]);
    
    enabled = 0;
    
    temp_sensor -> writeByte(devAddr, MPU6050_RA_INT_ENABLE, enabled);
    temp_sensor -> readByte(devAddr, MPU6050_RA_INT_ENABLE, buffer);
    printf("MPU6050_RA_INT_ENABLE is: %X\n", buffer[0]);

    printf("\n");
    
    temp_sensor -> readByte(devAddr, MPU6050_RA_GYRO_CONFIG, buffer);
    printf("2-byte is: %X\n", buffer[0]);
    
    temp_sensor -> readByte(devAddr, MPU6050_RA_ACCEL_CONFIG, buffer);
    printf("3-byte is: %X\n", buffer[0]);
    
    //New test read and write bits
    temp_sensor -> writeBits(devAddr, MPU6050_RA_GYRO_CONFIG, 4, 4, 0x01);
    
    temp_sensor -> readBits(devAddr, MPU6050_RA_GYRO_CONFIG, 4, 2, buffer);
    printf("gyro_config: %X\n", buffer[0]);
    
    //Try reading the whole accel
    temp_sensor -> readBytes(devAddr, 0x3B, 14, buffer);
    printf("All values is: %X\n", buffer[6] );
    
    
    return 0;
}







