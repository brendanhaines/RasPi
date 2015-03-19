/*
* PCA9685.cpp
*/

#include "PCA9685.h"
#include "PCA9685_Addresses.h"
;
#include <iostream>
#include "wiringPi.h"
#include "wiringPiI2C.h"

PCA9685::PCA9685()
{}

PCA9685::PCA9685( int devAddr )
{
    std::cout << "[ .... ] Getting I2C file handle..." << std::flush;
    i2cFileHandle = wiringPiI2CSetup( devAddr );  // parameter is dependent on board revision
    if( i2cFileHandle < 0 )
    {
        std::cout << "\r[ FAIL ]" << std::endl;
    }

    std::cout << " Setting all pwm to 0..." << std::flush;
    setAllPwm( 0, 0 );
    std::cout << "\r[ DONE ] " << std::endl;


    std::cout << "[ .... ] Setting up PCA9685..." << std::flush;
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE2, PCA9685_OUTDRV );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE1, PCA9685_ALLCALL );
    delay( 5 );

    int mode1 = wiringPiI2CReadReg8( i2cFileHandle, PCA9685_MODE1 );
    mode1 = mode1 & ~PCA9685_SLEEP; // set sleep bit (to wake)
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE1, mode1 );
    delay( 5 );
    std::cout << "\r[ DONE ]" << std::endl;
}

void PCA9685::setFrequency( int hertz )
{
    double preScale;
    int oldMode, newMode;

    preScale = 25000000; // 25Mhz
    preScale /= 4096;   // 12 bit
    preScale /= hertz;
    preScale -= 1;  // fencepost

    std::cout << "[ .... ] setting pwm frequency to: " << hertz << "..." << std::flush;
    std::cout << " estimate preScale: " << preScale << std::flush;
    preScale += 0.5;
    std::cout << " final preScale: " << (int) preScale << std::flush;

    oldMode = wiringPiI2CReadReg8( i2cFileHandle, PCA9685_MODE1 );
    newMode = ( oldMode & 0x7F ) | PCA9685_SLEEP;   // set sleep bit (to sleep)
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE1, newMode );  // go to sleep
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_PRE_SCALE, (int) preScale );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE1, oldMode );  // wake back up
    delay( 5 );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE1, oldMode | PCA9685_RESTART );   // set restart bit
    delay( 5 ); // wait for restart
    std::cout << "\r[ DONE ]" << std::endl;
}

void PCA9685::setPwm( int channel, int on, int off )
{
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED0_ON_L + 4*channel, on & 0xFF );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED0_ON_H + 4*channel, on >> 8 );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED0_OFF_L + 4*channel, off & 0xFF );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED0_OFF_H + 4*channel, off >> 8 );
}

void PCA9685::setAllPwm( int on, int off )
{
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_ALL_LED_ON_L, on & 0xFF );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_ALL_LED_ON_H, on >> 8 );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_ALL_LED_OFF_L, on & 0xFF );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_ALL_LED_OFF_H, on >> 8);

}







