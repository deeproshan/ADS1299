#include "ADS1299_DR.h"

// Pin Definitions
const int PIN_CS = 10;
const int PIN_DRDY = 9;
const int PIN_RESET = 8;

// Instantiate the new class
ADS1299_DR EEG(PIN_CS, PIN_DRDY, PIN_RESET);

void setup() {
  Serial.begin(115200);
  while(!Serial); 
  
  Serial.println("Initializing ADS1299_DR...");
  EEG.begin();

  // 1. Get Device ID [18]
  // Should return 0x3E for ADS1299-8
  byte id = EEG.readRegister(ADS_ID);
  Serial.print("Device ID: 0x");
  Serial.println(id, HEX);

  // 2. Configure Registers 
  // Set data rate to 250 SPS [19]
  EEG.writeRegister(ADS_CONFIG1, 0x96); 
  
  // Enable internal reference [20]
  EEG.writeRegister(ADS_CONFIG3, 0xE0);

  // Set channels to Test Signal Input [21]
  // Gain 24 (0x60) + Test Signal (0x05) = 0x65
  for(int i = ADS_CH1SET; i <= ADS_CH8SET; i++) {
    EEG.writeRegister(i, 0x65); 
  }

  // 3. Start
  EEG.rdatac(); 
  EEG.start();  
}

void loop() {
  if (EEG.updateData()) {
    // Access data using index 0-7
    Serial.print("CH1: ");
    Serial.println(EEG.channelData); 
  }
}
