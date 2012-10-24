// I2Cdev library collection - DS1307 I2C device class
// Based on Maxim DS1307 datasheet, 2008
// 11/13/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
// 2011-11-13 - initial release
// 2012-03-29 - alain.spineux@gmail.com: bug in getHours24()
// am/pm is bit 0x20 instead of 0x80
//

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


#include </usr/include/linux/i2c-dev.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "DS1307.h"
#include "i2CDev.h"



i2Cdev *DS1307_time = new i2Cdev();



/** Default constructor, uses default I2C address.
 * @see DS1307_DEFAULT_ADDRESS
 */
DS1307::DS1307() {
    devAddr = DS1307_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see DS1307_DEFAULT_ADDRESS
 * @see DS1307_ADDRESS
 */
DS1307::DS1307(unsigned char address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * This method reads the current 12/24-hour mode setting into the locally stored
 * variable so that it isn't accidentally changed by the setHour*() methods.
 */
void DS1307::initialize() {
    getMode(); // automatically sets internal "mode12" member variable
    getClockRunning(); // automatically sets internal "clockHalt" member variable
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool DS1307::testConnection() {
    if (DS1307_time -> readByte(devAddr, DS1307_RA_SECONDS, buffer) == 1) {
        printf("Connection Passed!\n");
        return true;
    }
    printf("Connection Failed!\n");
    return false;
}

// SECONDS register
bool DS1307::getClockRunning() {
    DS1307_time -> readBit(devAddr, DS1307_RA_SECONDS, DS1307_SECONDS_CH_BIT, buffer);
    clockHalt = buffer[0];
    return !buffer[0];
}
void DS1307::setClockRunning(bool running) {
    DS1307_time -> writeBit(devAddr, DS1307_RA_SECONDS, DS1307_SECONDS_CH_BIT, !running);
}
unsigned char DS1307::getSeconds() {
    // Byte: [7 = CH] [6:4 = 10SEC] [3:0 = 1SEC]
    DS1307_time -> readByte(devAddr, DS1307_RA_SECONDS, buffer);
    clockHalt = buffer[0] & 0x80;
    //printf("DS1307 - Sec: %X", buffer[0]);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x70) >> 4) * 10;
}
void DS1307::setSeconds(unsigned char seconds) {
    if (seconds > 59) return;
    unsigned char value = (clockHalt ? 0x80 : 0x00) + ((seconds / 10) << 4) + (seconds % 10);
    DS1307_time -> writeByte(devAddr, DS1307_RA_SECONDS, value);
}

// MINUTES register
unsigned char DS1307::getMinutes() {
    // Byte: [7 = 0] [6:4 = 10MIN] [3:0 = 1MIN]
    DS1307_time -> readByte(devAddr, DS1307_RA_MINUTES, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x70) >> 4) * 10;
}
void DS1307::setMinutes(unsigned char minutes) {
    if (minutes > 59) return;
    unsigned char value = ((minutes / 10) << 4) + (minutes % 10);
    DS1307_time -> writeByte(devAddr, DS1307_RA_MINUTES, value);
}

// HOURS register
unsigned char DS1307::getMode() {
    DS1307_time -> readBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_MODE_BIT, buffer);
    mode12 = buffer[0];
    return buffer[0];
}
void DS1307::setMode(unsigned char mode) {
    DS1307_time -> writeBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_MODE_BIT, mode);
}
unsigned char DS1307::getAMPM() {
    DS1307_time -> readBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_AMPM_BIT, buffer);
    return buffer[0];
}
void DS1307::setAMPM(unsigned char ampm) {
    DS1307_time -> writeBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_AMPM_BIT, ampm);
}
unsigned char DS1307::getHours12() {
    DS1307_time -> readByte(devAddr, DS1307_RA_HOURS, buffer);
    mode12 = buffer[0] & 0x40;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        return (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        unsigned char hours = (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;
        
        // convert 24-hour to 12-hour format, since that's what's requested
        if (hours > 12) hours -= 12;
        else if (hours == 0) hours = 12;
        return hours;
    }
}
void DS1307::setHours12(unsigned char hours, unsigned char ampm) {
    if (hours > 12 || hours < 1) return;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        if (ampm > 0) ampm = 0x20;
        unsigned char value = 0x40 + ampm + ((hours / 10) << 4) + (hours % 10);
        DS1307_time -> writeByte(devAddr, DS1307_RA_HOURS, value);
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        if (ampm > 0) hours += 12;
        if (hours == 0) hours = 12; // 12 AM
        else if (hours == 24) hours = 12; // 12 PM, after +12 adjustment
        unsigned char value = ((hours / 10) << 4) + (hours % 10);
        DS1307_time -> writeByte(devAddr, DS1307_RA_HOURS, value);
    }
}
unsigned char DS1307::getHours24() {
    DS1307_time -> readByte(devAddr, DS1307_RA_HOURS, buffer);
    mode12 = buffer[0] & 0x40;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        unsigned char hours = (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;
        
        // convert 12-hour to 24-hour format, since that's what's requested
        if (buffer[0] & 0x20) {
            // currently PM
            if (hours < 12) hours += 12;
        } else {
            // currently AM
            if (hours == 12) hours = 0;
        }
        return hours;
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        return (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;
    }
}
void DS1307::setHours24(unsigned char hours) {
    if (hours > 23) return;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        unsigned char ampm = 0;
        if (hours > 11) ampm = 0x20;
        if (hours > 12) hours -= 12;
        else if (hours == 0) hours = 12;
        unsigned char value = 0x40 + ampm + ((hours / 10) << 4) + (hours % 10);
        DS1307_time -> writeByte(devAddr, DS1307_RA_HOURS, value);
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        unsigned char value = ((hours / 10) << 4) + (hours % 10);
        DS1307_time -> writeByte(devAddr, DS1307_RA_HOURS, value);
    }
}

