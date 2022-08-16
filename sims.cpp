#include <Arduino.h>
#include <ESP32QRCodeReader.h>

ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);

void setup(){
  Serial.begin(115200);
  reader.setup();
  reader.beginOnCore(1);
  xTaskCreate(loop, "onQrCode", 4 * 1024, NULL, 4, NULL);
}

void loop(void *pvParameters){
  struct QRCodeData qrCodeData;
  while (true){
    if (reader.receiveQrCode(&qrCodeData, 100)){
      if (qrCodeData.valid){
        Serial.println((const char *)qrCodeData.payload);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  delay(50);
}