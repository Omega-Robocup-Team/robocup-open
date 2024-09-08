#include <Arduino.h>

class Dribbling
{
public:
  void init();
  void run();

  int speed = 0;
  bool flag = 0;

private:
  int PIN[2] = {10, 11};
  int freq = 1500;
};

void Dribbling::init()
{
  for (int i = 0; i < 2; i++)
  {
    pinMode(PIN[i], OUTPUT);
    analogWriteFrequency(PIN[i], freq);
    analogWrite(PIN[i], 0);
  }
}

void Dribbling::run()
{
  if (flag)
  {
    analogWrite(PIN[0], speed);
    analogWrite(PIN[1], 0);
  }
  else
  {
    analogWrite(PIN[0], 0);
    analogWrite(PIN[1], 0);
  }
}