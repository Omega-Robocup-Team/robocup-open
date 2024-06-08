#include <Arduino.h>

class Buttons
{
private:
  int pin[4] = {A0, A1, A2, A3};
  bool press_flag[4] = {0, 0, 0, 0};
  bool stop[4] = {1, 0, 0, 0};
  bool &motor_stop;
public:
  Buttons(bool &motor_stop): motor_stop(motor_stop) {}
  bool val[4] = {0, 0, 0, 0};
  bool press[4] = {0, 0, 0, 0};
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
    val[i] = digitalRead(pin[i]);

    if (val[i])
    {
      if (!press_flag[i])
      {
        press_flag[i] = 1;
        press[i] = 1;
      }
      else
      {
        press[i] = 0;
      }
    }
    else
    {
      press_flag[i] = 0;
      press[i] = 0;
    }

    if (press[i])
    {
      motor_stop = !motor_stop;
    }
  }
}