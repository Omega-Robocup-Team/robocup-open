#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Gyro.h"
#include "Motor.h"
#include "Buttons.h"
#include "Line.h"

#define PINPIXELS 33
#define NUMPIXELS 8

unsigned long long tm_buf = 1000;
elapsedMillis tm;

Adafruit_NeoPixel pixels(NUMPIXELS, PINPIXELS, NEO_GRB + NEO_KHZ800);
// Gyro gyro;
Motor motor;
Buttons buttons(motor.stop_flag);
Line line(tm);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);

  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++)
    pixels.setPixelColor(i, pixels.Color(50, 0, 0));
  pixels.show();
  motor.init();
  buttons.init();
  // gyro.init();
  // for (int i = 0; i < 50; i++)
  // {
  //   digitalWrite(LED_BUILTIN, i % 2);
  //   delay(100);
  // }
  line.init();
  for (int i = 0; i < NUMPIXELS; i++)
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  pixels.show();

  motor.stop_flag = 1;
  // line.colibration(motor, gyro, buttons, pixels, tm, tm_buf);
}

void loop()
{
  // gyro.read();
  buttons.read();
  line.read();
  line.calculate();

  pixels.setPixelColor(0, motor.stop_flag ? pixels.Color(50, 0, 0) : pixels.Color(0, 50, 0));
  pixels.setPixelColor(1, line.detected ? pixels.Color(50, 50, 50) : pixels.Color(0, 50, 0));
  pixels.show();


  if (tm > tm_buf)
  {
    for (int i = 0; i < 16; i++)
    {
      Serial.print(line.val[i]);
      // Serial.print(':');
      // Serial.print(line.bin[i]);
      Serial.print("\t");
    }
    // Serial.print(line.detected);
    // Serial.print("\t");
    // Serial.print(line.x);
    // Serial.print("\t");
    // Serial.print(line.y);
    // Serial.print("\t");
    // Serial.print(line.angle);
    // Serial.print("\t");
    // Serial.print(line.distance);
    Serial.println();
    tm_buf = tm + 100;
  }

  if (!line.detected)
  {
    motor.speed = 100;
    motor.correction = 0;
    // motor.direction = -gyro.angle;
    motor.direction = 0;
  }
  else
  {
    motor.speed = 50;
    motor.direction = line.reverse;
  }

  // motor.dribble(0);
  motor.run();

  delay(10);
}