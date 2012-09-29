i2CDev contains the lower level read/write bits and bytes.
MPU6050 has the library to read/write from the MPU6050 originally written by Jeff Rowberg.
main combines them all together

Compile using g++ i2CDev.cpp MPU6050.cpp main.cpp

Still not able to get back reading from Gyro/Accel.