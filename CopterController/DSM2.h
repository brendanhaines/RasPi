#ifndef DSM2_H
#define DSM2_H

class DSM2
{
public:
    DSM2();

    void update();
    int values[6];
private:
    int fd;
    int valueMask;
    char thisByte, lastByte;
    int data[8];
    char raw[16];
};

#endif