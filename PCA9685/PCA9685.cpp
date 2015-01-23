/*
* PCA9685.cpp
*/

#include "PCA9685.h"

#include "stdio.h"
#include "PCA9685_Addresses.h"
#include "wiringPi.h"
#include "wiringPiI2C.h"

PCA9685::PCA9685()
{
    PCA9685( PCA9685_DEFAULT_ADDRESS );
}

PCA9685::PCA9685( int devAddr )
{
    i2cFileHandle = wiringPiI2CSetup( devAddr );  // parameter is dependent on board revision
    if( i2cFileHandle < 0 )
    {
        printf( "failed to open i2c\n" );
    }

    setAllPwm( 0, 0 );

    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE2, PCA9685_OUTDRV );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE1, PCA9685_ALLCALL );
    delay( 5 );

    int mode1 = wiringPiI2CReadReg8( i2cFileHandle, PCA9685_MODE1 );
    mode1 = mode1 & ~PCA9685_SLEEP;
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE1, mode1 );

   /* wiringPiI2CWriteReg8( i2cFileHandle,
                          PCA9685_MODE1,
                          wiringPiI2CReadReg8( i2cFileHandle, PCA9685_MODE1 ) & ~PCA9685_SLEEP );
    */delay( 5 );
}

void PCA9685::setFrequency( int hertz )
{
    double preScale;
    int oldMode, newMode;

    preScale = 25000000; // 25Mhz
    preScale /= 4096;   // 12 bit
    preScale /= hertz;
    preScale -= 1;  // fencepost
    printf( "preScale: %f\n", preScale );

    oldMode = wiringPiI2CReadReg8( i2cFileHandle, PCA9685_MODE1 );
    newMode = ( oldMode & 0x7F ) | 0x10;
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE1, newMode );  // go to sleep
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_PRE_SCALE, (int) preScale );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE1, oldMode );  // wake back up
    delay( 5 );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_MODE1, oldMode | 0x80 );
}

void PCA9685::setPwm( int channel, int on, int off )
{
    //wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED_ADDRS[ 4 * channel ], on & 0xFF );
    //wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED_ADDRS[ 4 * channel +1 ], on >> 8 );
    //wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED_ADDRS[ 4 * channel +2 ], off & 0xFF );
    //wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED_ADDRS[ 4 * channel +3 ], off >> 8 );

    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED0_ON_L + 4*channel, on & 0xFF );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED0_ON_H + 4*channel, on >> 8 );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED0_OFF_L + 4*channel, off & 0xFF );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_LED0_OFF_H + 4*channel, off >> 8 );
}

void PCA9685::setAllPwm( int on, int off )
{
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_ALL_LED_ON_L, on & 0xFF );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_ALL_LED_OFF_H, on >> 8 );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_ALL_LED_ON_L, on & 0xFF );
    wiringPiI2CWriteReg8( i2cFileHandle, PCA9685_ALL_LED_OFF_H, on >> 8);

}







