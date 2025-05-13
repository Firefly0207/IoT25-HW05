#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "d360f1a0-5ccc-47bb-8936-3359a107cc01"
#define CHARACTERISTIC_UUID "dcf02970-e13f-4216-a337-39530e02359a"

BLEServer* pServer;
BLECharacteristic* pCharacteristic;

void setup() {
  Serial.begin(115200);

  BLEDevice::init("ESP32_Server");
  pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->setValue("Hello from Server");
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE Server Ready");
}

void loop() {
  static int count = 0;
  String value = "Message " + String(count++);
  pCharacteristic->setValue(value.c_str());
  pCharacteristic->notify(); // 알림 전송
  Serial.println("Notified: " + value);
  delay(2000);
}
