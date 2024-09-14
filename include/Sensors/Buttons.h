#include <Arduino.h>

class Buttons
{
private:
  int pin[4] = {A0, A1, A2, A3};
  uint64_t press_tm[4] = {};
public:
  bool val[4] = {0, 0, 0, 0};
  bool short_press[4] = {0, 0, 0, 0};
  bool long_press[4] = {0, 0, 0, 0};
  void init();
  void read();
  void setStop(int n);
};


void Buttons::init()
{
  for (int i = 0; i < 4; i++)
  {
    pinMode(pin[i], INPUT_PULLDOWN);
  }
}


void Buttons::read()
{
  for (int i = 0; i < 4; i++)
  {
    short_press[i] = 0;
    long_press[i] = 0;

    val[i] = digitalRead(pin[i]);

    if (val[i] && !press_tm[i])
    {
      press_tm[i] = millis();
    }
    if (!val[i] && press_tm[i])
    {
      if (millis() - press_tm[i] < 500)
        short_press[i] = 1;
      else
        long_press[i] = 1;
      press_tm[i] = 0;
    }
  }
}