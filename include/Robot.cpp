#include "Modules/Functions.h"

void Robot::init()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
  gyro.init();
  motor.init();

  buttons.init();
  pixels.begin();
  camera.init();

  pixels.clear();
  delay(1000);
  for (int i = 0; i < 4; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0, 30, 0));
    pixels.show();
    delay(1000);
  }
  pixels.clear();

}

void Robot::read()
{
  gyro.read();
  buttons.read();
  camera.read();

  if (camera.update)
  {
    ball.update(camera.data[0], camera.data[1], camera.data[2], gyro.angle);
    if (GOAL)
    {
      front_goal.update(camera.data[3], camera.data[4], camera.data[5], gyro.angle);
      back_goal.update(camera.data[6], camera.data[7], camera.data[8], gyro.angle);
    }
    else
    {
      back_goal.update(camera.data[3], camera.data[4], camera.data[5], gyro.angle);
      front_goal.update(camera.data[6], camera.data[7], camera.data[8], gyro.angle);
    }
  }

  if (front_goal.visible || back_goal.visible)
  {
    if (front_goal.distance * front_goal.visible > back_goal.distance * back_goal.visible)
      get_coord(&x, &y, front_goal.angle, front_goal.distance, front_goal.x, front_goal.y);
    else
      get_coord(&x, &y, back_goal.angle, back_goal.distance, back_goal.x, back_goal.y);
    update_coord = true;
  }
  else
  {
    update_coord = false;
  }

  if (ball.visible)
  {
    get_coord(&ball.x, &ball.y, ball.angle + 180, ball.distance, x, y);
    ball.update_coord = true;
  }
  else
  {
    ball.update_coord = false;
  }

  motor.flag = buttons.press[0] ? !motor.flag : motor.flag;
  dribbling.flag = buttons.press[0] ? !dribbling.flag : dribbling.flag;
  camera.flag = buttons.press[1] ? !camera.flag : camera.flag;

  pixels.setPixelColor(0, motor.flag ? pixels.Color(0, 30, 0) : pixels.Color(30, 0, 0));
  pixels.setPixelColor(1, camera.update ? pixels.Color(30, 30, 30) : pixels.Color(0, 0, 0));
}

void Robot::move()
{
  motor.direction = this->direction - gyro.angle;
  motor.speed = this->speed;
  motor.speed = this->speed_limit_flag ? constrain(motor.speed, -this->speed_limit, this->speed_limit) : motor.speed;
  this->relative_angle = min_angle(gyro.angle, this->angle);
  motor.correction = relative_angle * 2 + (this->relative_angle - this->lst_relative_angle) * 1;
  motor.correction = this->turn_speed_limit_flag ? constrain(motor.correction, -this->turn_speed_limit, this->turn_speed_limit) : motor.correction;
  motor.run();
  this->lst_relative_angle = this->relative_angle;
}

void Robot::move_coord(double x, double y, double angle)
{
  this->direction = get_coord_direction(this->x, this->y, x, y);
  this->speed = get_coord_dist(this->x, this->y, x, y) * 10;
  this->speed = this->speed < 30 ? 0 : this->speed;
  this->angle = angle;
}

void Robot::set_speed_limit(double speed_limit)
{
  this->speed_limit = speed_limit;
  this->speed_limit_flag = true;
}

void Robot::cancel_speed_limit()
{
  this->speed_limit = 0;
  this->speed_limit_flag = false;
}

void Robot::set_turn_speed_limit(double turn_speed_limit)
{
  this->turn_speed_limit = turn_speed_limit;
  this->turn_speed_limit_flag = true;
}

void Robot::cancel_turn_speed_limit()
{
  this->turn_speed_limit = 0;
  this->turn_speed_limit_flag = false;
}

void Robot::set_direction_limit(double direction_limit)
{
  this->direction_limit = direction_limit;
  this->direction_limit_flag = true;
}

void Robot::cancel_direction_limit()
{
  this->direction_limit = 0;
  this->direction_limit_flag = false;
}