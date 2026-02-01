/*
  ADS1299_DR.h - Library for Texas Instruments ADS1299 EEG ADC.
  Based on datasheet SBAS499C.
*/

#ifndef ADS1299_DR_H
#define ADS1299_DR_H

#include <Arduino.h>
#include <SPI.h>

// SPI Command Definitions (Datasheet Table 10)
#define _WAKEUP   0x02
#define _STANDBY  0x04
#define _RESET    0x06
#define _START    0x08
#define _STOP     0x0A
#define _RDATAC   0x10 // Read Data Continuous
#define _SDATAC   0x11 // Stop Read Data Continuous
#define _RDATA    0x12 // Read Data by command
#define _RREG     0x20 // Read Register (needs address added)
#define _WREG     0x40 // Write Register (needs address added)

// Register Addresses (Datasheet Table 11)
#define ADS_ID          0x00
#define ADS_CONFIG1     0x01
#define ADS_CONFIG2     0x02
#define ADS_CONFIG3     0x03
#define ADS_LOFF        0x04
#define ADS_CH1SET      0x05
#define ADS_CH2SET      0x06
#define ADS_CH3SET      0x07
#define ADS_CH4SET      0x08
#define ADS_CH5SET      0x09
#define ADS_CH6SET      0x0A
#define ADS_CH7SET      0x0B
#define ADS_CH8SET      0x0C
#define ADS_BIAS_SENSP  0x0D
#define ADS_BIAS_SENSN  0x0E
#define ADS_LOFF_SENSP  0x0F
#define ADS_LOFF_SENSN  0x10
#define ADS_LOFF_FLIP   0x11
#define ADS_LOFF_STATP  0x12
#define ADS_LOFF_STATN  0x13
#define ADS_GPIO        0x14
#define ADS_MISC1       0x15
#define ADS_MISC2       0x16
#define ADS_CONFIG4     0x17

class ADS1299_DR {
  public:
    // Constructor
    ADS1299_DR(int csPin, int drdyPin, int resetPin);
    
    // Initialization
    void begin();
    void resetHardware();
    
    // SPI Command Wrappers
    void start();   
    void stop();    
    void rdatac();  
    void sdatac();  
    
    // Register Read/Write
    byte readRegister(byte address); 
    void writeRegister(byte address, byte value); 
    
    // Data Retrieval
    bool updateData(); 
    
    // Accessors
    long channelData[2]; // Stores 24-bit signed values for 8 channels
    byte statusData[3];  // Stores the 24-bit status word

  private:
    int _csPin;
    int _drdyPin;
    int _resetPin;
    void transferCommand(byte cmd);
};

#endif
