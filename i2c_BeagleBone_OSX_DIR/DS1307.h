// I2Cdev library collection - DS1307 I2C device class header file
// Based on Maxim DS1307 datasheet, 2008
// 11/13/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
// 2011-11-13 - initial release

/* ============================================
 I2Cdev device library code is placed under the MIT license
 Copyright (c) 2011 Jeff Rowberg
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ===============================================
 */

#ifndef _DS1307_H_
#define _DS1307_H_

#include "i2CDev.h"

// comment this out if you already have a JeeLabs' DateTime class in your code
// or if you don't need DateTime functionality
#define DS1307_INCLUDE_DATETIME_CLASS

// comment this out if you don't need DateTime functionality
#define DS1307_INCLUDE_DATETIME_METHODS

#define DS1307_ADDRESS 0x68 // this device only has one address
#define DS1307_DEFAULT_ADDRESS 0x68

#define DS1307_RA_SECONDS 0x00
#define DS1307_RA_MINUTES 0x01
#define DS1307_RA_HOURS 0x02
#define DS1307_RA_DAY 0x03
#define DS1307_RA_DATE 0x04
#define DS1307_RA_MONTH 0x05
#define DS1307_RA_YEAR 0x06
#define DS1307_RA_CONTROL 0x07
#define DS1307_RA_RAM 0x08

#define DS1307_SECONDS_CH_BIT 7
#define DS1307_SECONDS_10_BIT 6
#define DS1307_SECONDS_10_LENGTH 3
#define DS1307_SECONDS_1_BIT 3
#define DS1307_SECONDS_1_LENGTH 4

#define DS1307_MINUTES_10_BIT 6
#define DS1307_MINUTES_10_LENGTH 3
#define DS1307_MINUTES_1_BIT 3
#define DS1307_MINUTES_1_LENGTH 4

#define DS1307_HOURS_MODE_BIT 6 // 0 = 24-hour mode, 1 = 12-hour mode
#define DS1307_HOURS_AMPM_BIT 5 // 2nd HOURS_10 bit if in 24-hour mode
#define DS1307_HOURS_10_BIT 4
#define DS1307_HOURS_1_BIT 3
#define DS1307_HOURS_1_LENGTH 4

#define DS1307_DAY_BIT 2
#define DS1307_DAY_LENGTH 3

#define DS1307_DATE_10_BIT 5
#define DS1307_DATE_10_LENGTH 2
#define DS1307_DATE_1_BIT 3
#define DS1307_DATE_1_LENGTH 4

#define DS1307_MONTH_10_BIT 4
#define DS1307_MONTH_1_BIT 3
#define DS1307_MONTH_1_LENGTH 4

#define DS1307_YEAR_10H_BIT 7
#define DS1307_YEAR_10H_LENGTH 4
#define DS1307_YEAR_1H_BIT 3
#define DS1307_YEAR_1H_LENGTH 4

#define DS1307_CONTROL_OUT_BIT 7
#define DS1307_CONTROL_SQWE_BIT 4
#define DS1307_CONTROL_RS_BIT 1
#define DS1307_CONTROL_RS_LENGTH 2

#define DS1307_SQW_RATE_1 0x0
#define DS1307_SQW_RATE_4096 0x1
#define DS1307_SQW_RATE_8192 0x2
#define DS1307_SQW_RATE_32768 0x3

#ifdef DS1307_INCLUDE_DATETIME_CLASS
// DateTime class courtesy of public domain JeeLabs code
// simple general-purpose date/time class (no TZ / DST / leap second handling!)
class DateTime {
public:
    DateTime (unsigned long int t=0);
    DateTime (unsigned int year, unsigned char month, unsigned char day, unsigned char hour=0, unsigned char min=0, unsigned char sec=0);
    DateTime (const char* date, const char* time);
    unsigned int year() const { return 2000 + yOff; }
    unsigned char month() const { return m; }
    unsigned char day() const { return d; }
    unsigned char hour() const { return hh; }
    unsigned char minute() const { return mm; }
    unsigned char second() const { return ss; }
    unsigned char dayOfWeek() const;
    
    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;
    // 32-bit times as seconds since 1/1/1970
    unsigned long int unixtime(void) const;
    
protected:
    unsigned char yOff, m, d, hh, mm, ss;
};
#endif

class DS1307 {
public:
    DS1307();
    DS1307(unsigned char address);
    
    void initialize();
    bool testConnection();
    
    // SECONDS register
    bool getClockRunning();
    void setClockRunning(bool running);
    unsigned char getSeconds(); // 0-59
    void setSeconds(unsigned char seconds);
    
    // MINUTES register
    unsigned char getMinutes(); // 0-59
    void setMinutes(unsigned char minutes);
    
    // HOURS register
    unsigned char getMode(); // 0-1
    void setMode(unsigned char mode);
    unsigned char getAMPM(); // 0-1
    void setAMPM(unsigned char ampm);
    unsigned char getHours12(); // 1-12
    void setHours12(unsigned char hours, unsigned char ampm);
    unsigned char getHours24(); // 0-23
    void setHours24(unsigned char hours);
    
    // DAY register
    unsigned char getDayOfWeek(); // 1-7
    void setDayOfWeek(unsigned char dow);
    
    // DATE register
    unsigned char getDay(); // 1-31
    void setDay(unsigned char day);
    
    // MONTH register
    unsigned char getMonth(); // 1-12
    void setMonth(unsigned char month);
    
    // YEAR register
    unsigned int getYear(); // 1970, 2000, 2011, etc
    void setYear(unsigned int year);
    
    // CONTROL register
    bool getFixedOutputLevel();
    void setFixedOutputLevel(bool level);
    bool getSquareWaveEnabled();
    void setSquareWaveEnabled(bool enabled);
    unsigned char getSquareWaveRate();
    void setSquareWaveRate(unsigned char rate);
    
    // RAM registers
    unsigned char getMemoryByte(unsigned char offset);
    void setMemoryByte(unsigned char offset, unsigned char value);
    
    // convenience methods
    
    void getDate(unsigned int *year, unsigned char *month, unsigned char *day);
    void setDate(unsigned int year, unsigned char month, unsigned char day);
    
    void getTime12(unsigned char *hours, unsigned char *minutes, unsigned char *seconds, unsigned char *ampm);
    void setTime12(unsigned char hours, unsigned char minutes, unsigned char seconds, unsigned char ampm);
    
    void getTime24(unsigned char *hours, unsigned char *minutes, unsigned char *seconds);
    void setTime24(unsigned char hours, unsigned char minutes, unsigned char seconds);
    
    void getDateTime12(unsigned int *year, unsigned char *month, unsigned char *day, unsigned char *hours, unsigned char *minutes, unsigned char *seconds, unsigned char *ampm);
    void setDateTime12(unsigned int year, unsigned char month, unsigned char day, unsigned char hours, unsigned char minutes, unsigned char seconds, unsigned char ampm);
    
    void getDateTime24(unsigned int *year, unsigned char *month, unsigned char *day, unsigned char *hours, unsigned char *minutes, unsigned char *seconds);
    void setDateTime24(unsigned int year, unsigned char month, unsigned char day, unsigned char hours, unsigned char minutes, unsigned char seconds);
    
#ifdef DS1307_INCLUDE_DATETIME_METHODS
    DateTime getDateTime();
    void setDateTime(DateTime dt);
#endif
    
    unsigned char* getBuffer();
    
private:
    unsigned char devAddr;
    unsigned char buffer[1];
    bool mode12;
    bool clockHalt;
};

#endif /* _DS1307_H_ */