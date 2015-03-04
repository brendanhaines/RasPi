#include "DSM2.h"

#include "wiringSerial.h"
#include <cstring>


DSM2::DSM2( char* device, int mode )
{

    fd = serialOpen( device, 115200 );
    if( mode != 1024 && mode != 2048 ) mode = 1024;
    if( mode == 1024 )
    {
        valueSize = 10;
        valueMask = 0x3FF;

    }
    else
    {
        valueSize = 11;
        valueMask = 0x7FF;
    }
}

bool DSM2::ready()
{
    if( serialDataAvail( fd ) >= 16 ) return true;
    return false;
}

void DSM2::update( bool block )
{
    int i = 0;
    if( block )
    {
        while( !ready() );
    }
    else if( !ready() ) return;

    for( i = 0; i < 8; i++ ) readNext();
    return;
}

void DSM2::readNext()
{
    int channel, value, raw;
    raw = ( serialGetchar( fd ) << 8 ) + serialGetchar( fd );

    if( lastReadChan == chanBeforeFl )
    {
        frameLoss = raw << 1 >> 1;
        lastReadChan = -1;
        return;
    }

    channel = (raw & 0x7FFF) >> valueSize;
    value = raw & valueMask;
    values[ channel ] = value;

    lastReadChan = channel;

    return;
}
