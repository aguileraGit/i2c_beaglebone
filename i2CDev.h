#include </usr/include/linux/i2c-dev.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//
//  i2CDev.h
//  
//
//  Created by Diego Aguilera on 8/14/12.
//
//

#ifndef _i2CDev_h
#define _i2CDev_h

class i2Cdev {
public:
    i2Cdev();
    ~i2Cdev();
    
    unsigned char readByte(unsigned char devAddr, unsigned char regAddr, unsigned char *data);
    unsigned char readBytes(unsigned char devAddr, unsigned char regAddr, unsigned char length, unsigned char *data);
    unsigned char readBit(unsigned char devAddr, unsigned char regAddr, unsigned char bitNum, unsigned char *data);
    unsigned char readBits(unsigned char devAddr, unsigned char regAddr, unsigned char bitStart, unsigned char length, unsigned char *data);
    
    bool writeByte(unsigned char devAddr, unsigned char regAddr, unsigned char data);
    bool writeBit(unsigned char devAddr, unsigned char regAddr, unsigned char bitNum, unsigned char data);
    bool writeBits(unsigned char devAddr, unsigned char regAddr, unsigned char bitStart, unsigned char length, unsigned char data);
    
private:
    int i2cFile;
    unsigned char I2C_WR_Buf[32];
    unsigned char I2C_RD_Buf[32];
};

#endif /* _I2CDEV_H_ */