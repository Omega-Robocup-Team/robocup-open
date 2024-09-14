#pragma once
#include "Modules/Functions.h"

double get_goal_angle(int goal_n = 0)
{
  switch (goal_n)
  {
  case 0: // central
    return get_coord_direction(robot.x, robot.y, 0, 115);
    break;
  case 1: // left
    return get_coord_direction(robot.x, robot.y, -180, 115);
    break;
  case 2: // right
    return get_coord_direction(robot.x, robot.y, 180, 115);
    break;
  }
}

int choose_goal(bool central = true, bool left = true, bool right = true)
{
  int res = 0;
  double central_shot = abs(min_angle(gyro.angle, get_goal_angle(0)));
  double left_shot = abs(min_angle(gyro.angle, get_goal_angle(1)));
  double right_shot = abs(min_angle(gyro.angle, get_goal_angle(2)));

  if (central && central_shot < left_shot && central_shot < right_shot)
    res = 0;
  else if (left && left_shot < central_shot && left_shot < right_shot)
    res = 1;
  else if (right && right_shot < central_shot && right_shot < left_shot)
    res = 2;

  return res;
}

void right_ricochet_attack()
{
  motor.flag = 1;
  motor.direction = 120;
  motor.correction = -150;
  motor.speed = 100;
  motor.run();
  delay(200);
  motor.speed = 0;
  motor.correction = 0;
  motor.direction = 0;
  motor.run();
  delay(1000);
  // kicker.forse_kick();
}

void forward_attack()
{ 
  robot.set_speed_limit(255);
  robot.set_turn_speed_limit(150);
  while (!emitter.is_captured_for(500))
  {
    robot.speed = ball.distance * 10;
    robot.direction = ball.angle;
    robot.angle = ball.angle;
    threads.yield();
  }

  robot.set_speed_limit(100);
  robot.set_turn_speed_limit(50);
  int chosen_goal = choose_goal();
  do
  {
    robot.move_coord(0, 30, get_goal_angle(chosen_goal));
    threads.yield();
  } while (!robot.reached_coord);

  threads.delay(1000);

  emitter.reset();
}

void return_to_center()
{
  robot.speed = 0;
  robot.angle = 0;
  // robot.set_speed_limit(100);
  // robot.set_turn_speed_limit(50);
  // while (true)
  // {
  //   robot.move_coord(0, 30, 0);
  // }
}

