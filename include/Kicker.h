#include <Arduino.h>

class Kicker
{
private:
  int fdbk_pin = 38; // уровень заряда конденсатора, ~400 - полный заряд
  int chrg_pin = 37;
  int kick_pin = 36;
  int retr_pin = 35;
  uint32_t retr_timer = 0;
  uint32_t kick_timer = 0;
  int kick_delay = 100;
  int retr_delay = 2000;
  float full_chrg_value = 500.;
  uint32_t delay_timer = 0;

public:
  Kicker() {}
  void init();
  float getChrg();
  void handle();
  void forse_kick();
  int current_state = 0; // 0 - Ничего не делаем, 1 - зарядка, 2 - заряжен (> 95 процентов), 3 - бьет, 4 - оттягивает
  bool execute = false;  // если true - то удар будет произведен по готовности, после удара станет false
  bool begin_if_null = false;
};

void Kicker::init()
{
  pinMode(fdbk_pin, INPUT);
  pinMode(chrg_pin, OUTPUT);
  pinMode(kick_pin, OUTPUT);
  pinMode(retr_pin, OUTPUT);
}

float Kicker::getChrg()
{
  return (analogRead(fdbk_pin) / full_chrg_value) * 100;
}

void Kicker::handle() // если цикл удара был поставлен на паузу ранее (значение current_state 0), то
{                     // при begin_if_null == true цикл продолжится
  if (current_state == 0 && begin_if_null)
  {
    current_state = 1;
  }
  if (current_state == 1)
  {
    digitalWrite(kick_pin, 0);
    digitalWrite(retr_pin, 0);
    digitalWrite(chrg_pin, 1);
    if (getChrg() > 90)
    {
      // Serial.println("AAAA");
      current_state = 2;
    }
  }
  if (current_state == 2)
  {
    // Serial.println("AAAA");
    digitalWrite(kick_pin, 0);
    digitalWrite(retr_pin, 0);
    digitalWrite(chrg_pin, 0);
  }
  if ((current_state == 2) && (getChrg() < 90))
  {
    Serial.println("aoaooa");
    current_state = 1;
  }

  if (execute && current_state == 2)
  {
    current_state = 3;
    kick_timer = millis() + kick_delay;
  }

  if (current_state == 3 && kick_timer > millis())
  {
    digitalWrite(chrg_pin, 0);
    digitalWrite(retr_pin, 0);
    digitalWrite(kick_pin, 1);
  }
  else if (current_state == 3 && kick_timer < millis())
  {
    digitalWrite(kick_pin, 0);
    digitalWrite(chrg_pin, 0);
    digitalWrite(retr_pin, 0);
    current_state = 4;
    execute = 0;
    delay_timer = millis() + 500;
  }
  if (current_state == 4 && delay_timer < millis())
  {
    retr_timer = millis() + retr_delay;
    current_state = 5;
  }

  if (current_state == 5 && retr_timer > millis())
  {
    digitalWrite(chrg_pin, 0);
    digitalWrite(kick_pin, 0);
    digitalWrite(retr_pin, 1);
  }
  else if (current_state == 5 && retr_timer < millis())
  {
    digitalWrite(kick_pin, 0);
    digitalWrite(chrg_pin, 0);
    digitalWrite(retr_pin, 0);
    current_state = 6;
    delay_timer = millis() + 500;
  }

  if (current_state == 6 && delay_timer < millis())
  {
    current_state = 1;
  }
}

void Kicker::forse_kick()
{
  digitalWrite(chrg_pin, 0);
  digitalWrite(retr_pin, 0);
  delay(300);
  digitalWrite(kick_pin, 1);
  delay(300);
  digitalWrite(kick_pin, 0);
}