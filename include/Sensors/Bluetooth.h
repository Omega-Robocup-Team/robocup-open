#include <Arduino.h>

class Bluetooth
{
public:
  void init();
  void read();
  void send(String msg);
  void send_data(int *, int);
  bool is_available(int = 0);
  bool available = false;
  bool new_data = false;
  String str_data;
  int data[10] = {};
private:
  void split(String, char, int[]);
  int pins[2] = {0, 1};
  uint64_t update_tm = 0;
};

void Bluetooth::init()
{
  Serial1.begin(9600);
  Serial1.setRX(pins[0]);
  Serial1.setTX(pins[1]);
}

void Bluetooth::read()
{
  if (Serial1.available())
  {
    available = true;
    update_tm = millis();
    str_data = Serial1.read();
    // Serial.println(str_data);
    new_data = false;
    if (str_data.startsWith("DATA "))
    {
      str_data.remove(0, 5);
      str_data.trim();
      split(str_data, ' ', data);
      new_data = true;
    }
  }
  else
  {
    available = false;
  }
}

void Bluetooth::send(String msg)
{
  Serial1.print(msg);
}

void Bluetooth::send_data(int* data, int size)
{
  Serial1.print("DATA ");
  for (int i = 0; i < size; i++)
  {
    Serial1.print(data[i]);
    Serial1.print(i == size - 1 ? "" : " ");
  }
}

bool Bluetooth::is_available(int tm = 0)
{
  if (!tm) return available;
  return (millis() - update_tm) < tm;
}

void Bluetooth::split(String str, char delimiter, int arr[])
{
  int index = 0;
  String part;
  while (str.indexOf(delimiter) != -1)
  {
    part = str.substring(0, str.indexOf(delimiter));
    arr[index] = part.toInt();
    str.remove(0, str.indexOf(delimiter) + 1);
    index++;
  }
  arr[index] = str.toInt();
}