
#include <iostream>
#include "stdio.h"
#include "wiringPi.h"
#include "PCA9685.h"

int main( int argc, const char* argv[] )
{
    wiringPiSetup();
    PCA9685 servo = PCA9685( 0x40 );
    servo.setFrequency( 60 );
    std::cout << "freq set" << std::endl;

    servo.setPwm( 0, 0, 600 );
    std::cout << "pwm set 600" << std::endl;
    while( std::cin.get() != '\n' );

    servo.setPwm( 0, 0, 150 );
    std::cout << "pwm set 150" << std::endl;
    while( std::cin.get() != '\n' );

    while( true )
    {
        servo.setPwm( 0, 0, 600 );
        std::cout << "pwm: 600" << std::endl;
        delay( 1000 );
        servo.setPwm( 0, 0, 150 );
        std::cout << "pwm: 150" << std::endl;
        delay( 1000 );
    }
}
