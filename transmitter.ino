#include "MAX6675.h"
#include <SPI.h>
#include <LoRa.h>
#include <math.h>

#define MQ2pin A0
#define RL 5.0
float Ro = 137.8;
const int flamePin = 3;


const int dataPin   = 4;
const int clockPin  = 6;
const int selectPin = 5;
MAX6675 thermoCouple(selectPin, dataPin, clockPin);

const int ss = 9;
const int rst = 8;
const int dio0 = 7;

void setup() {
  SPI.begin();
  Serial.begin(9600);
  thermoCouple.begin();
  thermoCouple.setSPIspeed(4000000);

  pinMode(flamePin, INPUT);

  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (true);
  }
  Serial.println("LoRa Sender Ready");
}

void loop() {
  int status = thermoCouple.read();
  float temp = thermoCouple.getCelsius();
  int flameStatus = digitalRead(flamePin);

  int adcValue = analogRead(MQ2pin);
  float rs = ((1023.0 / adcValue) - 1.0) * RL;
  float ratio = rs / Ro;
  float ppm = pow(10, (-1.72 * log10(ratio) + 1.27));
  

  // Format kiriman data
  String data = "Suhu=" + String(temp, 2) +
                ";PPM=" + String(ppm, 2) +
                ";Api=" + String(flameStatus);

  // Kirim via LoRa
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  Serial.println(data);
  delay(2000);
}
