#include <Wire.h>

#define PCA_ADDR 0x70
#define MPU_ADDR 0x68

#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H  0x43

float angle[3] = {0,0,0};
int16_t gyro_offset[3] = {0,0,0};

unsigned long lastTime;
float dt;

float alpha = 0.98;  // complementary filter factor

// -------- PCA Select --------
void selectChannel(uint8_t ch)
{
  Wire.beginTransmission(PCA_ADDR);
  Wire.write(1 << ch);
  Wire.endTransmission();
}

// -------- Wake MPU --------
void wakeMPU(uint8_t ch)
{
  selectChannel(ch);
  delay(5);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission(true);
}

// -------- Read Accel X,Z --------
void readAccel(uint8_t ch, int16_t &ax, int16_t &az)
{
  selectChannel(ch);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, 6, true);

  ax = Wire.read()<<8 | Wire.read();
  Wire.read(); Wire.read(); // ay
  az = Wire.read()<<8 | Wire.read();
}

// -------- Read Gyro X --------
int16_t readGyroX(uint8_t ch)
{
  selectChannel(ch);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(GYRO_XOUT_H);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR,2,true);

  return Wire.read()<<8 | Wire.read();
}

// -------- Calibration --------
void calibrateGyro()
{
  Serial.println("Calibrating... Keep still");

  long sum[3] = {0,0,0};

  for(int i=0;i<1000;i++)
  {
    for(int ch=0; ch<3; ch++)
    {
      sum[ch] += readGyroX(ch);
    }
    delay(2);
  }

  for(int ch=0; ch<3; ch++)
  {
    gyro_offset[ch] = sum[ch] / 1000;
  }

  Serial.println("Done");
}

// -------- Setup --------
void setup()
{
  Serial.begin(115200);
  Wire.begin(8,9);

  for(int ch=0; ch<3; ch++)
    wakeMPU(ch);

  delay(2000);

  calibrateGyro();

  lastTime = millis();
}

// -------- Loop --------
void loop()
{
  unsigned long now = millis();
  dt = (now - lastTime) / 1000.0;
  lastTime = now;

  for(int ch=0; ch<3; ch++)
  {
    int16_t ax, az;
    readAccel(ch, ax, az);

    float accAngle = atan2(ax, az) * 180 / PI;

    int16_t gx = readGyroX(ch) - gyro_offset[ch];
    float gyroRate = gx / 131.0;

    angle[ch] = alpha * (angle[ch] + gyroRate * dt)
              + (1 - alpha) * accAngle;
  }

  float knee  = angle[1] - angle[0];
  float ankle = angle[2] - angle[1];

  Serial.print("Knee: ");
  Serial.print(knee,2);
  Serial.print("  Ankle: ");
  Serial.println(ankle,2);

  delay(10);
}