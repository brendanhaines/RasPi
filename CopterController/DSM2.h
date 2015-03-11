#ifndef DSM2_H
#define DSM2_H

class DSM2
{
public:
    DSM2( int mode = 1024 );

    void update();
    int values[7];
private:
    int fd;
    int valueMask;
    char thisByte, lastByte;
    int data[7];
};

#endif