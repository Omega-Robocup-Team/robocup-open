#include <Arduino.h>
#include <SPI.h>


class Camera
{
private:
  int CHAR_BUF = 128;
  int ss_pin = 34;
  int len = 0;
  int32_t temp = 0;
  elapsedMillis tm;
  unsigned long long lst_tm = 0;
  int fps = 20;
public:
  Camera (elapsedMillis &tm) : tm(tm) {}
  void init();
  void read();
  char buff[128] = {0};
  int data[30] = {0};
  bool update = false;
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
  if (lst_tm < tm)
  {
    temp = 0;
    len = 0;
    for (int i = 0; i < CHAR_BUF; i++)
      buff[i] = 0;

    digitalWrite(ss_pin, LOW);
    delay(1);

    if (SPI1.transfer(1) == 85)
    {
      SPI1.transfer(&len, 4);
      if (len)
      {
        SPI1.transfer(&buff, min(len, CHAR_BUF));
        len -= min(len, CHAR_BUF);
      }
      while (len--)
        SPI1.transfer(0);
      this->update = true;
    }

    digitalWrite(ss_pin, HIGH);

    for (int i = 0; i < 30; i++)
      data[i] = buff[i] - 48;

    lst_tm = tm + 1000 / fps;
  }
}