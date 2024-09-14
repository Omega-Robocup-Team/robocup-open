#include <Arduino.h>

class Line
{
private:
  int analog = A6;
  int digital[4] = {12, 13, 24, 25};
  int index[16] = {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
  double sensitivity[2][16] = {
      {1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5},
      {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
  };

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
  uint64_t lst_avg_update_tm = 0;

  void get_avg_value();

public:
  double x = 0, y = 0, points = 0;
  int val[16] = {};
  bool bin[16] = {};
  double angle = 0, distance = 0, first_angle = 0, relative_angle = 0;
  bool visible = 0;

  uint64_t detect_tm = 0;

  double avg_value = 0;
  double calibration_data[2][16] = {
      {1.96, 0.65, 0.63, 0.79, 0.84, 1.37, 0.69, 0.77, 1.37, 0.67, 0.84, 0.71, 1.98, 1.20, 0.71, 0.82},
      {1.96, 0.65, 0.63, 0.79, 0.84, 1.37, 0.69, 0.77, 1.37, 0.67, 0.84, 0.71, 1.98, 1.20, 0.71, 0.82},
  };

  bool first_detection = 1;

  void init();
  void read();
  void calculate(double);
  void calibrate();
  bool is_visible(int tm = 0);
};

void Line::init()
{
  for (int i = 0; i < 4; i++)
    pinMode(digital[i], OUTPUT);
  pinMode(analog, INPUT_PULLDOWN);
}

void Line::read()
{
  visible = 0;

  for (int i = 0; i < 16; i++)
  {
    for (int j = 0; j < 4; j++)
      digitalWrite(digital[j], bitRead(i, (3 - j)));
    val[index[i]] = analogRead(analog);
  }

  if (millis() - lst_avg_update_tm > 1000)
  {
    get_avg_value();
    lst_avg_update_tm = millis();
  }

  for (int i = 0; i < 16; i++)
  {
    bin[i] = (val[i] / (calibration_data[ROBOT][i] * avg_value)) > sensitivity[ROBOT][i];
    if (bin[index[i]])
      detect_tm = millis();
    visible = bin[i] ? 1 : visible;
  }
}

void Line::calculate(double angle)
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
    relative_angle = fmod(atan2(y, x) / M_PI * 180.0 * -1 + 90 + 360, 360);
    this->angle = relative_angle + angle;
    distance = sqrt(x * x + y * y);

    if (first_detection)
    {
      first_angle = this->angle;
      first_detection = 0;
    }
  }
}

void Line::calibrate()
{
  read();
  get_avg_value();

  for (int i = 0; i < 16; i++)
    calibration_data[ROBOT][i] = val[i] / avg_value;
  
  // Serial.println("")
}

void Line::get_avg_value()
{
  avg_value = 0;
  for (int i = 0; i < 16; i++)
    avg_value += val[i];
  avg_value /= 16;
}

bool Line::is_visible(int tm = 0)
{
  if (not tm)
    return visible;
  return (millis() - detect_tm) < tm;
}