// DAY register
unsigned char DS1307::getDayOfWeek() {
    DS1307_time -> readBits(devAddr, DS1307_RA_DAY, DS1307_DAY_BIT, DS1307_DAY_LENGTH, buffer);
    return buffer[0];
}
void DS1307::setDayOfWeek(unsigned char dow) {
    if (dow < 1 || dow > 7) return;
    DS1307_time -> writeBits(devAddr, DS1307_RA_DAY, DS1307_DAY_BIT, DS1307_DAY_LENGTH, dow);
}

// DATE register
unsigned char DS1307::getDay() {
    // Byte: [7:6 = 0] [5:4 = 10DAY] [3:0 = 1DAY]
    DS1307_time -> readByte(devAddr, DS1307_RA_DATE, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;
}
void DS1307::setDay(unsigned char day) {
    unsigned char value = ((day / 10) << 4) + (day % 10);
    DS1307_time -> writeByte(devAddr, DS1307_RA_DATE, value);
}

// MONTH register
unsigned char DS1307::getMonth() {
    // Byte: [7:5 = 0] [4 = 10MONTH] [3:0 = 1MONTH]
    DS1307_time -> readByte(devAddr, DS1307_RA_MONTH, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;
}
void DS1307::setMonth(unsigned char month) {
    if (month < 1 || month > 12) return;
    unsigned char value = ((month / 10) << 4) + (month % 10);
    DS1307_time -> writeByte(devAddr, DS1307_RA_MONTH, value);
}

// YEAR register
unsigned int DS1307::getYear() {
    DS1307_time -> readByte(devAddr, DS1307_RA_YEAR, buffer);
    return 2000 + (buffer[0] & 0x0F) + ((buffer[0] & 0xF0) >> 4) * 10;
}
void DS1307::setYear(unsigned int year) {
    if (year < 2000) return;
    year -= 2000;
    unsigned char value = ((year / 10) << 4) + (year % 10);
    DS1307_time -> writeByte(devAddr, DS1307_RA_YEAR, value);
}

// CONTROL register
bool DS1307::getFixedOutputLevel() {
    DS1307_time -> readBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_OUT_BIT, buffer);
    return buffer[0];
}
void DS1307::setFixedOutputLevel(bool level) {
    DS1307_time -> writeBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_OUT_BIT, level);
}
bool DS1307::getSquareWaveEnabled() {
    DS1307_time -> readBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_SQWE_BIT, buffer);
    return buffer[0];
}
void DS1307::setSquareWaveEnabled(bool enabled) {
    DS1307_time -> writeBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_SQWE_BIT, enabled);
}
unsigned char DS1307::getSquareWaveRate() {
    DS1307_time -> readBits(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_RS_BIT, DS1307_CONTROL_RS_LENGTH, buffer);
    return buffer[0];
}
void DS1307::setSquareWaveRate(unsigned char rate) {
    DS1307_time -> writeBits(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_RS_BIT, DS1307_CONTROL_RS_LENGTH, rate);
}

// RAM registers
unsigned char DS1307::getMemoryByte(unsigned char offset) {
    if (offset > 55) return 0;
    DS1307_time -> readByte(devAddr, DS1307_RA_RAM + offset, buffer);
    return buffer[0];
}
void DS1307::setMemoryByte(unsigned char offset, unsigned char value) {
    if (offset > 55) return;
    DS1307_time -> writeByte(devAddr, DS1307_RA_RAM + offset, value);
}

// convenience methods

void DS1307::getDate(unsigned int *year, unsigned char *month, unsigned char *day) {
    *year = getYear();
    *month = getMonth();
    *day = getDay();
}
void DS1307::setDate(unsigned int year, unsigned char month, unsigned char day) {
    setYear(year);
    setMonth(month);
    setDay(day);
}

