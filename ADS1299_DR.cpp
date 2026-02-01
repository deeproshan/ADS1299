#include "ADS1299_DR.h"

ADS1299_DR::ADS1299_DR(int csPin, int drdyPin, int resetPin) {
  _csPin = csPin;
  _drdyPin = drdyPin;
  _resetPin = resetPin;
}

void ADS1299_DR::begin() {
  pinMode(_csPin, OUTPUT);
  pinMode(_drdyPin, INPUT);
  pinMode(_resetPin, OUTPUT);
  
  digitalWrite(_csPin, HIGH); // CS must be high when inactive [6]
  digitalWrite(_resetPin, HIGH);

  // SPI Settings: CPOL=0, CPHA=1 (SPI_MODE1) [4]
  // Max SCLK speed depends on setup; 4MHz is generally safe.
  SPI.begin();
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE1));
  
  resetHardware();
}

// Hardware Reset Sequence [5]
void ADS1299_DR::resetHardware() {
  // Wait tPOR (approx 2^18 tCLKs)
  delay(50); 
  
  digitalWrite(_resetPin, LOW);
  delayMicroseconds(4); // tRST min is 2 tCLKs [7]
  digitalWrite(_resetPin, HIGH);
  delayMicroseconds(20); // Wait 18 tCLKs for initialization [8]

  // Device wakes up in RDATAC mode [9]. 
  // We must send SDATAC to write to registers [10].
  sdatac(); 
}

void ADS1299_DR::start() {
  transferCommand(_START);
}

void ADS1299_DR::stop() {
  transferCommand(_STOP);
}

void ADS1299_DR::rdatac() {
  transferCommand(_RDATAC);
}

void ADS1299_DR::sdatac() {
  transferCommand(_SDATAC);
}

// Register Read [11]
byte ADS1299_DR::readRegister(byte address) {
  byte opcode1 = _RREG | address; 
  byte opcode2 = 0x00; // Read 1 register (n-1 = 0)
  
  digitalWrite(_csPin, LOW);
  SPI.transfer(opcode1);
  SPI.transfer(opcode2);
  byte data = SPI.transfer(0x00);
  digitalWrite(_csPin, HIGH);
  
  return data;
}

// Register Write [12]
void ADS1299_DR::writeRegister(byte address, byte value) {
  byte opcode1 = _WREG | address;
  byte opcode2 = 0x00; // Write 1 register (n-1 = 0)
  
  digitalWrite(_csPin, LOW);
  SPI.transfer(opcode1);
  SPI.transfer(opcode2);
  SPI.transfer(value);
  digitalWrite(_csPin, HIGH);
}

// Send single byte command
void ADS1299_DR::transferCommand(byte cmd) {
  digitalWrite(_csPin, LOW);
  SPI.transfer(cmd);
  delayMicroseconds(2); // Decode time tSDECODE is 4 tCLKs [13]
  digitalWrite(_csPin, HIGH);
}

// Read Data Continuous or On-Demand
bool ADS1299_DR::updateData() {
  if (digitalRead(_drdyPin) == HIGH) {
    return false; // Data not ready
  }

  digitalWrite(_csPin, LOW);

  // Read 24 status bits (3 bytes) [14]
  for(int i = 0; i < 3; i++) {
    statusData[i] = SPI.transfer(0x00);
  }

  // Read 8 channels * 24 bits (3 bytes each)
  for(int i = 0; i < 8; i++) {
    byte buf[3];
    buf = SPI.transfer(0x00);
    buf[15] = SPI.transfer(0x00);
    buf[16] = SPI.transfer(0x00);
    
    // Combine into 24-bit signed integer
    long val = ((long)buf << 16) | ((long)buf[15] << 8) | buf[16];
    
    // Handle 24-bit Sign Extension to 32-bit long [17]
    if (val & 0x800000) {
      val |= 0xFF000000;
    }
    
    channelData[i] = val;
  }

  digitalWrite(_csPin, HIGH);
  return true;
}