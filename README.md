# ADS1299_DR Arduino Library

A robust Arduino library for the **Texas Instruments ADS1299**, a low-noise, 24-bit simultaneous-sampling Analog-to-Digital Converter (ADC) designed for EEG and biopotential measurements.

## Supported Devices
*   **ADS1299**: 8-Channel EEG ADC [2]
*   **ADS1299-6**: 6-Channel EEG ADC [2]
*   **ADS1299-4**: 4-Channel EEG ADC [2]

## Features
*   **SPI Protocol**: Implements SPI Mode 1 (CPOL=0, CPHA=1) as required by the ADS1299 timing specifications [4].
*   **Data Conversion**: Automatically handles 24-bit two's complement conversion to signed 32-bit integers [5].
*   **Power-Up Sequence**: Handles the specific reset timing (tPOR) and `SDATAC` command sequence required to initialize digital registers [6].
*   **Test Signals**: Easy configuration for internal square-wave test signals [7].

## Hardware Connection
**Warning:** The ADS1299 digital I/O operates at **1.8V to 3.6V** [8]. Do not connect directly to a 5V Arduino (like Uno) without logic level shifters.

| ADS1299 Pin | Pin Name | Description | Arduino Pin (Example) |
| :--- | :--- | :--- | :--- |
| **39** | **CS** | Chip Select (Active Low) | 10 |
| **34** | **DIN** | MOSI (Data In) | 11 (MOSI) |
| **43** | **DOUT** | MISO (Data Out) | 12 (MISO) |
| **40** | **SCLK** | Serial Clock | 13 (SCK) |
| **47** | **DRDY** | Data Ready (Active Low) | 9 |
| **36** | **RESET** | System Reset (Active Low) | 8 |
| **38** | **START** | Start Conversion | 3.3V (or Digital Pin) |
| **35** | **PWDN** | Power Down | 3.3V |

## Usage

### 1. Include and Initialize
```cpp
#include "ADS1299_DR.h"

// CS, DRDY, RESET
ADS1299_DR EEG(10, 9, 8); 

void setup() {
  EEG.begin(); // Starts SPI and performs hardware reset
  
  // Device wakes in RDATAC mode. 
  // Commands are ignored until SDATAC is sent (handled by library).
  
  // Example: Set CONFIG3 to enable internal reference buffer (4.5V)
  EEG.writeRegister(ADS_CONFIG3, 0xE0); 
  
  EEG.rdatac(); // Return to continuous data mode
  EEG.start();  // Start conversions
}
2. Reading Data
The updateData() function checks the DRDY pin. If data is available, it reads the status word and channel data.
void loop() {
  if (EEG.updateData()) {
    // Access public variable 'channelData' (array of longs)
    long channel1_raw = EEG.channelData;
    
    // Convert to Voltage: (Data * 4.5V) / (Gain * 2^23)
    float voltage = ((float)channel1_raw * 4.5f) / (24.0f * 8388607.0f);
  }
}
References
// Texas Instruments ADS1299 Datasheet (https://www.ti.com/lit/ds/symlink/ads1299.pdf)
