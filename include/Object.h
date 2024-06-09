#include <Arduino.h>

class Object
{
private:
  Gyro gyro;
  double lst_angle = 0;
  double lst_abs_angle = 0;
  double lst_turn_angle = 0;
  double trim(double);
  double min_angle(double, double);
public:
  Object(Gyro &gyro) : gyro(gyro) {}
  void decodeArray(int data[30], int);
  void setNewPosition(double, double);
  double getTurn(int);
  bool visible = 0;
  double angle = 0;
  double distance = 0;
  double abs_angle = 0;
  double turn_angle = 0;
  double turn = 0;
};

double Object::trim(double new_angle)
{
  while (new_angle < 0)
    new_angle += 360;
  while (new_angle > 360)
    new_angle -= 360;
  return new_angle;
}

double Object::min_angle(double first_angle, double second_angle)
{
  first_angle = trim(first_angle);
  second_angle = trim(second_angle);
  return min(abs(first_angle - second_angle), 360 - abs(first_angle - second_angle)) * (abs(first_angle - second_angle) < 180 ? 1 : -1);
}

void Object::decodeArray(int data[30], int start)
{
  this->visible = data[start] == 1 ? 1 : 0;
  double new_angle = data[start + 2] * 100 + data[start + 3] * 10 + data[start + 4];
  double new_distance = data[start + 6] * 100 + data[start + 7] * 10 + data[start + 8];
  if (this->visible)
    this->setNewPosition(new_angle, new_distance);
}

void Object::setNewPosition(double new_angle, double new_distance)
{
  this->lst_angle = this->angle;
  this->lst_abs_angle = this->abs_angle;
  this->lst_turn_angle = this->turn_angle;

  this->angle = new_angle;
  this->distance = new_distance;
  this->abs_angle = new_angle - gyro.angle;
  this->turn_angle = min_angle(this->abs_angle, gyro.angle);
}

double Object::getTurn(int max_speed = 150)
{
  this->turn = this->turn_angle * 2 + (this->turn_angle - this->lst_turn_angle) * 1;
  this->turn = constrain(this->turn, -max_speed, max_speed);

  return this->turn;
}