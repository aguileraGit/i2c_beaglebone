#include "i2CDev.h"

/*
 
 Diego Aguilera
 
 Using code from: https://github.com/ajaykumarkannan/Beaglebone-I2C/blob/master/src/i2c.cpp and
 http://www.i2cdevlib.com/devices
 Also this guy has helped http://www.youtube.com/user/DerekMolloyDCU
 
 14-Aug-2012 - First attempt at converting the i2Cdev.cpp Arduino code to C Code.
 
 Only plan on creating read/write byte, open/close, and read/write bit. The MPU6050 only uses those commands. Hopefully, the code will be easy to port over. Hopefully.
 
 I have an error with the bit read/write. I need to be able to set the bit to write as 1 or 0 and not just 0.
 
 21-Aug-2012 - Code is compiling.
 
 28-Aug-2012 - Need to add readBits, readBytes, writeBits, and writeBytes before continuing. Finally figured out how to compile MPU6050 code, but can't continue
 until those functions are added.
 
 29-Aug-2012 - Added those missing functions. Compiles fine. Still can't compile MPU6050 code. PS, I was wrong on my comment from the 14th. Way wrong.
 
 
 */




// Default constructor.
i2Cdev::i2Cdev() {
}

// Default deconstructor.
i2Cdev::~i2Cdev() {
}

/** Read single byte from an 8-bit device register.
 
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @return Status of read operation (true = success)
 */
unsigned char i2Cdev::readByte(unsigned char devAddr, unsigned char regAddr, unsigned char *data) {
    
    //Open for reading
    i2cFile = open("/dev/i2c-3", O_RDWR);
    if (i2cFile < 0) {
        perror("Error opening");
        exit(1);
    }
    
    //Set the buffer to the address
    I2C_WR_Buf[0] = regAddr;
    
    
    //Set Address
    if (ioctl(i2cFile, I2C_SLAVE, devAddr) < 0) {
        perror("Error setting address");
        exit(1);
    }
    
    //Write to declare where we're reading from
    if(write(i2cFile, I2C_WR_Buf, 1) !=1){
        perror("Write Error");
    }
    
    //Set Address
    if (ioctl(i2cFile, I2C_SLAVE, devAddr) < 0) {
        perror("Error setting address");
        exit(1);
    }
    
    //Read reg_ADDR
    if(read(i2cFile, I2C_RD_Buf, 1) !=1){
        perror("Read Error");
    }
    
    //Close
    close(i2cFile);
    
    //Save buffer... to what? Error.
    *data = I2C_RD_Buf[0];
    
    //printf("readByte - I2C_RD_Buffer: %X\n", I2C_RD_Buf[0]);
    printf("i2C - readByte - data: %X\n", *data);
    
    //Made it this far. Everything must of passed. Return 0. Usually 0 is success, but this would cause it to not go into readBit if statement.
    return 1;
    
}


/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (-1 indicates failure)
 */
unsigned char i2Cdev::readBytes(unsigned char devAddr, unsigned char regAddr, unsigned char length, unsigned char *data) {
    
    I2C_WR_Buf[0] = regAddr;
    int i;
    
    //Open for reading
    i2cFile = open("/dev/i2c-3", O_RDWR);
    if (i2cFile < 0) {
        perror("Error opening");
        exit(1);
    }
    
    //Set Address
    if (ioctl(i2cFile, I2C_SLAVE, devAddr) < 0) {
        perror("Error setting address");
        exit(1);
    }
    
    if(write(i2cFile, I2C_WR_Buf, 1) != 1) {
        perror("Write Error");
    }
    
    //Set Address
    if (ioctl(i2cFile, I2C_SLAVE, devAddr) < 0) {
        perror("Error setting address");
        exit(1);
    }
    
    if(read(i2cFile, I2C_RD_Buf, length) != length){
        perror("Read Error");
    }
    
    //Close
    close(i2cFile);
    
    //Save buffer to data... Error data needs to be define or something.
    *data = I2C_RD_Buf[length];
    
    //printf("readBytes - I2C_RD_Buffer: %X\n", I2C_RD_Buf[length]);
    for(i = 0; i< length; i++){
        printf("i2c - readBytes - data: %X\n", *(data + i) );
    }
    
    //Return number of bytes returned.
    return length;
}


/** Read a single bit from an 8-bit device register.
 
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @return Status of read operation (true = success)
 */
unsigned char i2Cdev::readBit(unsigned char devAddr, unsigned char regAddr, unsigned char bitNum, unsigned char *data) {
    
    unsigned char b;
    unsigned char count = readByte(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    
    
    //For testing actual output
    //printf("i2c - readBit - Count: %X\n", count);
    printf("i2c - readBit - data: %X\n", *data);
    
    return count;
    
}


/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
unsigned char i2Cdev::readBits(unsigned char devAddr, unsigned char regAddr, unsigned char bitStart, unsigned char length, unsigned char *data) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    unsigned char count, b;
    if ((count = readByte(devAddr, regAddr, &b)) != 0) {
        unsigned char mask = ((1 << length) - 1) << (bitStart - length + 1);
        //printf("i2c - readBits - Mask: %X\n", b);
        b &= mask;
        //printf("i2c - readBits - B: %X\n", b);
        b >>= (bitStart - length + 1);
        *data = b;
    }
    
    //For testing actual output
    //printf("i2c - readBits - b: %c\n", b);
    printf("i2c - readBits - data: %X\n", *data );
    
    return count;
    
    
}


/** Write a single bit in an 8-bit device register.
 
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */


