#include <SD.h>
#include <SPI.h>
#include <math.h>

#define SD_CLK 41   // SPI Clock (SCK)
#define SD_DATA0 36 // Data Output (MISO)
#define SD_DATA1 37 // Data Input (MOSI)
#define CS 5        // Chip Select

// Create new SPI instance
SPIClass spi = SPIClass(FSPI);

// Time and sensor reading variables
float startTime;
float currentTime;
float lastPrintTime = 0;
float depth;

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

void writeToCSV(fs::FS &fs, const char *path, const char *csvRow) {
    File file = fs.open(path, FILE_APPEND);  
    if (!file) {
        Serial.print("Failed to open CSV for writing: ");
        Serial.println(path);
        return;
    }
    if (file.println(csvRow)) {  // Add newline automatically
        Serial.print("CSV row written to ");
        Serial.println(path);
    } else {
        Serial.print("CSV write failed for ");
        Serial.println(path);
    }
    file.close();
}

void writeToCSV(fs::FS &fs, const char *path, float time, float depth) {
  File file = fs.open(path, FILE_APPEND);
  // File file = SD.open(filename, FILE_WRITE);
  if (file) {
    file.print(time, 3);     // prints time with 3 decimal places
    file.print(",");
    file.println(depth, 2);  // prints depth with 2 decimal places
    file.close();
  } else {
    Serial.println("Failed to open file");
  }
}

// simlulate depth increasing 
float getDepth() {
  depth += 0.05; 
  return depth;
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

    // readFromFile(SD, "/test.txt");
    // writeToFile(SD, "/test2.txt", "Test message");
    // readFromFile(SD, "/test2.txt");

    startTime = millis() / 1000.0;  // ms to seconds
    writeToCSV(SD, "/data.csv", "Time,Depth");
    // readFromFile(SD, "/data.csv"); // Confirm csv contents

    listDir(SD, "/", 0);

    // Listing directory: /
    //   FILE: sketch.ino  SIZE: 4172
    //   FILE: diagram.json  SIZE: 840
    //   FILE: buoyancy-system.chip.json  SIZE: 389
    //   FILE: buoyancy-system.chip.c  SIZE: 10293
    //   FILE: test.txt  SIZE: 194
    //   FILE: data.csv  SIZE: 12

}

void loop() {
  // Update readings
  currentTime = (millis() / 1000.0) - startTime;
  depth = getDepth(); 

  // Log values to SD Card
  writeToCSV(SD, "/data.csv", currentTime, depth);

  // Periodically display csv contents
  if (currentTime - lastPrintTime >= 10.0) {
    lastPrintTime = currentTime;
    readFromFile(SD, "/data.csv"); 
}
  
  delay(1000);

}


// Reading from /data.csv
// Time,Depth
// 0.025,0.05
// 1.031,0.10
// 2.042,0.15
// 3.048,0.20
// 4.059,0.25
// 5.065,0.30
// 6.076,0.35
// 7.082,0.40
// 8.093,0.45
// 9.099,0.50
// 10.110,0.55
