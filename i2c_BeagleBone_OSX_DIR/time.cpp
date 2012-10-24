#include </usr/include/linux/i2c-dev.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "DS1307.h"
#include "i2CDev.h"
#include "time.h"

#include <time.h>
#include <sys/time.h>


/*
 
 Diego Aguilera
 18-Oct-2012
 
 RTC - DS1307. Note: Has the same address as the Gyro/Accel
 
 23-Oct-2012: Code works. Just need to put it in a start up script. 
 
 */

int main() {
    
    DS1307 *DS1307_clk = new DS1307();
    int i;
    unsigned char* buffer_read;
    
    short year, month, day, hours, minutes, seconds;
    
    bool mode12;

    DS1307_clk -> setClockRunning(true);
    DS1307_clk -> testConnection();
    
    //Uncomment to set the time/date. Year, month, day, hour, minute, sec. Don't forget to comment out again.
    //DS1307_clk -> setDateTime24(2012, 10, 23, 20, 33, 0);
    
    DS1307_clk -> getYear();
    buffer_read = DS1307_clk -> getBuffer();
    //Convert BCD to int
    year = ( (buffer_read[0] & 0x0F) + ((buffer_read[0] & 0x70) >> 4) * 10 ) + 2000;
    printf("Year: %d\n", year);
    
    DS1307_clk -> getMonth();
    buffer_read = DS1307_clk -> getBuffer();
    month = (buffer_read[0] & 0x0F) + ((buffer_read[0] & 0x10) >> 4) * 10;
    printf("Month: %d\n", month);
    
    DS1307_clk -> getDay();
    buffer_read = DS1307_clk -> getBuffer();
    day = (buffer_read[0] & 0x0F) + ((buffer_read[0] & 0x30) >> 4) * 10;
    printf("Day: %d\n", day);
    
    DS1307_clk -> getHours24();
    buffer_read = DS1307_clk -> getBuffer();
    mode12 = buffer_read[0] & 0x40;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        hours = (buffer_read[0] & 0x0F) + ((buffer_read[0] & 0x10) >> 4) * 10;
        
        // convert 12-hour to 24-hour format, since that's what's requested
        if (buffer_read[0] & 0x20) {
            // currently PM
            if (hours < 12) hours += 12;
        } else {
            // currently AM
            if (hours == 12) hours = 0;
        }
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        hours = (buffer_read[0] & 0x0F) + ((buffer_read[0] & 0x30) >> 4) * 10;
    }
    printf("Hours: %d\n", hours);
    
    DS1307_clk -> getMinutes();
    buffer_read = DS1307_clk -> getBuffer();
    minutes = (buffer_read[0] & 0x0F) + ((buffer_read[0] & 0x70) >> 4) * 10;
    printf("Minutes: %d\n", minutes);
    
    DS1307_clk -> getSeconds();
    buffer_read = DS1307_clk -> getBuffer();
    seconds = (buffer_read[0] & 0x0F) + ((buffer_read[0] & 0x70) >> 4) * 10;
    printf("Seconds: %d\n", seconds);
    
    //Create structure and set time
    
    struct tm mytime;
    struct timeval systime;
    char * text_time;
    
    mytime.tm_sec = seconds;    // Seconds
    mytime.tm_min = minutes;    // Minutes
    mytime.tm_hour = hours;   // Hours
    mytime.tm_mday = day;      // Day of Month
    mytime.tm_mon = month - 1;     // Month
    mytime.tm_year = year - 1900; // Year

    systime.tv_sec = mktime(&mytime);
    systime.tv_usec = 0;
    
    settimeofday(&systime, NULL);
    system("hwclock --systohc");
    
}




