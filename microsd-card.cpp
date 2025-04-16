#include <SD.h>
#include <SPI.h>

#define SD_CLK 41   // SPI Clock (SCK)
#define SD_DATA0 36 // Data Output (MISO)
#define SD_DATA1 37 // Data Input (MOSI)
#define CS 5        // Chip Select

// Create new SPI instance
SPIClass spi = SPIClass(FSPI);

// List files and sizes
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if(levels){
        listDir(fs, file.name(), levels -1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void readFromFile(fs::FS &fs,const char *path) {
    File file = fs.open(path);
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

void writeToFile(fs::FS &fs, const char *path, const char *message) {
    File file = fs.open(path, FILE_WRITE);
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

    listDir(SD, "/", 0);

    readFromFile(SD, "/test.txt");
    writeToFile(SD, "/test2.txt", "Test message");
    readFromFile(SD, "/test2.txt");

    listDir(SD, "/", 0);

    // Listing directory: /
    //   FILE: sketch.ino  SIZE: 2515
    //   FILE: diagram.json  SIZE: 840
    //   FILE: buoyancy-system.chip.json  SIZE: 389
    //   FILE: buoyancy-system.chip.c  SIZE: 10293
    //   FILE: test.txt  SIZE: 194
    //   FILE: test2.txt  SIZE: 12

}

void loop() {}
