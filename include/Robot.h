#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "Sensors/Gyro.h"
#include "Sensors/Motor.h"
#include "Sensors/Buttons.h"
#include "Sensors/Camera.h"
#include "Sensors/Dribbling.h"
#include "Modules/Object.h"

Adafruit_NeoPixel pixels(4, 33, NEO_GRB + NEO_KHZ800);
Gyro gyro;
Motor motor;
Dribbling dribbling;
Buttons buttons;
Camera camera;

Object ball;
Object front_goal(0, 115);
Object back_goal(0, -115);

class Robot
{
public:
  void init();
  void read();
  void move();

  void move_coord(double, double, double);

  void set_speed_limit(double);
  void cancel_speed_limit();
  void set_turn_speed_limit(double);
  void cancel_turn_speed_limit();
  void set_direction_limit(double);
  void cancel_direction_limit();

  double direction = 0;
  double speed = 0;
  double angle = 0;

  double x = 0, y = 0;
  bool update_coord = false;

private:
  int num_pixels = 4;
  int pin_pixels = 33;

  double direction_limit = 0;
  bool direction_limit_flag = 0;
  double speed_limit = 0;
  bool speed_limit_flag = 0;
  double turn_speed_limit = 0;
  bool turn_speed_limit_flag = 0;

  double lst_relative_angle = 0;
  double relative_angle = 0;
};

Robot robot;