#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SERVICE_UUID        "d360f1a0-5ccc-47bb-8936-3359a107cc01"
#define CHARACTERISTIC_UUID "dcf02970-e13f-4216-a337-39530e02359a"

BLEAdvertisedDevice* myDevice;
BLEClient*  pClient;
BLERemoteCharacteristic* pRemoteCharacteristic;

bool deviceConnected = false;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (advertisedDevice.haveServiceUUID() &&
          advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
        Serial.println("Found target device!");
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        BLEDevice::getScan()->stop();
      }
    }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(10);
}

void loop() {
  if (myDevice && !deviceConnected) {
    pClient = BLEDevice::createClient();
    Serial.println("Connecting to server...");
    pClient->connect(myDevice); 

    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
    if (pRemoteService == nullptr) {
      Serial.println("Service not found.");
      pClient->disconnect();
      return;
    }

    pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.println("Characteristic not found.");
      pClient->disconnect();
      return;
    }

    if (pRemoteCharacteristic->canNotify()) {
      pRemoteCharacteristic->registerForNotify(notifyCallback);
    }

    deviceConnected = true;
  }
  delay(1000);
}

void notifyCallback(BLERemoteCharacteristic* pCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  String received = "";
  for (size_t i = 0; i < length; i++) {
    received += (char)pData[i];
  }
  Serial.print("Received: ");
  Serial.println(received);
}
