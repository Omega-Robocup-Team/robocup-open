#define ROBOT 1

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Gyro.h"
#include "Motor.h"
#include "Buttons.h"
#include "Line.h"
#include "Object.h"
#include "Camera.h"

#define PINPIXELS 33
#define NUMPIXELS 8

unsigned long long tm_buf = 0;
elapsedMillis tm;
int main_delay = 10;
unsigned long long main_start_time = 0;

Adafruit_NeoPixel pixels(NUMPIXELS, PINPIXELS, NEO_GRB + NEO_KHZ800);
Gyro gyro;
Motor motor;
Buttons buttons(motor.stop_flag);
Line line(tm);
Camera camera(tm);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);

  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++)
    pixels.setPixelColor(i, pixels.Color(50, 50, 50));
  pixels.show();
  motor.init();
  buttons.init();
  camera.init();
  delay(1000);
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

  // line.colibration(motor, gyro, buttons, pixels, tm, tm_buf);
  // delay(10000);

  motor.stop_flag = 1;
}

void loop()
{
  main_start_time = tm;

  gyro.read();
  buttons.read();
  line.read();
  line.calculate();
  camera.read();

  pixels.setPixelColor(0, motor.stop_flag ? pixels.Color(0, 0, 0) : pixels.Color(0, 50, 0));
  pixels.setPixelColor(1, line.detected ? pixels.Color(50, 50, 50) : pixels.Color(0, 50, 0));
  pixels.setPixelColor(2, camera.update ? pixels.Color(0, 50, 0) : pixels.Color(0, 0, 0));
  pixels.show();


  if (tm > tm_buf)
  {
    // for (int i = 0; i < 16; i++)
    // {
    //   Serial.print(line.val[i]);
    //   // Serial.print(':');
    //   // Serial.print(line.bin[i]);
    //   Serial.print("\t");
    // }
    // Serial.print(line.detected);
    // Serial.print("\t");
    // Serial.print(line.x);
    // Serial.print("\t");
    // Serial.print(line.y);
    // Serial.print("\t");
    // Serial.print(line.angle);
    // Serial.print("\t");
    // Serial.print(line.distance);
    Serial.print(camera.buff);
    Serial.println();

    Serial.print(camera.ball_visible);
    Serial.print(' ');
    Serial.print(camera.ball_angle);
    Serial.print(' ');
    Serial.println(camera.ball_dist);
    Serial.println();
    tm_buf = tm + 100;
  }

  // if (!line.detected)
  // {
  //   motor.speed = 150;
  //   motor.correction = 0;
  //   motor.direction = -gyro.angle;
  // }
  // else
  // {
  //   motor.speed = 100;
  //   motor.direction = line.reverse;
  // }

  motor.dribble(0);
  motor.run();

  // delay(((unsigned long long)tm - main_start_time) < main_delay ? main_delay - ((unsigned long long)tm - main_start_time) : 0);
  delay(10);
}

// #include <SPI.h>
// #define SS_PIN 34
// #define BAUD_RATE 19200
// #define CHAR_BUF 128

// void setup()
// {
//    pinMode(SS_PIN, OUTPUT);
//   pinMode(LED_BUILTIN, OUTPUT);
//   digitalWrite(LED_BUILTIN, HIGH);
//   Serial.begin(BAUD_RATE);
//   SPI1.begin();
//   SPI1.setBitOrder(MSBFIRST);
//   SPI1.setClockDivider(SPI_CLOCK_DIV16);
//   SPI1.setDataMode(SPI_MODE0);
//   delay(1000); // Give the OpenMV Cam time to bootup.
// }

// void loop()
// {
//   int32_t len = 0;
//   char buff[CHAR_BUF] = {0};
//    digitalWrite(SS_PIN, LOW);
//   delay(1); // Give the OpenMV Cam some time to setup to send data.

//   if (SPI1.transfer(1) == 85)
//   {                         // saw sync char?
//     SPI1.transfer(&len, 4); // get length
//     if (len)
//     {
//       SPI1.transfer(&buff, min(len, CHAR_BUF));
//       len -= min(len, CHAR_BUF);
//     }
//     while (len--)
//       SPI1.transfer(0); // eat any remaining bytes
//   }

//    digitalWrite(SS_PIN, HIGH);
//   Serial.println(buff);
//   delay(1); // Don't loop to quickly.
// }
