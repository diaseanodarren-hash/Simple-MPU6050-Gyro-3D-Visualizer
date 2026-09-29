#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

// Accumulated angles since boot (degrees), wrapped to -180..180
float angleX = 0, angleY = 0, angleZ = 0;

// Gyro bias (rad/s)
float gyroX_offset = 0, gyroY_offset = 0, gyroZ_offset = 0;

unsigned long lastTime = 0;

// Wrap an angle into the range -180..180
float wrap180(float a) {
  while (a > 180.0) a -= 360.0;
  while (a < -180.0) a += 360.0;
  return a;
}

// Packet format (8 bytes total, little-endian):
//   0xAA 0x55 | X (int16) | Y (int16) | Z (int16)
// Angles are sent in tenths of a degree (e.g. 452 = 45.2 deg).
// NOTE: no Serial.println() debug text is used, because text bytes would
// corrupt the binary stream that the Python script is reading.
void sendAngles() {
  int16_t x = (int16_t)round(angleX * 10.0);
  int16_t y = (int16_t)round(angleY * 10.0);
  int16_t z = (int16_t)round(angleZ * 10.0);

  Serial.write(0xAA);
  Serial.write(0x55);
  Serial.write((uint8_t*)&x, 2);
  Serial.write((uint8_t*)&y, 2);
  Serial.write((uint8_t*)&z, 2);
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Wire.begin();

  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Calibrate gyro bias on all three axes - keep the sensor still!
  const int samples = 1000;
  for (int i = 0; i < samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    gyroX_offset += g.gyro.x;
    gyroY_offset += g.gyro.y;
    gyroZ_offset += g.gyro.z;
    delay(5);
  }
  gyroX_offset /= samples;
  gyroY_offset /= samples;
  gyroZ_offset /= samples;

  lastTime = millis();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  // Subtract calibrated bias, convert rad/s -> deg/s
  float gx = (g.gyro.x - gyroX_offset) * 180.0 / PI;
  float gy = (g.gyro.y - gyroY_offset) * 180.0 / PI;
  float gz = (g.gyro.z - gyroZ_offset) * 180.0 / PI;

  // Noise deadband filter for gyro stability
  if (abs(gx) > 0.1) angleX = wrap180(angleX + gx * dt);
  if (abs(gy) > 0.1) angleY = wrap180(angleY + gy * dt);
  if (abs(gz) > 0.1) angleZ = wrap180(angleZ + gz * dt);

  sendAngles();

  delay(20);
}
