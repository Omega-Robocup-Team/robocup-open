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
  double angle = 0, lst_angle = 0, turn = 0;
  void init();
  void read();
  double get_abs_angle(double);
  double get_abs_turn(double, int);
};

void Gyro::init()
{
  Serial.println("Initialize Wire");
  Wire.begin();
  Serial.println("Initialize MPU6050");
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
    lst_angle = angle;
  }
}

void Gyro::get_abs_angle(double angle_data)
{
  return angle_data - angle;
}

double Gyro::get_turn(int max_speed = 150)
{
  turn = constrain(angle * 2 + (angle - lst_angle) * 1, -max_speed, max_speed) * -1;
}