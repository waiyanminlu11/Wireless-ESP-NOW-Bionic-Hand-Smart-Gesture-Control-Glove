#include <WiFi.h>
#include <esp_now.h>

// ======================================================
// BIONIC HAND - GLOVE SIDE
// 5 FLEX SENSORS -> ESP-NOW TRANSMITTER
// ======================================================

// ---------- Flex Sensor Pins ----------
const int FLEX_COUNT = 5;

const int flexPins[FLEX_COUNT] = {
  32,   // Thumb
  33,   // Index
  34,   // Middle
  35,   // Ring
  36    // Little
};

// ---------- Sensor Values ----------
int flexValue[FLEX_COUNT];

// ---------- Auto Calibration ----------
int minVal[FLEX_COUNT] = {
  4095, 4095, 4095, 4095, 4095
};

int maxVal[FLEX_COUNT] = {
  0, 0, 0, 0, 0
};

// ---------- Finger Names ----------
const char* fingerNames[FLEX_COUNT] = {
  "Thumb",
  "Index",
  "Middle",
  "Ring",
  "Little"
};

// ======================================================
// DATA PACKET
// ======================================================

typedef struct struct_message {

  int thumb;
  int index;
  int middle;
  int ring;
  int little;

} struct_message;

struct_message handData;


// ======================================================
// ROBOT HAND ESP32 MAC ADDRESS
// ======================================================

// CHANGE THIS TO YOUR REAL ROBOT ESP32 MAC ADDRESS
//
// Example:
//
// Robot MAC = A0:B7:65:12:34:56
//
// Write:
//
// {0xA0, 0xB7, 0x65, 0x12, 0x34, 0x56}

uint8_t receiverAddress[] = {
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};


// ---------- ESP-NOW Peer ----------
esp_now_peer_info_t peerInfo;


// ======================================================
// FLEX SENSOR -> SERVO ANGLE
// ======================================================

int readFlexAngle(int sensorNumber) {

  // Read sensor
  flexValue[sensorNumber] =
    analogRead(flexPins[sensorNumber]);

  // Update minimum value
  if (flexValue[sensorNumber] < minVal[sensorNumber]) {
    minVal[sensorNumber] = flexValue[sensorNumber];
  }

  // Update maximum value
  if (flexValue[sensorNumber] > maxVal[sensorNumber]) {
    maxVal[sensorNumber] = flexValue[sensorNumber];
  }

  int angle = 0;

  // Prevent invalid map range
  if (maxVal[sensorNumber] > minVal[sensorNumber]) {

    angle = map(
      flexValue[sensorNumber],
      minVal[sensorNumber],
      maxVal[sensorNumber],
      0,
      180
    );

    angle = constrain(angle, 0, 180);
  }

  return angle;
}


// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("==================================");
  Serial.println(" BIONIC HAND - GLOVE ESP32");
  Serial.println(" 5 FLEX SENSOR ESP-NOW SENDER");
  Serial.println("==================================");

  // ESP-NOW requires Wi-Fi radio
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {

    Serial.println("ERROR: ESP-NOW initialization failed!");

    return;
  }

  Serial.println("ESP-NOW initialized successfully.");

  // Clear peer information
  memset(&peerInfo, 0, sizeof(peerInfo));

  // Copy Robot ESP32 MAC address
  memcpy(
    peerInfo.peer_addr,
    receiverAddress,
    6
  );

  peerInfo.channel = 0;

  peerInfo.encrypt = false;

  peerInfo.ifidx = WIFI_IF_STA;


  // Add Robot ESP32 as peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {

    Serial.println("ERROR: Failed to add Robot Hand ESP32!");

    return;
  }

  Serial.println("Robot Hand peer added successfully.");

  Serial.println();
  Serial.println("Glove Sender READY");
  Serial.println();

  Serial.println(
    "Move every finger from STRAIGHT to FULLY BENT"
  );

  Serial.println(
    "several times for automatic calibration."
  );

  Serial.println();
}


// ======================================================
// MAIN LOOP
// ======================================================

void loop() {

  // ====================================================
  // READ ALL FIVE FLEX SENSORS
  // ====================================================

  int thumbAngle =
    readFlexAngle(0);

  int indexAngle =
    readFlexAngle(1);

  int middleAngle =
    readFlexAngle(2);

  int ringAngle =
    readFlexAngle(3);

  int littleAngle =
    readFlexAngle(4);


  // ====================================================
  // PUT ANGLES INTO ESP-NOW PACKET
  // ====================================================

  handData.thumb =
    thumbAngle;

  handData.index =
    indexAngle;

  handData.middle =
    middleAngle;

  handData.ring =
    ringAngle;

  handData.little =
    littleAngle;


  // ====================================================
  // SEND COMPLETE HAND DATA
  // ====================================================

  esp_err_t result =
    esp_now_send(
      receiverAddress,
      (uint8_t *)&handData,
      sizeof(handData)
    );


  // ====================================================
  // SERIAL MONITOR
  // ====================================================

  if (result == ESP_OK) {

    Serial.print("Thumb: ");
    Serial.print(thumbAngle);

    Serial.print(" | Index: ");
    Serial.print(indexAngle);

    Serial.print(" | Middle: ");
    Serial.print(middleAngle);

    Serial.print(" | Ring: ");
    Serial.print(ringAngle);

    Serial.print(" | Little: ");
    Serial.println(littleAngle);
  }

  else {

    Serial.println("ESP-NOW SEND ERROR");
  }


  // Approximately 20 updates per second
  delay(50);
}