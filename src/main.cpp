#define ROBOT 1 // 0 - goalkeeper, 1 - forward (номер робота)
#define GOAL 1  // 0 - blue, 1 - yellow (ворота в которые мы играем)
#define ROLE 1  // 0 - goalkeeper, 1 - forward (роль игрока)

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Gyro.h"
#include "Motor.h"
#include "Kicker.h"
#include "Buttons.h"
#include "Line.h"
#include "Object.h"
#include "Camera.h"
#include "Emitter.h"

#define PINPIXELS 33
#define NUMPIXELS 8

unsigned long long tm_buf = 0;
elapsedMillis tm;
int main_delay = 10;
unsigned long long main_start_time = 0;

Adafruit_NeoPixel pixels(NUMPIXELS, PINPIXELS, NEO_GRB + NEO_KHZ800);
Gyro gyro;
Motor motor;
Kicker kicker;
Buttons buttons(motor, kicker);
Line line(tm);
Camera camera(tm);
Object ball(gyro, tm);
Object yellow_goal(gyro, tm);
Object blue_goal(gyro, tm);
Emitter emitter(tm);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);

  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++)
    pixels.setPixelColor(i, pixels.Color(20, 20, 20));
  pixels.show();
  motor.init();
  buttons.init();
  emitter.init();
  kicker.init();
  camera.init();
  gyro.init();
  for (int i = 0; i < 50; i++)
  {
    digitalWrite(LED_BUILTIN, i % 2);
    delay(100);
  }
  line.init();
  for (int i = 0; i < NUMPIXELS; i++)
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  pixels.show();

  // line.colibration(motor, gyro, buttons, pixels, tm, tm_buf);
  // delay(10000);

  motor.stop_flag = 1;
  motor.stop_dribble = 0;
  kicker.begin_if_null = 1;
}

void loop()
{
  main_start_time = tm;

  gyro.read();
  buttons.read();
  emitter.read();
  line.read();
  line.calculate();
  camera.read();
  kicker.handle();

  ball.decodeArray(camera.data, 0);
  yellow_goal.decodeArray(camera.data, 10);
  blue_goal.decodeArray(camera.data, 20);

  pixels.setPixelColor(0, motor.stop_flag ? pixels.Color(0, 0, 0) : pixels.Color(0, 20, 0));
  pixels.setPixelColor(1, line.detected ? pixels.Color(20, 20, 20) : pixels.Color(0, 20, 0));
  pixels.setPixelColor(2, camera.update ? (ball.visible ? pixels.Color(0, 20, 0) : pixels.Color(20, 20, 20)) : pixels.Color(0, 0, 0));
  pixels.setPixelColor(3, kicker.getChrg() > 90 ? pixels.Color(0, 20, 0) : (kicker.begin_if_null ? pixels.Color(20, 20, 20) : pixels.Color(0, 0, 0)));
  pixels.show();

  if (tm > tm_buf)
  {
    // for (int i = 0; i < 16; i++)
    // {
    //   Serial.print(line.val[i]);
    //   // Serial.print(':');
    //   // Serial.print(line.bin[i]);
    //   Serial.print("\t");
    // }
    // Serial.print(line.detected);
    // Serial.print("\t");
    // Serial.print(line.x);
    // Serial.print("\t");
    // Serial.print(line.y);
    // Serial.print("\t");
    // Serial.print(line.angle);
    // Serial.print("\t");
    // Serial.print(line.distance);
    // Serial.print(camera.buff);
    // Serial.println();

    // for (int i = 0; i < 30; i++)
    // {
    //   Serial.print(camera.data[i]);
    //   Serial.print(" ");
    // }

    Serial.print(ball.visible);
    Serial.print(" ");
    Serial.print(ball.angle);
    Serial.print(" ");
    Serial.print(ball.distance);
    Serial.print(" ");
    Serial.print(ball.abs_angle);
    Serial.print(" ");
    Serial.println(ball.turn_angle);

    tm_buf = tm + 100;
  }

  if (ROLE == 1) // forward ------------------------------------------------------------------------
  {
    Object goal = GOAL ? blue_goal : yellow_goal;
    // if (!line.detected)
    // {
    //   motor.speed = 150;
    //   motor.correction = 0;
    //   motor.direction = -gyro.angle;
    // }
    // else
    // {
    //   motor.speed = 100;
    //   motor.direction = line.reverse;
    // }

    if (emitter.captured)
    {
      if (abs(gyro.angle) < 10 && kicker.current_state == 2)
      {
        motor.direction = 0;
        motor.speed = 255;
        motor.correction = 0;
        motor.run();
        motor.dribble(0);
        delay(200);
        kicker.execute = true;
        kicker.handle();
        delay(100);
        motor.speed = 0;
        motor.run();
        motor.dribble(0);
        delay(300);
        emitter.capture_tm = 0;
      }
      else
      {
        motor.correction = constrain(gyro.turn, -30, 30);
        motor.speed = 0;
        motor.direction = 0;
        motor.run();
        motor.dribble(255);
      }
    }
    else if (ball.lst_tm_visible + 500 > tm)
    {
      motor.correction = ball.getTurn(50);
      motor.direction = ball.turn_angle;
      motor.speed = ball.distance < 60 ? 50 : 100;
      motor.dribble(ball.distance < 60 ? 255 : 0);
    }
    else
    {
      motor.correction = gyro.turn;
      motor.direction = 0;
      motor.speed = 0;
      motor.dribble(0);
    }
  }
  else // goalkeeper -------------------------------------------------------------------------------
  {
    Object goal = GOAL ? yellow_goal : blue_goal;

    // if (ball.lst_tm_visible + 500 > tm)
    // {
    //   motor.direction = ball.turn_angle > 0 ? 90 : -90;
    //   motor.speed = abs(ball.turn_angle) < 10 ? 100 : 200;
    //   motor.dribble(0);
    // }
    // else
    // {
    //   motor.direction = 0;
    //   motor.dribble(0);
    // }

    double x_speed = 0;
    double y_speed = 0;
  }

  motor.run();
  // delay(((unsigned long long)tm - main_start_time) < main_delay ? main_delay - ((unsigned long long)tm - main_start_time) : 0);
  delay(10);
}
