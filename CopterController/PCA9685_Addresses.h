#ifndef PCA9685_ADDRESSES_H
#define PCA9685_ADDRESSES_H

#define PCA9685_DEFAULT_ADDRESS 0x40


#define PCA9685_RESTART 0x80
#define PCA9685_SLEEP   0x10
#define PCA9685_ALLCALL 0x01
#define PCA9685_INVRT   0x10
#define PCA9685_OUTDRV  0x04


#define PCA9685_MODE1 0x00
#define PCA9685_MODE2 0x01
#define PCA9685_SUBADR1 0x02
#define PCA9685_SUBADR2 0x03
#define PCA9685_SUBADR3 0x04
#define PCA9685_ALLCALLADR 0x05

#define PCA9685_PRE_SCALE   0xFE
#define PCA9685_TESTMODE    0xFF

const int PCA9685_LED_ADDRS[] = {
 // ON_L  ON_H  OFF_L OFF_H // channel
    0x06, 0x07, 0x08, 0x09, // 0
    0x0A, 0x0B, 0x0C, 0x0D, // 1
    0x0E, 0x0F, 0x10, 0x11, // 2
    0x12, 0x13, 0x14, 0x15, // 3
    0x16, 0x17, 0x18, 0x19, // 4
    0x1A, 0x1B, 0x1C, 0x1D, // 5
    0x1E, 0x1F, 0x20, 0x21, // 6
    0x22, 0x23, 0x24, 0x25, // 7
    0x26, 0x27, 0x28, 0x29, // 8
    0x2A, 0x2B, 0x2C, 0x2D, // 9
    0x2E, 0x2F, 0x30, 0x31, // 10
    0x32, 0x33, 0x34, 0x35, // 11
    0x36, 0x37, 0x38, 0x39, // 12
    0x3A, 0x3B, 0x3C, 0x3D, // 13
    0x3E, 0x3F, 0x40, 0x41, // 14
    0x42, 0x43, 0x44, 0x45 };// 15

#define PCA9685_ALL_LED_ON_L    0xFA
#define PCA9685_ALL_LED_ON_H    0xFB
#define PCA9685_ALL_LED_OFF_L   0xFC
#define PCA9685_ALL_LED_OFF_H   0xFD

#define PCA9685_LED0_ON_L   0x06
#define PCA9685_LED0_ON_H   0x07
#define PCA9685_LED0_OFF_L  0x08
#define PCA9685_LED0_OFF_H  0x09

#define PCA9685_LED1_ON_L   0x0A
#define PCA9685_LED1_ON_H   0x0B
#define PCA9685_LED1_OFF_L  0x0C
#define PCA9685_LED1_OFF_H  0x0D

#define PCA9685_LED2_ON_L   0x0E
#define PCA9685_LED2_ON_H   0x0F
#define PCA9685_LED2_OFF_L  0x10
#define PCA9685_LED2_OFF_H  0x11

#define PCA9685_LED3_ON_L   0x12
#define PCA9685_LED3_ON_H   0x13
#define PCA9685_LED3_OFF_L  0x14
#define PCA9685_LED3_OFF_H  0x15

#define PCA9685_LED4_ON_L   0x16
#define PCA9685_LED4_ON_H   0x17
#define PCA9685_LED4_OFF_L  0x18
#define PCA9685_LED4_OFF_H  0x19

#define PCA9685_LED5_ON_L   0x1A
#define PCA9685_LED5_ON_H   0x1B
#define PCA9685_LED5_OFF_L  0x1C
#define PCA9685_LED5_OFF_H  0x1D

#define PCA9685_LED6_ON_L   0x1E
#define PCA9685_LED6_ON_H   0x1F
#define PCA9685_LED6_OFF_L  0x20
#define PCA9685_LED6_OFF_H  0x21

#define PCA9685_LED7_ON_L   0x22
#define PCA9685_LED7_ON_H   0x23
#define PCA9685_LED7_OFF_L  0x24
#define PCA9685_LED7_OFF_H  0x25

#define PCA9685_LED8_ON_L   0x26
#define PCA9685_LED8_ON_H   0x27
#define PCA9685_LED8_OFF_L  0x28
#define PCA9685_LED8_OFF_H  0x29

#define PCA9685_LED9_ON_L   0x2A
#define PCA9685_LED9_ON_H   0x2B
#define PCA9685_LED9_OFF_L  0x2C
#define PCA9685_LED9_OFF_H  0x2D

#define PCA9685_LED10_ON_L  0x2E
#define PCA9685_LED10_ON_H  0x2F
#define PCA9685_LED10_OFF_L 0x30
#define PCA9685_LED10_OFF_H 0x31

#define PCA9685_LED11_ON_L  0x32
#define PCA9685_LED11_ON_H  0x33
#define PCA9685_LED11_OFF_L 0x34
#define PCA9685_LED11_OFF_H 0x35

#define PCA9685_LED12_ON_L  0x36
#define PCA9685_LED12_ON_H  0x37
#define PCA9685_LED12_OFF_L 0x38
#define PCA9685_LED12_OFF_H 0x39

#define PCA9685_LED13_ON_L  0x3A
#define PCA9685_LED13_ON_H  0x3B
#define PCA9685_LED13_OFF_L 0x3C
#define PCA9685_LED13_OFF_H 0x3D

#define PCA9685_LED14_ON_L  0x3E
#define PCA9685_LED14_ON_H  0x3F
#define PCA9685_LED14_OFF_L 0x40
#define PCA9685_LED14_OFF_H 0x41

#define PCA9685_LED15_ON_L  0x42
#define PCA9685_LED15_ON_H  0x43
#define PCA9685_LED15_OFF_L 0x44
#define PCA9685_LED15_OFF_H 0x45

#endif