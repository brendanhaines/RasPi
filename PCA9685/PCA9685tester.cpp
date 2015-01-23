
#include "stdio.h"
#include "wiringPi.h"
#include "PCA9685.h"

int main( int argc, const char* argv[] )
{
	wiringPiSetup();
	PCA9685 servo = PCA9685( 0x40 );
	servo.setFrequency( 60 );
	printf( "freq set\n" );

	while( true )
	{
		servo.setPwm( 0, 0, 600 );
		printf( "pwm: 600\n" );
		delay( 1000 );
		servo.setPwm( 0, 0, 150 );
		printf("pwm: 150\n" );
		delay( 1000 );
	}
}
