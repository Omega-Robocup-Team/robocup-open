#include <Arduino.h>
#include "Modules/Functions.h"
#include "math.h"

class Object
{
private:
  int lst_id = -1;
  uint64_t lst_tm = 0;
  bool is_goal = false;
public:
  Object() {};
  Object(double x, double y): x(x), y(y), is_goal(1) {};
  void update(int, int, int, double);
  bool is_visible(int);
  void print();

  bool visible = 0;
  bool new_value = 0;
  double angle = 0;
  double distance = 0;
  double relative_angle = 0;
  double x = 0;
  double y = 0;
  bool update_coord = false;
};

void Object::update(int id, int angle, int dist, double gyro_angle)
{
  new_value = 0;
  if (id)
  {
    if (lst_id != id)
    {
      lst_id = id;
      lst_tm = millis();
      visible = 1;
      new_value = 1;
      relative_angle = angle;
      if (is_goal)
      {
        distance = constrain(pow(1.05651, dist) + 7.03967, 0, 200);
      }
      this->angle = relative_angle + gyro_angle;
      this->angle = trim(this->angle);
    }
  }
  else
  {
    visible = 0;
  }
}

bool Object::is_visible(int visible_tm = 0)
{
  if (!visible_tm) return visible;
  return ((millis() - lst_tm) < visible_tm) ? 1 : 0;
}

void Object::print()
{
  Serial.print(new_value);
  Serial.print(" ");
  Serial.print(distance);
  Serial.print(" ");
  Serial.print(relative_angle);
  Serial.print(" ");
  Serial.print(angle);
}