#include <SPI.h>
#include <DHT.h>

#define DHTTYPE DHT22
#define DHTPIN GPIO_NUM_26
DHT dht(DHTPIN, DHTTYPE);

bool getTemperature(float *temp, float *humi) {
  pinMode(DHTPIN, INPUT_PULLUP);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return false;
  }
  *temp = t;
  *humi = h;
  Serial.print("temperature:"); Serial.print(t);
  Serial.print(" humidity:"); Serial.print(h);
  Serial.print("\n");
  return true;
}
