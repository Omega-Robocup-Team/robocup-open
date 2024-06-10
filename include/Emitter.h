#include <Arduino.h>

class Emitter
{
private:
  int pin = A17;
  elapsedMillis &tm;
  unsigned long long first_capture_tm = 0;
  bool first_capture = 1;
public:
  Emitter(elapsedMillis &tm) : tm(tm) {}
  void init();
  void read();
  bool state = 0;
  bool captured = 0;
  bool ready_for_turn = 0;
  unsigned long long capture_tm = 0;
};

void Emitter::init()
{
  pinMode(pin, INPUT);
}

void Emitter::read()
{
  if (analogRead(pin) > 1000)
    capture_tm = tm + 200;
  captured = capture_tm > tm;
  if (captured && first_capture)
  {
    first_capture_tm = tm;
    first_capture = 0;
  }
  if (captured && first_capture_tm + 1000 < tm)
    ready_for_turn = 1;
  else
    ready_for_turn = 0;
  
  if (!captured)
    first_capture = 1;
}
