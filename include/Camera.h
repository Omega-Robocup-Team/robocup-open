#include <Arduino.h>
#include <Wire.h>

class Camera
{
public:
  Camera();
  void init();
  void read();
};

void Camera::init()
{
  Wire.begin();
}

void Camera::read()
{
  int32_t temp = 0;
  char buff[128] = {0};

  Wire.requestFrom(0x12, 2);
  if (Wire.available() == 2)
  { // got length?

    temp = Wire.read() | (Wire.read() << 8);
    delay(1); // Give some setup time...

    Wire.requestFrom(0x12, temp);
    if (Wire.available() == temp)
    { // got full message?

      temp = 0;
      while (Wire.available())
        buff[temp++] = Wire.read();
    }
    else
    {
      while (Wire.available())
        Wire.read(); // Toss garbage bytes.
    }
  }
  else
  {
    while (Wire.available())
      Wire.read(); // Toss garbage bytes.
  }
}