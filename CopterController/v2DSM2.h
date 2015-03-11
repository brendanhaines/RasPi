#ifndef V2DSM2_H
#define V2DSM2_H

class v2DSM2
{
public:
    v2DSM2( int mode = 1024 );

    void update();
    int values[7];
private:
    int fd;
    int valueMask;
    char thisByte, lastByte;
    int data[7];
};

#endif