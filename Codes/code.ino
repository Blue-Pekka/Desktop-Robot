#include <Wire.h>
#include <U8g2lib.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


// ======================================================
// OLED
// ======================================================

#define OLED_SDA 2
#define OLED_SCL 15

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(
  U8G2_R0,
  U8X8_PIN_NONE
);


// ======================================================
// TB6612FNG MOTOR DRIVER
// ======================================================

#define PWMA 13
#define AIN1 14
#define AIN2 12

#define PWMB 33
#define BIN1 26
#define BIN2 25

#define STBY 27


// ======================================================
// ULTRASONIC SENSOR
// ======================================================

#define TRIG_PIN 22
#define ECHO_PIN 23


// ======================================================
// BUZZER
// ======================================================

#define BUZZER_PIN 4


// ======================================================
// SPEED SETTINGS
// ======================================================

// DELIBERATELY SLOW
int forwardSpeed = 60;
int reverseSpeed = 50;
int turnSpeed = 45;


// ======================================================
// AUTONOMOUS SETTINGS
// ======================================================

// Your sensor realistically reaches about 16 cm.
// We therefore react well before the maximum range.
const float OBSTACLE_DISTANCE = 11.0;


// ======================================================
// ROBOT MODE
// ======================================================

bool autonomousMode = true;


// ======================================================
// BLUETOOTH / BLE
// ======================================================

BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;

bool deviceConnected = false;


// Nordic UART Service UUIDs
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


// ======================================================
// BLE CALLBACKS
// ======================================================

class MyServerCallbacks : public BLEServerCallbacks {

  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;

    // Start advertising again
    pServer->getAdvertising()->start();
  }
};


class MyCallbacks : public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic *pCharacteristic) {

    String command = pCharacteristic->getValue().c_str();

    if (command.length() == 0) {
      return;
    }

    char cmd = command[0];

    // ==============================================
    // MODE COMMANDS
    // ==============================================

    if (cmd == 'A' || cmd == 'a') {

      autonomousMode = true;

      stopMotors();

      Serial.println("MODE: AUTONOMOUS");
    }


    else if (cmd == 'M' || cmd == 'm') {

      autonomousMode = false;

      stopMotors();

      Serial.println("MODE: MANUAL");
    }


    // ==============================================
    // MANUAL MOVEMENT
    // ==============================================

    else if (!autonomousMode) {

      switch (cmd) {

        case 'F':
        case 'f':
          forward();
          break;


        case 'B':
        case 'b':
          reverse();
          break;


        case 'L':
        case 'l':
          turnLeft();
          break;


        case 'R':
        case 'r':
          turnRight();
          break;


        case 'S':
        case 's':
          stopMotors();
          break;
      }
    }
  }
};


// ======================================================
// MOTOR FUNCTIONS
// ======================================================

void forward() {

  // Left motor
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  // Right motor
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, forwardSpeed);
  analogWrite(PWMB, forwardSpeed);
}


void reverse() {

  // Left motor
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  // Right motor
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, reverseSpeed);
  analogWrite(PWMB, reverseSpeed);
}


// ======================================================
// CONTROLLED LEFT TURN
// ======================================================

void turnLeft() {

  // LEFT wheel stopped
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);

  analogWrite(PWMA, 0);


  // RIGHT wheel moves slowly
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMB, turnSpeed);
}


// ======================================================
// CONTROLLED RIGHT TURN
// ======================================================

void turnRight() {

  // RIGHT wheel stopped
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMB, 0);


  // LEFT wheel moves slowly
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  analogWrite(PWMA, turnSpeed);
}


// ======================================================
// STOP
// ======================================================

void stopMotors() {

  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}


// ======================================================
// ULTRASONIC DISTANCE
// ======================================================

float getDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);


  long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    30000
  );


  if (duration == 0) {
    return -1;
  }


  float distance = duration * 0.0343 / 2.0;


  // Since your sensor has been behaving reliably only
  // within approximately 16 cm, ignore unrealistic values.

  if (distance > 16.0) {
    return -1;
  }


  return distance;
}


// ======================================================
// BUZZER
// ======================================================

void obstacleBeep() {

  tone(BUZZER_PIN, 3000);

  delay(120);

  noTone(BUZZER_PIN);
}


// ======================================================
// OLED
// ======================================================

