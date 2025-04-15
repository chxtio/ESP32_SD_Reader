#include <SD.h>
#include <SPI.h>

#define SD_CLK 41   // SPI Clock (SCK)
#define SD_DATA0 36 // Data Output (MISO)
#define SD_DATA1 37 // Data Input (MOSI)
#define CS 5        // Chip Select

// Create new SPI instance
SPIClass spi = SPIClass(FSPI);

void setup()
{
    Serial.begin(115200);
    // Initialize SPI communication protocol with custom pins
    spi.begin(SD_CLK, SD_DATA0, SD_DATA1);

    // Initialize SD card
    uint32_t freq = 80000000;
    if (!SD.begin(CS, spi, freq))
    {
        Serial.println("Failed to mount SD card");
        return;
    }
    Serial.println("SD card initialized");

    // Read a file
    File file = SD.open("/test.txt");
    if (!file) {
      Serial.println("Failed to open file");
      return;
    }
    Serial.println("File contents: ");
    while (file.available()){
      Serial.write(file.read());
    }
    file.close();
}

void loop()
{
}
