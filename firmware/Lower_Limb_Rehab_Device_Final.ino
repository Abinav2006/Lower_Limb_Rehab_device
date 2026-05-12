#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <Arduino.h>

// ===== WIFI =====
const char* ssid = "Abinav's S22";
const char* password = "12345679";

// ===== WEBSOCKET =====
WebSocketsServer webSocket = WebSocketsServer(81);

// ===== PCA =====
#define PCA_ADDR 0x70

// ===== MPU =====
#define MPU_ADDR 0x68
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B

// ===== FSR =====
#define FSR_HEEL 0
#define FSR_MID  1
#define FSR_TOE  2

// ===== STEP =====
int steps = 0;
float cadence = 0;
unsigned long lastStepTime = 0;

// ===== ANKLE OFFSETS =====
float shank_offset = 0;
float foot_offset  = 0;

// ===== PCA SELECT =====
void selectChannel(uint8_t ch) {
  Wire.beginTransmission(PCA_ADDR);
  Wire.write(1 << ch);
  Wire.endTransmission();
}

// ===== MPU INIT =====
void initMPU(uint8_t ch) {
  selectChannel(ch);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission(true);
}

// ===== READ VECTOR =====
void readMPU(uint8_t ch, float &ax, float &ay, float &az) {
  selectChannel(ch);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);

  int16_t axr = Wire.read() << 8 | Wire.read();
  int16_t ayr = Wire.read() << 8 | Wire.read();
  int16_t azr = Wire.read() << 8 | Wire.read();

  ax = axr / 16384.0;
  ay = ayr / 16384.0;
  az = azr / 16384.0;
}

// ===== VECTOR ANGLE (KNEE) =====
float computeAngle(float ax1, float ay1, float az1,
                   float ax2, float ay2, float az2) {

  float dot = ax1*ax2 + ay1*ay2 + az1*az2;

  float mag1 = sqrt(ax1*ax1 + ay1*ay1 + az1*az1);
  float mag2 = sqrt(ax2*ax2 + ay2*ay2 + az2*az2);

  float cos_theta = dot / (mag1 * mag2);
  cos_theta = constrain(cos_theta, -1.0, 1.0);

  return acos(cos_theta) * 180.0 / PI;
}

// ===== CALIBRATION =====
void calibrate() {
  Serial.println("Keep leg STRAIGHT...");
  delay(4000);

  float ax, ay, az;

  readMPU(1, ax, ay, az); // shank
  shank_offset = atan2(-ax, sqrt(ay*ay + az*az)) * 180 / PI;

  readMPU(2, ax, ay, az); // foot
  foot_offset = atan2(-ax, sqrt(ay*ay + az*az)) * 180 / PI;

  Serial.println("Calibration done");
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  Wire.begin();

  initMPU(0);
  initMPU(1);
  initMPU(2);

  pinMode(FSR_HEEL, INPUT);
  pinMode(FSR_MID, INPUT);
  pinMode(FSR_TOE, INPUT);

  calibrate();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected");
  Serial.println(WiFi.localIP());

  webSocket.begin();
}

// ===== LOOP =====
void loop() {
  webSocket.loop();

  unsigned long currentTime = millis();

  float ax1, ay1, az1;
  float ax2, ay2, az2;
  float ax3, ay3, az3;

  readMPU(0, ax1, ay1, az1); // thigh
  readMPU(1, ax2, ay2, az2); // shank
  readMPU(2, ax3, ay3, az3); // foot

  // ===== KNEE =====
  float knee = computeAngle(ax1, ay1, az1, ax2, ay2, az2);
  knee = 180.0 - knee;

  // ===== ANKLE (FIXED) =====
  float shank_pitch = atan2(-ax2, sqrt(ay2*ay2 + az2*az2)) * 180 / PI;
  float foot_pitch  = atan2(-ax3, sqrt(ay3*ay3 + az3*az3)) * 180 / PI;

  float shank_tilt = shank_pitch - shank_offset;
  float foot_tilt  = foot_pitch  - foot_offset;

  float ankle = 90.0 + (foot_tilt - shank_tilt);

  knee  = constrain(knee, 0, 180);
  ankle = constrain(ankle, 0, 180);

  // ===== FSR =====
  int fsr_heel = analogRead(FSR_HEEL);
  int fsr_mid  = analogRead(FSR_MID);
  int fsr_toe  = analogRead(FSR_TOE);

  float heel_load = fsr_heel * 0.05;
  float mid_load  = fsr_mid  * 0.05;
  float toe_load  = fsr_toe  * 0.05;

  int threshold = 200;
  String phase = "swing";

  if (fsr_heel > threshold) phase = "heel_strike";
  else if (fsr_mid > threshold) phase = "mid_stance";
  else if (fsr_toe > threshold) phase = "toe_off";

  // ===== STEP =====
  static bool heel_prev = false;
  bool heel_now = fsr_heel > threshold;

  if (heel_now && !heel_prev) {
    steps++;
    float interval = (currentTime - lastStepTime) / 1000.0;
    if (interval > 0) cadence = 60.0 / interval;
    lastStepTime = currentTime;
  }
  heel_prev = heel_now;

  // ===== JSON =====
  String data = "{";
  data += "\"knee\":" + String(knee,2) + ",";
  data += "\"ankle\":" + String(ankle,2) + ",";
  data += "\"phase\":\"" + phase + "\",";
  data += "\"steps\":" + String(steps) + ",";
  data += "\"cadence\":" + String(cadence,2) + ",";
  data += "\"heel_load\":" + String(heel_load,2) + ",";
  data += "\"mid_load\":" + String(mid_load,2) + ",";
  data += "\"toe_load\":" + String(toe_load,2);
  data += "}";

  webSocket.broadcastTXT(data);
  Serial.println(data);

  delay(100);
}