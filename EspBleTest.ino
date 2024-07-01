#include <ArduinoBLE.h>
#include "definitions.h"

BLEService customService("C700");
// four byte fixed length output
BLECharacteristic dataOut("C701", BLERead | BLENotify, DATA_OUT_SIZE, true);
// five byte fixed length input
BLECharacteristic dataIn("C702", BLERead | BLEWrite, DATA_IN_SIZE, true);

uint32_t startTime = 0, elapsedTime = 0;
int16_t cmdData = 0;
float_t sinData = 0;
int32_t rxData[TELEMETRY_STATES];
uint8_t txBuffer[DATA_OUT_SIZE], rxBuffer[DATA_IN_SIZE];

void setup() {
  Serial.begin(115200);
  if(BLE.begin()) {
    BLE.setLocalName("BenEspBle");
    BLE.setAdvertisedService(customService);
    customService.addCharacteristic(dataOut);
    dataIn.setEventHandler(BLEWritten, newDataRecieved);
    customService.addCharacteristic(dataIn);
    BLE.addService(customService);
    // start BLE services
    BLE.advertise();
    Serial.println("waiting for connection");
  }
  else {
    while(true);
  }
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    while (central.connected()) {
      // update command data
      cmdData = BYTES_TO_INT16(rxBuffer);
      loadData32(txBuffer, 0, cmdData);
      // update sine data
      sinData = update_sin();
      loadData32(txBuffer, 1, FLOAT_TO_INT_BITS(sinData));
      // load outputs
      startTime = micros();
      dataOut.writeValue(txBuffer, DATA_OUT_SIZE, BLEWriteWithoutResponse);
      elapsedTime = micros() - startTime;
      Serial.println("t: " + String(elapsedTime));
      delay(UPDATE_INTERVAL);
    }
  }
  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
  delay(UPDATE_INTERVAL);
}

void newDataRecieved(BLEDevice central, BLECharacteristic ch) {
  ch.readValue(rxBuffer, sizeof(rxBuffer));
  for(int i = 0; i < sizeof(rxBuffer); i++) {
    Serial.print(rxBuffer[i]);
    Serial.print(" ");
  }
  Serial.println();
}

float_t update_sin(void) {
  static float_t time = 0;
  // generate sample data
  time += 0.002F;
  if(time >= 1.0F) {
    time = 0;
  }
  return sin(2.0F * PI * time);
}

void loadData32(uint8_t* buffer, uint8_t dataIndex, int32_t data) {
  int16_t byteIndex = dataIndex * 4;
  buffer[byteIndex] = (data >> 24) & 0xFF;
  buffer[byteIndex + 1] = (data >> 16) & 0xFF;
  buffer[byteIndex + 2] = (data >> 8) & 0xFF;
  buffer[byteIndex + 3] = data & 0xFF;
}