void updateOLED(float distance) {

  oled.clearBuffer();

  oled.setFont(u8g2_font_ncenB08_tr);


  // Title
  oled.drawStr(0, 11, "RAQEEB ROBOT");


  // Mode
  oled.setCursor(0, 27);

  if (autonomousMode) {
    oled.print("MODE: AUTO");
  }
  else {
    oled.print("MODE: MANUAL");
  }


  // Distance
  oled.setCursor(0, 41);
  oled.print("DIST: ");

  if (distance < 0) {
    oled.print("--");
  }
  else {
    oled.print(distance, 1);
    oled.print(" cm");
  }


  // Speed
  oled.setCursor(0, 55);
  oled.print("SPD: ");

  if (autonomousMode) {
    oled.print(forwardSpeed);
  }
  else {
    oled.print(forwardSpeed);
  }


  // Bluetooth indicator
  oled.setCursor(82, 27);

  if (deviceConnected) {
    oled.print("BT ON");
  }
  else {
    oled.print("BT --");
  }


  oled.sendBuffer();
}


// ======================================================
// AUTONOMOUS MODE
// ======================================================

void autonomousDrive() {

  float distance = getDistance();


  Serial.print("Distance: ");

  if (distance < 0) {
    Serial.println("No Echo");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }


  updateOLED(distance);


  // ====================================================
  // OBSTACLE
  // ====================================================

  if (
    distance > 0 &&
    distance <= OBSTACLE_DISTANCE
  ) {

    Serial.println("OBSTACLE DETECTED!");


    // STOP
    stopMotors();

    updateOLED(distance);

    delay(100);


    // BUZZER
    obstacleBeep();


    // ==================================================
    // SHORT REVERSE
    // ==================================================

    reverse();

    delay(250);


    // STOP BEFORE TURN
    stopMotors();

    delay(100);


    // ==================================================
    // CONTROLLED RIGHT TURN
    // ==================================================

    turnRight();

    delay(450);


    // STOP
    stopMotors();

    delay(150);
  }


  // ====================================================
  // NOTHING IN FRONT
  // ====================================================

  else {

    forward();
  }
}


// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);


  // ====================================================
  // MOTOR PINS
  // ====================================================

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);


  // Enable TB6612
  digitalWrite(STBY, HIGH);


  // ====================================================
  // ULTRASONIC
  // ====================================================

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);


  // ====================================================
  // BUZZER
  // ====================================================

  pinMode(BUZZER_PIN, OUTPUT);


  // ====================================================
  // OLED
  // ====================================================

  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );

  oled.begin();

  oled.clearBuffer();

  oled.setFont(
    u8g2_font_ncenB08_tr
  );

  oled.drawStr(
    0,
    20,
    "RAQEEB ROBOT"
  );

  oled.drawStr(
    0,
    40,
    "Starting..."
  );

  oled.sendBuffer();


  delay(1500);


  // ====================================================
  // BLE
  // ====================================================

  BLEDevice::init("RAQEEB ROBOT");

  pServer = BLEDevice::createServer();

  pServer->setCallbacks(
    new MyServerCallbacks()
  );


  BLEService *pService =
    pServer->createService(
      SERVICE_UUID
    );


  // TX characteristic
  BLECharacteristic *txCharacteristic =
    pService->createCharacteristic(
      CHARACTERISTIC_UUID_TX,
      BLECharacteristic::PROPERTY_NOTIFY
    );

  txCharacteristic->addDescriptor(
    new BLE2902()
  );


  // RX characteristic
  pCharacteristic =
    pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE
    );


  pCharacteristic->setCallbacks(
    new MyCallbacks()
  );


  pService->start();


  // Start advertising
  pServer->getAdvertising()->start();


  Serial.println(
    "RAQEEB ROBOT READY"
  );

  Serial.println(
    "BLE name: RAQEEB ROBOT"
  );
}


// ======================================================
// LOOP
// ======================================================

void loop() {

  // ====================================================
  // AUTONOMOUS
  // ====================================================

  if (autonomousMode) {

    autonomousDrive();

    delay(80);
  }


  // ====================================================
  // MANUAL
  // ====================================================

  else {

    // In manual mode the BLE callback
    // directly controls the motors.

    float distance = getDistance();

    updateOLED(distance);

    delay(100);
  }
}