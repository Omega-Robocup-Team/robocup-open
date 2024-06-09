#include <Arduino.h>

class Object
{
private:
  Gyro gyro;
public:
  Object(Gyro &gyro) : gyro(gyro) {}
  void setAngle(int angle);
  void setDistance(int distance);
  int angle = 0;
  int distance = 0;
  int abs_angle = 0;
  int abs_turn = 0;
};

void Object::setAngle(int angle)
{
  this->angle = angle;
  this->abs_angle = angle - gyro.angle;
}