bool i2Cdev::writeBit(unsigned char devAddr, unsigned char regAddr, unsigned char bitNum, unsigned char data) {
    
    unsigned char b;
    readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    printf("i2c - writeBit - b: %X\n", b);
    return writeByte(devAddr, regAddr, b);
}


/** Write single byte to an 8-bit device register.
 
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool i2Cdev::writeByte(unsigned char devAddr, unsigned char regAddr, unsigned char data) {
    
    //Open for reading
    i2cFile = open("/dev/i2c-3", O_RDWR);
    if (i2cFile < 0) {
        perror("Error opening");
        exit(1);
    }
    
    //Set Address
    if (ioctl(i2cFile, I2C_SLAVE, devAddr) < 0) {
        perror("Error setting address");
        exit(1);
    }
    
    //Create buffer to write
    I2C_WR_Buf[0] = regAddr;
    I2C_WR_Buf[1] = data;
    
    //Write
    if(write(i2cFile, I2C_WR_Buf, 2) != 2) {
        perror("Write Error");
        
    }
    
    //Close
    close(i2cFile);
    
    printf("i2C - writeByte - data: %X\n", data);
}


/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool i2Cdev::writeBits(unsigned char devAddr, unsigned char regAddr, unsigned char bitStart, unsigned char length, unsigned char data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    
    unsigned char b;
    if (readByte(devAddr, regAddr, &b) != 0) {
        //printf("In like flint\n");
        unsigned char mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        printf("i2c - writeBit - b: %X\n", b);
        return writeByte(devAddr, regAddr, b);
    } else {
        //else was already here
        printf("writeBits - No bueno!\n");
        return false;
    }
}


/*

int main() {
    i2Cdev *temp_sensor = new i2Cdev();
    
    unsigned enabled = 1;
    unsigned devAddr = 0x68;
    unsigned char buffer[14]; //14
    int i;
    
    //Test INT_ENABLE register
    printf("INT Enable test\n");
    temp_sensor -> writeByte(devAddr, 0x38, enabled);
    temp_sensor -> readByte(devAddr, 0x38, buffer);
    printf("Enabled should be 1. Result is: %X\n", buffer[0]);
    
    enabled = 0;
    
    temp_sensor -> writeByte(devAddr, 0x38, enabled);
    temp_sensor -> readByte(devAddr, 0x38, buffer);
    printf("Enabled should be 0. Result is: %X\n", buffer[0]);
    
    printf("\n");
    
    
    
    //Test GYRO_CONFIG
    printf("Gyro config test\n");
    temp_sensor -> writeBits(devAddr, 0x1B, 4, 2, 1);
    temp_sensor -> readByte(devAddr, 0x1B, buffer);
    temp_sensor -> readBits(devAddr, 0x1B, 4, 2, buffer);
    printf("gyro_config: %X\n", buffer[0]);
    
    printf("\n");
    
    
    //getdeviceid
    printf("Get Device ID\n");
    temp_sensor -> readBits(devAddr, 0x75, 6, 6, buffer);
    printf("id is: %X\n", buffer[0]);
    
    printf("\n");
    
    //sleepEnabled
    printf("Sleep enabled = 0 Test...\n");
    //get byte
    temp_sensor -> readByte(devAddr, 0x6B, buffer);
    printf("Full byte is: %X\n", buffer[0]);
    temp_sensor -> writeBit(devAddr, 0x6B, 6, 0);
    sleep(1);
    printf("\n");
    temp_sensor -> readBit(devAddr, 0x6B, 6, buffer);
    printf("Sleep enabled?: %X\n", buffer[0]);
    
    printf("\n");
    
    //Leave sleep enabled off
    /*
    printf("Sleep enabled = 1 Test...\n");
    //get byte
    temp_sensor -> readByte(devAddr, 0x6B, buffer);
    printf("Full byte is: %X\n", buffer[0]);
    temp_sensor -> writeBit(devAddr, 0x6B, 6, 1);
    sleep(1);
    printf("\n");
    temp_sensor -> readBit(devAddr, 0x6B, 6, buffer);
    printf("Sleep enabled?: %X\n", buffer[0]);
    
    */
    /*
    //Read from Accel
    printf("\nRead from Accel\n");
    
    for (i= 0; i< 3; i++) {

        temp_sensor -> readBytes(devAddr, 0x3B, 2, buffer);
        printf("Accel_X: %X, %X\n", buffer[0], buffer[1] );
        //Convert to int
        //unsigned int reInt1  = (((int)buffer[0]) << 8) | buffer[1];
        //printf("Accel_X: %d\n", reInt1);
        
        temp_sensor -> readBytes(devAddr, 0x3D, 2, buffer);
        printf("Accel_Y: %X, %X\n", buffer[0], buffer[1] );
        //Convert to int
        //unsigned int reInt2  = (((int)buffer[0]) << 8) | buffer[1];
        //printf("Accel_Y: %d\n", reInt2);
        
        temp_sensor -> readBytes(devAddr, 0x3F, 2, buffer);
        printf("Accel_Z: %X, %X\n", buffer[0], buffer[1] );
        //Convert to int
        //unsigned int reInt3  = (((int)buffer[0]) << 8) | buffer[1];
        //printf("Accel_Z: %d\n\n", reInt3);
        
        
        sleep(1);
        
    }
    
    //Try reading the whole accel
    temp_sensor -> readBytes(devAddr, 0x3B, 14, buffer);
    printf("All values is: %X, %X, %X\n", buffer[0], buffer[1], buffer[2]);
    
    
    
    
    return 0;
}


*/

