#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
// #include "Buttons.h"

class Line
{
private:
  int analog = A6;
  int digital[4] = {12, 13, 24, 25};
  int index[16] = {
      0,
      8,
      4,
      12,
      2,
      10,
      6,
      14,
      1,
      9,
      5,
      13,
      3,
      11,
      7,
      15,
  };
  int grey[2][16] = {
    {422, 256, 289, 395, 322, 436, 260, 295, 380, 319, 307, 407, 431, 296, 255, 326},
    {67, 39, 38, 56, 31, 71, 47, 56, 63, 52, 50, 67, 70, 53, 46, 50}
  };

  // int grey[16] = {231, 136, 149, 213, 170, 230, 137, 169, 216, 178, 124, 120, 219, 149, 127, 151};
  double coord[16][2] = {
      {0.0, 1.0},
      {0.383, 0.924},
      {0.707, 0.707},
      {0.924, 0.383},
      {1.0, 0.0},
      {0.924, -0.383},
      {0.707, -0.707},
      {0.383, -0.924},
      {0.0, -1.0},
      {-0.383, -0.924},
      {-0.707, -0.707},
      {-0.924, -0.383},
      {-1.0, -0.0},
      {-0.924, 0.383},
      {-0.707, 0.707},
      {-0.383, 0.924},
  };
  int detect_tm = 100;
  elapsedMillis tm;
  double first_angle = 0;
  bool first_detection = 1;
  // double x = 0, y = 0, points = 0;

public:
  Line(elapsedMillis &tm) : tm(tm) {}
  double x = 0, y = 0, points = 0;
  int val[16] = {};
  bool bin[16] = {};
  double angle = 0, distance = 0, reverse = 0;
  bool detected = 0;
  void init();
  void read();
  void calculate();
  void colibration(Motor &, Gyro &, Buttons &, Adafruit_NeoPixel &, elapsedMillis &, unsigned long long &);
};

void Line::init()
{
  for (int i = 0; i < 4; i++)
    pinMode(digital[i], OUTPUT);
  pinMode(analog, INPUT_PULLDOWN);
}

void Line::read()
{
  for (int i = 0; i < 16; i++)
  {
    for (int j = 0; j < 4; j++)
      digitalWrite(digital[j], bitRead(i, (3 - j)));
    val[index[i]] = analogRead(analog);
    bin[index[i]] = val[index[i]] > grey[ROBOT][index[i]];
    if (bin[index[i]])
      detect_tm = tm + 100;
  }
  detected = tm < detect_tm;
}

void Line::calculate()
{
  if (detected)
  {
    x = 0;
    y = 0;
    points = 0;
    for (int i = 0; i < 16; i++)
    {
      if (bin[i])
      {
        x += coord[i][0];
        y += coord[i][1];
        points++;
      }
    }
    if (points)
    {
      x /= points;
      y /= points;
      angle = fmod(atan2(y, x) / M_PI * 180.0 * -1 + 90 + 360, 360);
      distance = sqrt(x * x + y * y);

      if (first_detection)
      {
        first_angle = angle;
        first_detection = 0;
      }
      reverse = first_angle + 180;
    }
  }
  else
  {
    x = 0;
    y = 0;
    points = 0;
    angle = 0;
    distance = 0;
    reverse = 0;
    first_detection = 1;
  }
}

void Line::colibration(Motor &motor, Gyro &gyro, Buttons &buttons, Adafruit_NeoPixel &pixels, elapsedMillis &tm, unsigned long long &tm_buf)
{
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
  while (!buttons.press[0])
  {
    buttons.read();
    delay(10);
  }
  buttons.read();
  int max_val[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int min_val[16] = {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024};
  motor.direction = 90;
  motor.stop_flag = 0;
  while (!buttons.press[0])
  {
    read();

    if (tm > tm_buf)
    {
      motor.direction *= -1;
      tm_buf = tm + 1000;
    }

    pixels.setPixelColor(0, pixels.Color(0, 0, 0));

    for (int i = 0; i < 16; i++)
    {
      if (max_val[i] < val[i])
      {
        max_val[i] = val[i];
        pixels.setPixelColor(0, pixels.Color(100, 100, 100));
      }
      if (min_val[i] > val[i])
      {
        min_val[i] = val[i];
        pixels.setPixelColor(0, pixels.Color(100, 100, 100));
      }
      grey[ROBOT][i] = (max_val[i] + min_val[i]) / 2.0;
    }

    Serial.print("grey = {");
    for (int i = 0; i < 16; i++)
    {
      Serial.print(grey[ROBOT][i]);
      Serial.print(i == 16 ? "" : ", ");
    }
    Serial.println("};");

    Serial.print("green = {");
    double avg_green = 0;
    for (int i = 0; i < 16; i++)
    {
      Serial.print(min_val[i]);
      Serial.print(i == 16 ? "" : ", ");
      avg_green += min_val[i];
    }
    avg_green /= 16.0;
    Serial.println("};");

    Serial.print("avg_green = ");
    Serial.println(avg_green);

    Serial.print("white = {");
    for (int i = 0; i < 16; i++)
    {
      Serial.print(max_val[i]);
      Serial.print(i == 16 ? "" : ", ");
    }
    Serial.println("};");

    pixels.show();
    buttons.read();

    motor.speed = 50;
    motor.correction = gyro.turn;
    motor.run();

    delay(10);
  }
  motor.stop();
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
}