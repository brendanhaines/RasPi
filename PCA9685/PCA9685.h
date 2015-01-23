#ifndef PCA9685_H
#define PCA9685_H

class PCA9685
{
public:
    PCA9685();  // no-args constructor: uses default device address @ 50hz
    PCA9685( int devAddr ); // frequency: 50hz

    void setFrequency( int hertz );
    void setPwm( int channel, int on, int off );   // for standard servo behavior: 150 <= value <= 600
    void setAllPwm( int on, int off );
private:
    int i2cFileHandle;
};

#endif
