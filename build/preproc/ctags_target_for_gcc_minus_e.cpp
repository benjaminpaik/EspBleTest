# 1 "C:\\Users\\benja\\Documents\\Arduino\\EspBleTest\\EspBleTest.ino"
# 2 "C:\\Users\\benja\\Documents\\Arduino\\EspBleTest\\EspBleTest.ino" 2
# 3 "C:\\Users\\benja\\Documents\\Arduino\\EspBleTest\\EspBleTest.ino" 2

BLEService customService("C700");
// four byte fixed length output
BLECharacteristic dataOut("C701", BLERead | BLENotify, 8, true);
// five byte fixed length input
BLECharacteristic dataIn("C702", BLERead | BLEWrite, 3, true);

uint32_t startTime = 0, elapsedTime = 0;
int16_t cmdData = 0;
float_t sinData = 0;
int32_t rxData[2];
uint8_t txBuffer[8], rxBuffer[3];

void setup() {
  Serial0.begin(115200);
  if(BLE.begin()) {
    BLE.setLocalName("BenEspBle");
    BLE.setAdvertisedService(customService);
    customService.addCharacteristic(dataOut);
    dataIn.setEventHandler(BLEWritten, newDataRecieved);
    customService.addCharacteristic(dataIn);
    BLE.addService(customService);
    // start BLE services
    BLE.advertise();
    Serial0.println("waiting for connection");
  }
  else {
    while(true);
  }
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    Serial0.print("Connected to central: ");
    Serial0.println(central.address());
    while (central.connected()) {
      // update command data
      cmdData = ((int16_t)rxBuffer[1] << 8) | (rxBuffer[0]);
      loadData32(txBuffer, 0, cmdData);
      // update sine data
      sinData = update_sin();
      loadData32(txBuffer, 1, *(int32_t*)(&sinData));
      // load outputs
      startTime = micros();
      dataOut.writeValue(txBuffer, 8, BLEWriteWithoutResponse);
      elapsedTime = micros() - startTime;
      Serial0.println("t: " + String(elapsedTime));
      delay(20);
    }
  }
  Serial0.print("Disconnected from central: ");
  Serial0.println(central.address());
  delay(20);
}

void newDataRecieved(BLEDevice central, BLECharacteristic ch) {
  ch.readValue(rxBuffer, sizeof(rxBuffer));
  for(int i = 0; i < sizeof(rxBuffer); i++) {
    Serial0.print(rxBuffer[i]);
    Serial0.print(" ");
  }
  Serial0.println();
}

float_t update_sin(void) {
  static float_t time = 0;
  // generate sample data
  time += 0.002F;
  if(time >= 1.0F) {
    time = 0;
  }
  return sin(2.0F * 3.1415926535897932384626433832795 * time);
}

void loadData32(uint8_t* buffer, uint8_t dataIndex, int32_t data) {
  int16_t byteIndex = dataIndex * 4;
  buffer[byteIndex] = (data >> 24) & 0xFF;
  buffer[byteIndex + 1] = (data >> 16) & 0xFF;
  buffer[byteIndex + 2] = (data >> 8) & 0xFF;
  buffer[byteIndex + 3] = data & 0xFF;
}
