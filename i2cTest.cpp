
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
    
    unsigned enabled = 1;
    unsigned devAddr = 0x68;
    unsigned char buffer[14];
    
    unsigned char reCharUn;
    
    //Test writeByte
    /** Set full interrupt enabled status.
     * Full register byte for all interrupts, for quick reading. Each bit should be
     * set 0 for disabled, 1 for enabled.
     * @param enabled New interrupt enabled status
     * @see getIntFreefallEnabled()
     * @see MPU6050_RA_INT_ENABLE -> 0x38
     * @see MPU6050_INTERRUPT_FF_BIT
     **/
    temp_sensor -> writeByte(devAddr, 0x38, enabled);

    //Test readByte
    reCharUn = temp_sensor -> readByte(devAddr, 0x38, buffer);
    printf("byte is: %d\n", reCharUn);
    
    enabled = 0;
    
    temp_sensor -> writeByte(devAddr, 0x38, enabled);
    
    //readByte returned 0 and 1 accordingly using 0x38 but not 0x1B or 0x1C
    reCharUn = temp_sensor -> readByte(devAddr, 0x1B, buffer);
    printf("byte is: %d\n", reCharUn);
    
    reCharUn = temp_sensor -> readByte(devAddr, 0x1C, buffer);
    printf("byte is: %d\n", reCharUn);
    
    //New test read and write bits
    temp_sensor -> writeBits(devAddr, 0x1B, 4, 4, 0x01);
    
    reCharUn = temp_sensor -> readBits(devAddr, 0x1B, 4, 2, buffer);
    printf("gyro_config: %d\n", reCharUn);
    
    
    return 0;
}