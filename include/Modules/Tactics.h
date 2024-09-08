#pragma once
#include "Modules/Functions.h"

double choose_goal()
{
  double left_shot = get_coord_direction(robot.x, robot.y, -180, 115);
  double central_shot = get_coord_direction(robot.x, robot.y, 0, 115);
  double right_shot = get_coord_direction(robot.x, robot.y, 180, 115);

  if (abs(min_angle(gyro.angle, central_shot)) < abs(min_angle(gyro.angle, left_shot)) && abs(min_angle(gyro.angle, central_shot)) < abs(min_angle(gyro.angle, right_shot)))
  {
    return central_shot;
  }
  else if (abs(min_angle(gyro.angle, left_shot)) < abs(min_angle(gyro.angle, central_shot)) && abs(min_angle(gyro.angle, left_shot)) < abs(min_angle(gyro.angle, right_shot)))
  {
    return left_shot;
  }
  else
  {
    return right_shot;
  }
}