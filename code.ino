#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SparkFun_SCD4x_Arduino_Library.h>

SCD4x scd41;

const char* ssid = "M";
const char* password = "MinD2547";

*วาง URL จาก Google Apps Script ที่ Deploy แล้ว
String scriptURL = "https://script.google.com/macros/s/AKfycbwiuxZDaj8tyXpPPDkLb9pb36bNFPq3npwPmzv54Kds9GR43vLrtxK6svRv1inAT7FaCA/exec";

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  // เชื่อม WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // เริ่มต้น SCD41
  if (!scd41.begin()) {
    Serial.println("SCD41 not detected. Check wiring!");
    while (1);
  }

  scd41.startPeriodicMeasurement();
}

void loop() {

  delay(5000);  // วัดทุก 5 วินาที

  if (scd41.readMeasurement()) {

    int co2 = scd41.getCO2();
    float temperature = scd41.getTemperature();
    float humidity = scd41.getHumidity();

    Serial.print("CO2: ");
    Serial.print(co2);
    Serial.print(" ppm | Temp: ");
    Serial.print(temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    if (WiFi.status() == WL_CONNECTED) {

      HTTPClient http;

      String url = scriptURL + "?co2=" + String(co2) +
                   "&temp=" + String(temperature) +
                   "&hum=" + String(humidity);

      http.begin(url);
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.println("Data sent to Google Sheets");
      } else {
        Serial.println("Error sending data");
      }

      http.end();
    }
  }
}
