#include <Arduino.h>

class Emitter
{
private:
  int pin = A17;
  uint64_t update_tm = 0;
  uint64_t first_capture_tm = 0;
public:
  void init();
  void read();
  void reset();
  bool is_captured_for(int = 0);
  bool val = 0;
};

void Emitter::init()
{
  pinMode(pin, INPUT_PULLDOWN);
}

void Emitter::read()
{
  update_tm = (analogRead(pin) < 50) ? millis() : update_tm;
  val = (millis() - update_tm) < 200 && update_tm;
  if (val && !first_capture_tm)
    first_capture_tm = millis();
  if (!val)
    first_capture_tm = 0;  
}

bool Emitter::is_captured_for(int tm = 0)
{
  if (!tm) return val;
  return (millis() - first_capture_tm) > tm && first_capture_tm;
}

void Emitter::reset()
{
  update_tm = 0;
  first_capture_tm = 0;
  val = 0;
}