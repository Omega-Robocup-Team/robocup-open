bool ROBOT = 0; // 0 - goalkeeper, 1 - forward (номер робота)
bool GOAL = 1;  // 0 - blue, 1 - yellow (ворота в которые мы играем)
bool ROLE = 0;  // 0 - goalkeeper, 1 - forward (роль игрока)

#include <Arduino.h>
#include "Modules/Functions.h"
#include "Robot.h"
#include "Robot.cpp"
#include "Modules/Tactics.h"

void logging(), serial_commands();

uint64_t keeping_ball_tm = 0;
double dir = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  robot.init();

  robot.set_turn_speed_limit(100);
  robot.set_speed_limit(150);
  kicker.begin_if_null = 0;

  // line.calibrate();
}

void loop()
{
  robot.read();

  Serial.println(ball.distance);

  if (line.visible)
  {
    pixels.setPixelColor(2, pixels.Color(30, 30, 30));
    robot.direction = line.angle + 180;
    // if (abs(min_angle(line.first_angle, line.angle)) < 90)
    //   robot.direction = line.angle + 180;
    // else
    //   robot.direction = line.first_angle + 180;
    robot.speed = 70;
  }
  else if (ROLE)
  {
    pixels.setPixelColor(2, pixels.Color(0, 0, 0));
    if (ball.is_visible(500))
    {
      dribbling.speed = 0;
      robot.set_speed_limit(150);
      robot.set_turn_speed_limit(100);

      robot.speed = ball.distance * 7;
      if (abs(min_angle(gyro.angle, ball.angle)) < 30 && abs(min_angle(gyro.angle, front_goal.angle)) < 30)
      {
        dribbling.speed = 255;
        robot.angle = (min_angle(0, ball.angle) + min_angle(0, front_goal.angle)) / 2;
        robot.direction = ball.angle;
        // if (ball.distance > 9)
        // {
        //   keeping_ball_tm = 0;
        //   kicker.execute = false;
        // }
        // if (!keeping_ball_tm)
        //   keeping_ball_tm = millis();
        // if (millis() - keeping_ball_tm > 1000)
        if (emitter.val)
        {
          kicker.execute = true;
          kicker.handle();
          emitter.reset();
          for (int i = 0; i < 4; i++)
            pixels.setPixelColor(i, pixels.Color(30, 30, 30));
          pixels.show();
          delay(100);
          pixels.clear();
        }
      }
      else
      {
        robot.angle = ball.angle;
        robot.direction = max(0, 40 - ball.distance) / 25 * 90 * -get_sign(min_angle(gyro.angle, front_goal.angle)) + gyro.angle;
        keeping_ball_tm = 0;
        kicker.execute = false;
      }
    }
    else
    {
      dribbling.speed = 0;
      robot.set_speed_limit(150);
      robot.set_turn_speed_limit(50);
      robot.move_coord(0, 0, 0);
    }
  }
  else
  {
    pixels.setPixelColor(2, pixels.Color(0, 0, 0));
    double goal_dist = 50;
    if (ball.is_visible(500))
    {
      dribbling.speed = 0;
      robot.set_speed_limit(200);
      robot.set_turn_speed_limit(150);
      double angle = min_angle(0, ball.angle);
      if (abs(angle) < 60)
        robot.move_coord(constrain(tan(radians(angle)) * goal_dist, -goal_dist, goal_dist), back_goal.y + goal_dist, ball.angle);
      else
        robot.move_coord(goal_dist * get_sign(angle), back_goal.y + goal_dist, ball.angle);

      // if (ball.distance > 30) // охранять ворота
      // {
      // }
      // else // идти в атаку
      // {
      //   robot.set_speed_limit(150);
      //   robot.set_turn_speed_limit(100);
      //   if (abs(min_angle(gyro.angle, ball.angle)) < 20 && abs(min_angle(gyro.angle, front_goal.angle)) < 20)
      //   {
      //     robot.angle = (min_angle(0, ball.angle) + min_angle(0, front_goal.angle)) / 2;
      //     robot.direction = ball.angle;
      //     if (emitter.val)
      //     {
      //       kicker.execute = true;
      //       kicker.handle();
      //       emitter.reset();
      //       for (int i = 0; i < 4; i++)
      //         pixels.setPixelColor(i, pixels.Color(30, 30, 30));
      //       pixels.show();
      //       delay(100);
      //       pixels.clear();
      //     }
      //   }
      //   else
      //   {
      //     robot.angle = ball.angle;
      //     robot.direction = max(0, 30 - ball.distance) / 25 * 90 * -get_sign(min_angle(gyro.angle, front_goal.angle)) + gyro.angle;
      //     keeping_ball_tm = 0;
      //     kicker.execute = false;
      //   }
      // }
      
    }
    else
    {
      dribbling.speed = 0;
      robot.set_speed_limit(150);
      robot.set_turn_speed_limit(50);
      robot.move_coord(0, back_goal.y + goal_dist, 0);
    }
  }

  pixels.show();
  robot.move();
  dribbling.run();

  // serial_commands();
  // logging();

  delay_fps(100);
}

void serial_commands()
{
  if (Serial.available())
  {
    char command = Serial.read();
    if (command == 'calibrate')
    {
      line.calibrate();
      Serial.print("calibration_data: {");
      for (int i = 0; i < 16; i++)
      {
        Serial.print(line.calibration_data[ROBOT][i]);
        Serial.print(i != 15 ? ", " : "");
      }
      Serial.println("}");
    }
  }
}

void logging()
{
  // Serial.print(GOAL);
  // Serial.print(" ");
  // Serial.print(robot.is_coord_updated());
  // Serial.print(" ");
  // Serial.print(robot.x);
  // Serial.print(" ");
  // Serial.print(robot.y);
  // Serial.print(" ");
  // Serial.print(gyro.angle);
  // Serial.print(" ");
  // Serial.print(ball.is_visible(500));
  // Serial.print(" ");
  // Serial.print(ball.x);
  // Serial.print(" ");
  // Serial.print(ball.y);
  // Serial.print(" ");
  // Serial.print(line.is_visible(500));
  // Serial.print(" ");
  // Serial.print(line.angle);
  // Serial.print(" ");
  // Serial.print(line.distance);
  // for (int i = 0; i < 16; i++)
  // {
  //   Serial.print(" ");
  //   Serial.print(line.bin[i]);
  //   Serial.print(line.val[i]);
  // }
  // Serial.println();

  Serial.print(line.avg_value);
  Serial.print("; ");
  for (int i = 0; i < 16; i++)
  {
    Serial.print(line.bin[i]);
    Serial.print(" ");
    Serial.print(line.val[i]);
    Serial.print(" ");
    Serial.print(line.val[i] / (line.avg_value * line.calibration_data[ROBOT][i]));
    Serial.print("; ");
  }
  Serial.println();
}