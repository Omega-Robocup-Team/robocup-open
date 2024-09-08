#define ROBOT 1 // 0 - goalkeeper, 1 - forward (номер робота)
#define GOAL 1  // 0 - blue, 1 - yellow (ворота в которые мы играем)
#define ROLE 0  // 0 - goalkeeper, 1 - forward (роль игрока)

#include <Arduino.h>
#include "Modules/Functions.h"
#include "Robot.h"
#include "Robot.cpp"
#include "Modules/Tactics.h"

void logging();

void setup()
{
  robot.init();

  robot.set_turn_speed_limit(50);
  robot.set_speed_limit(70);
}

void loop()
{
  robot.read();

  if (ROLE)
  {
    if (ball.is_visible(500))
    {
      robot.speed = 100;
      robot.direction = ball.angle;
      robot.angle = ball.angle;
    }
    else
    {
      robot.speed = 0;
      robot.direction = 0;
      robot.angle = 0;
    }
  }
  else
  {
    if (ball.is_visible(1000))
    {
      // robot.set_speed_limit(255);
      robot.move_coord(sin(ball.angle / 180.0 * M_PI) * 30, back_goal.y + 30, ball.angle);
    }
    else
    {
      // robot.set_speed_limit(100);
      robot.move_coord(0, back_goal.y + 30, 0);
    }
  }

  pixels.show();
  robot.move();

  logging();
  delay_fps(100);
}

void logging()
{
  Serial.print(back_goal.is_visible(500));
  Serial.print(" ");
  Serial.print(back_goal.angle);
  Serial.print(" ");
  Serial.print(back_goal.distance);
  Serial.print(" ");
  Serial.print(robot.x);
  Serial.print(" ");
  Serial.print(robot.y);
  Serial.print(" ");
  Serial.print(get_coord_direction(robot.x, robot.y, 0, -90));
  Serial.print(" ");
  Serial.println();
}