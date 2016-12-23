/*
 * +---------+
 * | SD Card |
 * +---------+
 * MOSI - pin 11
 * MISO - pin 12
 * CLK  - pin 13
 * CS   - pin 4
 * 
 * +------------+
 * | GPS module |
 * +------------+
 * TX   - pin 3
 * RX   - pin 2
 */

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SD.h>

#define PIN_SD  4
#define PIN_LED 5

SoftwareSerial gpsSerial(3, 2);
Adafruit_GPS GPS(&gpsSerial);
uint32_t timer = millis();

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);

  Serial.print("Initializing SD card... ");
  if (!SD.begin(PIN_SD)) {
    Serial.println("Card failed, or not present");
    while (true) {
      digitalWrite(PIN_LED, HIGH);
      delay(50);
      digitalWrite(PIN_LED, LOW);
      delay(50);
    }
  }
  Serial.println("Card initialized.");

  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  gpsSerial.println(PMTK_Q_RELEASE);
}

void loop() {
  char c = GPS.read();
  /*if (c) {
    Serial.print(c);
  }*/

  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())) {
      return;
    }
  }

  if (timer > millis()) {
    timer = millis();
  }

  if (millis() - timer > 2000) {
    timer = millis();

    // Example: 2016-08-12T19:18:14.000Z 491.813 49.20268062 14.048416114

    // GPS.year returns only last two digits (i.e. "17" for year 2017). Change "20"
    // to "21" after 83 years and update it for every change of century.
    String line = "20";
    line += GPS.year;
    line += "-";
    if (GPS.month < 10) line += "0";
    line += GPS.month;
    line += "-";
    if (GPS.day < 10) line += "0";
    line += GPS.day;
    line += "T";
    if (GPS.hour < 10) line += "0";
    line += GPS.hour;
    line += ":";
    if (GPS.minute < 10) line += "0";
    line += GPS.minute;
    line += ":";
    if (GPS.seconds < 10) line += "0";
    line += GPS.seconds;
    line += ".";
    line += GPS.milliseconds;
    line += "Z";

    if (GPS.fix) {
      line += " ";
      line += GPS.altitude;
      line += " ";
      line += String(GPS.latitudeDegrees, 5);
      line += " ";
      line += String(GPS.longitudeDegrees, 5);
      line += " ";
      line += GPS.satellites;

      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      if (dataFile) {
        dataFile.println(line);
        dataFile.close();
        Serial.print("Save to SD ");
      } else {
        Serial.println("error opening datalog.txt");
        for (int i = 0; i < 5; i++) {
          digitalWrite(PIN_LED, HIGH);
          delay(100);
          digitalWrite(PIN_LED, LOW);
          delay(100);
        }
      }
    } else {
      digitalWrite(PIN_LED, HIGH);
      delay(100);
      digitalWrite(PIN_LED, LOW);
      delay(100);
    }

    Serial.println(line);
  }
}
