#include <SD.h>
#include <SPI.h>

#define SD_CLK 41   // SPI Clock (SCK)
#define SD_DATA0 36 // Data Output (MISO)
#define SD_DATA1 37 // Data Input (MOSI)
#define CS 5        // Chip Select

// Create new SPI instance
SPIClass spi = SPIClass(FSPI);

void writeToFile(const char *path, const char *message) {
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.print("Failed to open file for writing: ");
        Serial.println(path);
        return;
    }

    if (file.print(message)) {
        Serial.print("\nFile written to ");
        Serial.println(path);
    } else {
        Serial.print("Write failed for ");
        Serial.println(path);
    }
    file.close();
}

void readFromFile(const char *path) {
    File file = SD.open(path);
    if (!file) {
        Serial.print("Failed to open file for reading: ");
        Serial.println(path);
        return;
    }

    Serial.print("\nReading from ");
    Serial.println(path);
    // Serial.println("File contents:");
    while (file.available()) {
        Serial.write(file.read());
    }
    Serial.println(); 
    file.close();
}

void setup() {
    Serial.begin(115200);
    
    // Initialize SPI communication protocol with custom pins
    spi.begin(SD_CLK, SD_DATA0, SD_DATA1);
    
    // Initialize SD card
    uint32_t freq = 80000000;
    if (!SD.begin(CS, spi, freq)) {
        Serial.println("Failed to mount SD card");
        return;
    }
    Serial.println("SD card initialized");

    readFromFile("/test.txt");
    writeToFile("/test2.txt", "Test message");
    readFromFile("/test2.txt");
}

void loop() {}
