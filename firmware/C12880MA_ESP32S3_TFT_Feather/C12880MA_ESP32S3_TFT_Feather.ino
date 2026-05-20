/*
  Hamamatsu C12880MA spectrometer reader
  Target: Adafruit ESP32-S3 TFT Feather / ESP32-S3 Reverse TFT Feather

  Notes:
  - SPEC_VIDEO is assigned to A5 / GPIO8 because it is on ADC1.
  - A0..A4 are ADC2 on this board; avoid them for the spectrometer video signal.
  - ESP32-S3 ADC input must never exceed 3.3 V.
*/

#include <Arduino.h>

// Exposed Feather GPIO pins. Adjust only if your wiring differs.
constexpr uint8_t SPEC_TRG   = 9;   // optional, kept LOW in this first version
constexpr uint8_t SPEC_ST    = 6;
constexpr uint8_t SPEC_CLK   = 5;
constexpr uint8_t SPEC_VIDEO = A5;  // GPIO8, ADC1
constexpr uint8_t WHITE_LED  = 11;
constexpr uint8_t LASER_404  = 12;

constexpr uint16_t SPEC_CHANNELS = 288;
constexpr uint8_t CLOCK_DELAY_US = 1;

uint16_t data[SPEC_CHANNELS];

static inline void clockPulse()
{
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(CLOCK_DELAY_US);
  digitalWrite(SPEC_CLK, LOW);
  delayMicroseconds(CLOCK_DELAY_US);
}

void setup()
{
  pinMode(SPEC_TRG, OUTPUT);
  pinMode(SPEC_CLK, OUTPUT);
  pinMode(SPEC_ST, OUTPUT);
  pinMode(LASER_404, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  digitalWrite(SPEC_TRG, LOW);
  digitalWrite(SPEC_CLK, HIGH);
  digitalWrite(SPEC_ST, LOW);
  digitalWrite(LASER_404, LOW);
  digitalWrite(WHITE_LED, LOW);

  analogReadResolution(12); // ESP32-S3: values from 0 to 4095
  analogSetPinAttenuation(SPEC_VIDEO, ADC_11db);

  Serial.begin(921600);
  delay(1500);

  digitalWrite(LASER_404, HIGH);
  delay(500);
  digitalWrite(LASER_404, LOW);

  digitalWrite(WHITE_LED, HIGH);
  delay(500);
  digitalWrite(WHITE_LED, LOW);
}

/*
  Reads spectrometer data from SPEC_VIDEO.
  Sequence follows the C12880MA timing chart used by the original Arduino sketch.
*/
void readSpectrometer()
{
  digitalWrite(SPEC_CLK, LOW);
  delayMicroseconds(CLOCK_DELAY_US);
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(CLOCK_DELAY_US);
  digitalWrite(SPEC_CLK, LOW);

  digitalWrite(SPEC_ST, HIGH);
  delayMicroseconds(CLOCK_DELAY_US);

  for (uint8_t i = 0; i < 15; i++) {
    clockPulse();
  }

  digitalWrite(SPEC_ST, LOW);

  for (uint8_t i = 0; i < 85; i++) {
    clockPulse();
  }

  clockPulse();

  for (uint16_t i = 0; i < SPEC_CHANNELS; i++) {
    data[i] = analogRead(SPEC_VIDEO);
    clockPulse();
  }

  digitalWrite(SPEC_ST, HIGH);

  for (uint8_t i = 0; i < 7; i++) {
    clockPulse();
  }

  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(CLOCK_DELAY_US);
}

void printData()
{
  for (uint16_t i = 0; i < SPEC_CHANNELS; i++) {
    if (i > 0) {
      Serial.print(',');
    }
    Serial.print(data[i]);
  }
  Serial.println();
}

void loop()
{
  readSpectrometer();
  printData();
  delay(10);
}
