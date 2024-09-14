#include "Modules/Functions.h"

void Robot::init()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);

  pixels.begin();
  pixels.clear();

  // bluetooth.init();
  gyro.init();
  motor.init();
  buttons.init();
  emitter.init();
  camera.init();
  dribbling.init();
  kicker.init();

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
  line.read();
  emitter.read();
  // bluetooth.read();
  kicker.handle();

  // calculate line angle and distance
  if (line.visible)
    line.calculate(gyro.angle);
  
  if (line.is_visible(500))
  {
    on_line = true;
  }
  else
  {
    line.first_detection = 1;
    on_line = false;
  }

  // update ball and goals data
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

  // update robot coord
  if (front_goal.visible && back_goal.visible)
  {
    // double front_a1 = min_angle(front_goal.angle, 0);
    // double back_a2 = min_angle(180, back_goal.angle);
    // double sin_a1 = sin(radians(front_a1));
    // double sin_a2 = sin(radians(back_a2));
    // double side_x1 = sqrt(230.0 * 230.0 / (pow(sin_a2, 2) / pow(sin_a1, 2) + 1 - 2 * sin_a2 / sin_a1 * cos(radians(180 - front_a1 - back_a2))));
    // x = sin(radians(back_a2)) * side_x1;
    if (front_goal.distance < back_goal.distance)
    {
      x = get_x_coord(front_goal.angle, front_goal.distance, front_goal.x, front_goal.y);
      y = get_y_coord(front_goal.angle, front_goal.distance, front_goal.x, front_goal.y);
    }
    else
    {
      x = get_x_coord(back_goal.angle, back_goal.distance, back_goal.x, back_goal.y);
      y = get_y_coord(back_goal.angle, back_goal.distance, back_goal.x, back_goal.y);
    }

    // Serial.print("both\t");
  }
  else if (front_goal.visible)
  {
    x = get_x_coord(front_goal.angle, front_goal.distance, front_goal.x, front_goal.y);
    y = get_y_coord(front_goal.angle, front_goal.distance, front_goal.x, front_goal.y);

    // Serial.print("front\t");
  }
  else if (back_goal.visible)
  {
    x = get_x_coord(back_goal.angle, back_goal.distance, back_goal.x, back_goal.y);
    y = get_y_coord(back_goal.angle, back_goal.distance, back_goal.x, back_goal.y);

    // Serial.print("back\t");
  }

  if (front_goal.visible || back_goal.visible)
  {
    update_tm = millis();
    update_coord = true;
  }
  else
  {
    // Serial.print("none\t");
  }

  // Serial.print(robot.x);
  // Serial.print("\t");
  // Serial.print(robot.y);
  // Serial.print("\t");
  // Serial.print(back_goal.angle);
  // Serial.print("\t");
  // Serial.print(back_goal.distance);
  // Serial.print("\t");
  // Serial.print(front_goal.angle);
  // Serial.print("\t");
  // Serial.print(front_goal.distance);
  // Serial.println();

  // update ball coord
  if (ball.visible)
  {
    ball.x = get_x_coord(ball.angle, ball.distance, x, y);
    ball.y = get_y_coord(ball.angle, ball.distance, x, y);
    ball.update_coord = true;
  }
  else
  {
    ball.update_coord = false;
  }

  line.visible = false;
  if (camera.update)
  {
    for (int i = 0; i < 16; i++)
    {
      line.bin[i] = camera.data[9 + i];
      if (line.bin[i])
      {
        line.visible = true;
        line.detect_tm = millis();
      }
    }
  }

  if (line.visible)
  {
    line.calculate(gyro.angle);
  }

  // if (bluetooth.new_data)
  // {
  //   robot2.role = bluetooth.data[0];
  //   robot2.x = bluetooth.data[1];
  //   robot2.y = bluetooth.data[2];
  // }

  // bluetooth.send_data({ROLE, robot.x, robot.y}, 3);

  motor.flag = buttons.short_press[0] ? !motor.flag : motor.flag;
  dribbling.flag = buttons.short_press[0] ? !dribbling.flag : dribbling.flag;
  camera.flag = buttons.short_press[1] ? !camera.flag : camera.flag;
  kicker.execute = buttons.short_press[3];

  if (buttons.long_press[3])
  {
    if (kicker.begin_if_null)
    {
      kicker.forse_kick();
      kicker.begin_if_null = 0;
      kicker.current_state = 0;
    }
    else
    {
      kicker.begin_if_null = 1;
    }
  }

  pixels.setPixelColor(0, motor.flag ? pixels.Color(0, 30, 0) : pixels.Color(0, 0, 0));
  pixels.setPixelColor(1, camera.update ? pixels.Color(0, 30, 0) : pixels.Color(0, 0, 0));
  pixels.setPixelColor(3, kicker.getChrg() > 30 ? pixels.Color(0, 30, 0) : pixels.Color(0, 0, 0));
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
  // this->speed = this->speed < 30 ? 0 : this->speed;
  this->angle = angle;
  this->reached_coord = get_coord_dist(this->x, this->y, x, y) < 5 && abs(min_angle(gyro.angle, this->angle)) < 5;
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

bool Robot::is_coord_updated(int tm = 0)
{
  if (not tm) return update_coord;
  return (millis() - update_tm) < tm;
}