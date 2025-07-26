#include <SPI.h>
#include <LoRa.h>

#define SS      5
#define RST     14
#define DIO0    27
#define LED_PIN 4
#define BUZZER_PIN 26

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (true);
  }

  Serial.println("LoRa Receiver Ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String data = "";

    while (LoRa.available()) {
      data += (char)LoRa.read();
    }

    Serial.println("Diterima: " + data);

    // Parsing opsional (kalau mau langsung ditindaklanjuti nanti)
    int suhuIdx = data.indexOf("Suhu=");
    int ppmIdx = data.indexOf(";PPM=");
    int flameIdx = data.indexOf(";Api=");

    if (suhuIdx != -1 && ppmIdx != -1 && flameIdx != -1) {
      float suhu = data.substring(suhuIdx + 5, ppmIdx).toFloat();
      float ppm = data.substring(ppmIdx + 5, flameIdx).toFloat();
      int flame = data.substring(flameIdx + 5).toInt();

      Serial.print("Suhu: "); 
      Serial.println(suhu);
      Serial.print("PPM: "); 
      Serial.println(ppm);
      Serial.print("Api: "); 
      Serial.println(flame);
      if (suhu > 50 || ppm > 300 || flame == 1) { 
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
      }
    }
  }
}