void DS1307::getTime12(unsigned char *hours, unsigned char *minutes, unsigned char *seconds, unsigned char *ampm) {
    *hours = getHours12();
    *minutes = getMinutes();
    *seconds = getSeconds();
    *ampm = getAMPM();
}
void DS1307::setTime12(unsigned char hours, unsigned char minutes, unsigned char seconds, unsigned char ampm) {
    // write seconds first to reset divider chain and give
    // us 1 second to write remaining time info
    setSeconds(seconds);
    setMinutes(minutes);
    setHours12(hours, ampm);
}

void DS1307::getTime24(unsigned char *hours, unsigned char *minutes, unsigned char *seconds) {
    *hours = getHours24();
    *minutes = getMinutes();
    *seconds = getSeconds();
}
void DS1307::setTime24(unsigned char hours, unsigned char minutes, unsigned char seconds) {
    // write seconds first to reset divider chain and give
    // us 1 second to write remaining time info
    setSeconds(seconds);
    setMinutes(minutes);
    setHours24(hours);
}

void DS1307::getDateTime12(unsigned int *year, unsigned char *month, unsigned char *day, unsigned char *hours, unsigned char *minutes, unsigned char *seconds, unsigned char *ampm) {
    getTime12(hours, minutes, seconds, ampm);
    getDate(year, month, day);
}
void DS1307::setDateTime12(unsigned int year, unsigned char month, unsigned char day, unsigned char hours, unsigned char minutes, unsigned char seconds, unsigned char ampm) {
    setTime12(hours, minutes, seconds, ampm);
    setDate(year, month, day);
}

void DS1307::getDateTime24(unsigned int *year, unsigned char *month, unsigned char *day, unsigned char *hours, unsigned char *minutes, unsigned char *seconds) {
    getTime24(hours, minutes, seconds);
    getDate(year, month, day);
}
void DS1307::setDateTime24(unsigned int year, unsigned char month, unsigned char day, unsigned char hours, unsigned char minutes, unsigned char seconds) {
    setTime24(hours, minutes, seconds);
    setDate(year, month, day);
}
/*
#ifdef DS1307_INCLUDE_DATETIME_METHODS
DateTime DS1307::getDateTime() {
    DateTime dt = DateTime(getYear(), getMonth(), getDay(), getHours24(), getMinutes(), getSeconds());
    return dt;
}
void DS1307::setDateTime(DateTime dt) {
    setSeconds(dt.second());
    setMinutes(dt.minute());
    setHours24(dt.hour());
    setDay(dt.day());
    setMonth(dt.month());
    setYear(dt.year());
}
#endif

#ifdef DS1307_INCLUDE_DATETIME_CLASS
// DateTime class courtesy of public domain JeeLabs code
//#include <avr/pgmspace.h>
#define SECONDS_PER_DAY 86400L
#define SECONDS_FROM_1970_TO_2000 946684800

////////////////////////////////////////////////////////////////////////////////
// utility code, some of this could be exposed in the DateTime API if needed

static unsigned char daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// number of days since 2000/01/01, valid for 2001..2099
static unsigned int date2days(unsigned int y, unsigned char m, unsigned char d) {
    if (y >= 2000)
        y -= 2000;
    unsigned int days = d;
    for (unsigned char i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(unsigned int days, unsigned char h, unsigned char m, unsigned char s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

DateTime::DateTime (unsigned long int t) {
    t -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970
    
    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    unsigned int days = t / 24;
    unsigned char leap;
    for (yOff = 0; ; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        unsigned char daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

DateTime::DateTime (unsigned int year, unsigned char month, unsigned char day, unsigned char hour, unsigned char min, unsigned char sec) {
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

static unsigned char conv2d(const char* p) {
    unsigned char v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

// A convenient constructor for using "the compiler's time":
// DateTime now (__DATE__, __TIME__);
// NOTE: using PSTR would further reduce the RAM footprint
DateTime::DateTime (const char* date, const char* time) {
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    yOff = conv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    switch (date[0]) {
        case 'J': m = date[1] == 'a' ? 1 : m = date[2] == 'n' ? 6 : 7; break;
        case 'F': m = 2; break;
        case 'A': m = date[2] == 'r' ? 4 : 8; break;
        case 'M': m = date[2] == 'r' ? 3 : 5; break;
        case 'S': m = 9; break;
        case 'O': m = 10; break;
        case 'N': m = 11; break;
        case 'D': m = 12; break;
    }
    d = conv2d(date + 4);
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);
}

unsigned char DateTime::dayOfWeek() const {
    unsigned int day = secondstime() / SECONDS_PER_DAY;
    return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

unsigned long int DateTime::unixtime(void) const {
    unsigned long int t;
    unsigned int days = date2days(yOff, m, d);
    t = time2long(days, hh, mm, ss);
    t += SECONDS_FROM_1970_TO_2000; // seconds from 1970 to 2000
    
    return t;
}
#endif
*/


unsigned char* DS1307::getBuffer() {
    return buffer;
}


