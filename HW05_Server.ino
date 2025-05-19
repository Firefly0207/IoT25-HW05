#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <DHT.h>

#define DHT_PIN 13
#define DHT_TYPE DHT11  // DHT11 or DHT22

DHT dht(DHT_PIN, DHT_TYPE);

#define BLE_SERVER_NAME "ESP32_DHT"

#define SERVICE_UUID       "91bad492-b950-4226-aa2b-4ede9fa42f59"
#define TEMP_UUID          "cba1d466-344c-4be3-ab3f-189f80dd7518"
#define HUMIDITY_UUID      "ca73b3ba-39f6-4ab3-91ae-186dc9577d99"

BLECharacteristic tempCharacteristic(TEMP_UUID, BLECharacteristic::PROPERTY_NOTIFY);
BLECharacteristic humidityCharacteristic(HUMIDITY_UUID, BLECharacteristic::PROPERTY_NOTIFY);

bool deviceConnected = false;

unsigned long lastTime = 0;
const unsigned long interval = 5000;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Client Connected.");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Client Disconnected.");
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 BLE Server started...");

  dht.begin();

  BLEDevice::init(BLE_SERVER_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pService->addCharacteristic(&tempCharacteristic);
  tempCharacteristic.addDescriptor(new BLE2902());

  pService->addCharacteristic(&humidityCharacteristic);
  humidityCharacteristic.addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();

  Serial.println("BLE advertising started.");
}

void loop() {
  if (deviceConnected) {
    if (millis() - lastTime >= interval) {
      lastTime = millis();

      float temperature = dht.readTemperature();
      float humidity = dht.readHumidity();

      if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read data from sensor.");
        return;
      }

      char tempStr[8];
      char humStr[8];
      dtostrf(temperature, 6, 2, tempStr);
      dtostrf(humidity, 6, 2, humStr);

      tempCharacteristic.setValue(tempStr);
      tempCharacteristic.notify();

      humidityCharacteristic.setValue(humStr);
      humidityCharacteristic.notify();

      Serial.print("Temperature: ");
      Serial.print(tempStr);
      Serial.print(" °C | Humidity: ");
      Serial.print(humStr);
      Serial.println(" %");
    }
  }

  delay(100);
}
