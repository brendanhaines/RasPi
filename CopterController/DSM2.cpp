#include "DSM2.h"

#include <iostream>
#include "wiringSerial.h"

#define VALUE_MASK 0x3FF

DSM2::DSM2( int mode )
{
    std::cout << "opening serial port..." << std::flush;
    fd = serialOpen( "/dev/ttyAMA0", 115200 );
    std::cout << "SUCCESS " << fd << std::endl;
    std::cout << "flushing serial..." << std::flush;
    serialFlush(fd);
    std::cout << "SUCCESS" << std::endl;
}

void DSM2::update()
{
    int i = 0;

    if( serialDataAvail(fd) < 16 ) return;

    for( i = 0; i < 8; i++ )
        data[ i ] = ( (int)(serialGetchar(fd)) << 8 ) + serialGetchar(fd);

    *(values + 0) = data[7] & VALUE_MASK; //Throttle
    *(values + 1) = data[1] & VALUE_MASK; //Aileron
    *(values + 2) = data[3] & VALUE_MASK; //Elevator
    *(values + 3) = data[5] & VALUE_MASK; //Rudder
    *(values + 4) = data[4] & VALUE_MASK; //Gear
    *(values + 5) = data[2] & VALUE_MASK; //Aux1
    return;
}
