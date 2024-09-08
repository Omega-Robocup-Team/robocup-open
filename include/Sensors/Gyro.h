#include <Arduino.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <Wire.h>

class Gyro
{
private:
  MPU6050 *mpu = new MPU6050(0x68, &Wire);
  uint8_t fifoBuffer[64];
  Quaternion q;
  VectorFloat gravity;
  float ypr[3];

public:
  double angle = 0;
  void init();
  void read();
};

void Gyro::init()
{
  Wire.begin();
  mpu->initialize();
  mpu->dmpInitialize();
  mpu->CalibrateGyro(6);
  mpu->setDMPEnabled(true);
}

void Gyro::read()
{
  if (mpu->dmpGetCurrentFIFOPacket(fifoBuffer))
  {
    mpu->dmpGetQuaternion(&q, fifoBuffer);
    mpu->dmpGetYawPitchRoll(ypr, &q, &gravity);
    angle = ypr[0] * 180 / M_PI;
  }
}