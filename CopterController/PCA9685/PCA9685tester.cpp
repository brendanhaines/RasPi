
#include <iostream>
#include "stdlib.h"
#include "wiringPi.h"
#include "PCA9685.h"

#define MOTOR_MIN 200
#define MOTOR_MAX 600
#define SERVO_FREQ 60

int main( int argc, const char* argv[] )
{
    std::cout << "will only test motor 0" << std::endl;
    int currentValue = MOTOR_MIN;

    wiringPiSetup();
    PCA9685 servo = PCA9685( 0x40 );
    servo.setFrequency( SERVO_FREQ );
    std::cout << "freq set: " << SERVO_FREQ << std::endl;

    if( argc > 1 )
    {
        if( atoi(argv[1]) == 1 )
        {
            std::cout << "will set max pwm" << std::endl;
            servo.setPwm( 0, 0, MOTOR_MAX );
            std::cout << "pwm set " << MOTOR_MAX << std::endl;
            std::cout << "press ENTER to continue" << std::endl;
            while( std::cin.get() != '\n' );
        }
    }

    servo.setPwm( 0, 0, MOTOR_MIN );
    std::cout << "pwm set " << MOTOR_MIN << std::endl;
    std::cout << "press ENTER to continue" << std::endl;
    while( std::cin.get() != '\n' );

    while( true )
    {
        int keyPressed = std::cin.get();
        if( keyPressed == ' ' )
        {
            std::cout << "KILL MOTOR" << std::endl;
            currentValue = MOTOR_MIN;
            servo.setPwm( 0, 0, currentValue );
            std::cout << "pwm: " << currentValue << std::endl;
        }
        else if( keyPressed == 'w' )  // up arrow
        {
            std::cout << "up pressed" << std::endl;
            currentValue += 5;
            if( currentValue > MOTOR_MAX ) currentValue = MOTOR_MAX;
            servo.setPwm( 0, 0, currentValue );
            std::cout << "pwm: " << currentValue << std::endl;
        }
        else if( keyPressed == 's' ) // down arrow
        {
            std::cout << "down pressed" << std::endl;
            currentValue -= 5;
            if( currentValue < MOTOR_MIN ) currentValue = MOTOR_MIN;
            servo.setPwm( 0, 0, currentValue );
            std::cout << "pwm: " << currentValue << std::endl;
        }

        /*
        servo.setPwm( 0, 0, MOTOR_MAX );
        std::cout << "pwm: " << MOTOR_MAX << std::endl;
        delay( 1000 );
        servo.setPwm( 0, 0, MOTOR_MIN );
        std::cout << "pwm: " << MOTOR_MIN << std::endl;
        delay( 1000 );
        */
    }
}
