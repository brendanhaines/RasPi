#include "DSM2.h"

#include <iostream>
#include <cstdio>
#include "wiringSerial.h"

#define VALUE_MASK 0x3FF

DSM2::DSM2()
{
    std::cout << "[ .... ] opening serial port..." << std::flush;
    fd = serialOpen( "/dev/ttyAMA0", 115200 );
    if( fd < 0 )
    {
        std::cout << "\r[ FAIL ]" << std::endl;
    }
    else
    {
        std::cout << "\r[ DONE ]" << fd << std::endl;
    }
    std::cout << "[ .... ] flushing serial..." << std::flush;
    serialFlush(fd);
    std::cout << "\r[ DONE ]" << std::endl;
}

void DSM2::update()
{
    int i = 0;

    if( serialDataAvail(fd) < 16 ) return;


    while( serialDataAvail(fd) > 16 )
        read( fd, raw, 16 );

    //while( serialDataAvail(fd) > 32 )
        //for( i = 0; i < 16; i++ )
            //read(fd);

    //for( i = 0; i < 8; i++ )
        //data[ i ] = ( (int)(serialGetchar(fd)) << 8 ) + serialGetchar(fd);

    for( i = 0; i < 8; i++ )
        data[i] = ( ((int)raw[2*i]) << 8 ) + raw[ 2*i+1 ];

    //printf( "%04X %04X %04X %04X %04X %04X %04X %04X\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] );

    *(values + 0) = data[7] & VALUE_MASK; //Throttle
    *(values + 1) = data[1] & VALUE_MASK; //Aileron
    *(values + 2) = data[3] & VALUE_MASK; //Elevator
    *(values + 3) = data[5] & VALUE_MASK; //Rudder
    *(values + 4) = data[4] & VALUE_MASK; //Gear
    *(values + 5) = data[2] & VALUE_MASK; //Aux1

    //printf( "%04X %04X %04X %04X %04X %04X\n", values[0], values[1], values[2], values[3], values[4], values[5] );
    return;
}
