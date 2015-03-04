#ifndef DSM2_H
#define DSM2_H

#define chanBeforeFl -1

class DSM2 {
public:
    DSM2( char* device = "/dev/ttyAMA0", int mode = 1024 );
    
    bool ready();
    void update( bool block = false );
    void readNext();

    int* values;
    int frameLoss;
private:
    int lastReadChan;

    int fd; // file descriptor for serial bus
    int valueSize;  // number of bits for value part of frame
    int valueMask;
};

#endif