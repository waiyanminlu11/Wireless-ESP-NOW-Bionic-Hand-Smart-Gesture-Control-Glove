#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

// ======================================================
// BIONIC HAND - ROBOT HAND SIDE
// ESP-NOW RECEIVER -> 5 SERVO MOTORS
// ======================================================


// ======================================================
// CREATE FIVE SERVO OBJECTS
// ======================================================

Servo thumbServo;
Servo indexServo;
Servo middleServo;
Servo ringServo;
Servo littleServo;


// ======================================================
// SERVO PINS
// ======================================================

const int THUMB_SERVO_PIN  = 13;
const int INDEX_SERVO_PIN  = 14;
const int MIDDLE_SERVO_PIN = 25;
const int RING_SERVO_PIN   = 26;
const int LITTLE_SERVO_PIN = 27;


// ======================================================
// DATA STRUCTURE
// MUST BE EXACTLY SAME AS GLOVE SIDE
// ======================================================

typedef struct struct_message {

  int thumb;
  int index;
  int middle;
  int ring;
  int little;

} struct_message;


// Incoming packet
struct_message receivedData;


// Latest finger angles
int thumbAngle  = 0;
int indexAngle  = 0;
int middleAngle = 0;
int ringAngle   = 0;
int littleAngle = 0;


// New data flag
volatile bool newData = false;


// ======================================================
// ESP-NOW RECEIVE CALLBACK
// ======================================================

void OnDataRecv(
  const esp_now_recv_info_t *recv_info,
  const uint8_t *incomingData,
  int len
) {

  // Make sure packet size is correct
  if (len == sizeof(receivedData)) {

    memcpy(
      &receivedData,
      incomingData,
      sizeof(receivedData)
    );

    // Copy incoming finger angles
    thumbAngle =
      receivedData.thumb;

    indexAngle =
      receivedData.index;

    middleAngle =
      receivedData.middle;

    ringAngle =
      receivedData.ring;

    littleAngle =
      receivedData.little;

    newData = true;
  }
}


// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("==================================");
  Serial.println(" BIONIC ROBOT HAND");
  Serial.println(" ESP-NOW 5 SERVO RECEIVER");
  Serial.println("==================================");


  // ====================================================
  // ATTACH SERVOS
  // ====================================================

  thumbServo.attach(
    THUMB_SERVO_PIN
  );

  indexServo.attach(
    INDEX_SERVO_PIN
  );

  middleServo.attach(
    MIDDLE_SERVO_PIN
  );

  ringServo.attach(
    RING_SERVO_PIN
  );

  littleServo.attach(
    LITTLE_SERVO_PIN
  );


  // Initial position
  thumbServo.write(0);
  indexServo.write(0);
  middleServo.write(0);
  ringServo.write(0);
  littleServo.write(0);


  // ====================================================
  // START WI-FI RADIO
  // ====================================================

  WiFi.mode(WIFI_STA);


  // ====================================================
  // INITIALIZE ESP-NOW
  // ====================================================

  if (esp_now_init() != ESP_OK) {

    Serial.println(
      "ERROR: ESP-NOW initialization failed!"
    );

    return;
  }


  // ====================================================
  // REGISTER RECEIVE CALLBACK
  // ====================================================

  esp_now_register_recv_cb(
    OnDataRecv
  );


  Serial.println(
    "ESP-NOW initialized successfully."
  );

  Serial.println(
    "Robot Hand Receiver READY."
  );

  Serial.println(
    "Waiting for Glove data..."
  );

  Serial.println();
}


// ======================================================
// MAIN LOOP
// ======================================================

void loop() {

  if (newData) {

    newData = false;


    // ==================================================
    // SAFETY LIMIT
    // ==================================================

    thumbAngle =
      constrain(thumbAngle, 0, 180);

    indexAngle =
      constrain(indexAngle, 0, 180);

    middleAngle =
      constrain(middleAngle, 0, 180);

    ringAngle =
      constrain(ringAngle, 0, 180);

    littleAngle =
      constrain(littleAngle, 0, 180);


    // ==================================================
    // MOVE ALL FIVE SERVOS
    // ==================================================

    thumbServo.write(
      thumbAngle
    );

    indexServo.write(
      indexAngle
    );

    middleServo.write(
      middleAngle
    );

    ringServo.write(
      ringAngle
    );

    littleServo.write(
      littleAngle
    );


    // ==================================================
    // SERIAL MONITOR
    // ==================================================

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
}