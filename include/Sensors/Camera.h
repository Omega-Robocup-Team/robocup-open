#include <Arduino.h>
#include <SPI.h>

class Camera
{
private:
  int ss_pin = 34;

public:
  void init();
  void read();
  int data[26];
  bool update = false;
  bool flag = true;
};

void Camera::init()
{
  pinMode(ss_pin, OUTPUT);
  SPI1.begin();
  SPI1.setBitOrder(MSBFIRST);
  SPI1.setClockDivider(SPI_CLOCK_DIV16);
  SPI1.setDataMode(SPI_MODE0);
}

void Camera::read()
{
  update = false;

  if (this->flag)
  {
    digitalWrite(ss_pin, LOW);

    delay(1);
    if (SPI1.transfer(1) == 85)
    {
      for (int i = 0; i < 3; i++)
      {
        SPI1.transfer(&data[i * 3], 1);
        SPI1.transfer(&data[i * 3 + 1], 2);
        SPI1.transfer(&data[i * 3 + 2], 2);
      }
      for (int i = 0; i < 16; i++)
      {
        SPI1.transfer(&data[9 + i], 1);
      }
      SPI1.transfer(&data[25], 1);
      this->update = true;
    }
    
    digitalWrite(ss_pin, HIGH);
  }
}