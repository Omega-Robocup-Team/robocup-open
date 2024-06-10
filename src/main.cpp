#define ROBOT 1

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Gyro.h"
#include "Motor.h"
#include "Kicker.h"
#include "Buttons.h"
#include "Line.h"
#include "Object.h"
#include "Camera.h"
#include "Emitter.h"

#define PINPIXELS 33
#define NUMPIXELS 8

unsigned long long tm_buf = 0;
elapsedMillis tm;
int main_delay = 10;
unsigned long long main_start_time = 0;

Adafruit_NeoPixel pixels(NUMPIXELS, PINPIXELS, NEO_GRB + NEO_KHZ800);
Gyro gyro;
Motor motor;
Kicker kicker;
Buttons buttons(motor, kicker);
Line line(tm);
Camera camera(tm);
Object ball(gyro, tm);
Object yellow_goal(gyro, tm);
Object blue_goal(gyro, tm);
Emitter emitter(tm);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);

  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++)
    pixels.setPixelColor(i, pixels.Color(20, 20, 20));
  pixels.show();
  motor.init();
  buttons.init();
  emitter.init();
  kicker.init();
  camera.init();
  gyro.init();
  for (int i = 0; i < 50; i++)
  {
    digitalWrite(LED_BUILTIN, i % 2);
    delay(100);
  }
  line.init();
  for (int i = 0; i < NUMPIXELS; i++)
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  pixels.show();

  // line.colibration(motor, gyro, buttons, pixels, tm, tm_buf);
  // delay(10000);

  motor.stop_flag = 1;
  motor.stop_dribble = 1;
}

void loop()
{
  main_start_time = tm;

  gyro.read();
  buttons.read();
  emitter.read();
  line.read();
  line.calculate();
  camera.read();
  kicker.handle();

  ball.decodeArray(camera.data, 0);
  yellow_goal.decodeArray(camera.data, 10);

  pixels.setPixelColor(0, motor.stop_flag ? pixels.Color(0, 0, 0) : pixels.Color(0, 20, 0));
  pixels.setPixelColor(1, line.detected ? pixels.Color(20, 20, 20) : pixels.Color(0, 20, 0));
  pixels.setPixelColor(2, camera.update ? (ball.visible ? pixels.Color(0, 20, 0) : pixels.Color(20, 20, 20)) : pixels.Color(0, 0, 0));
  pixels.setPixelColor(3, kicker.getChrg() > 90 ? pixels.Color(0, 20, 0) : (kicker.begin_if_null ? pixels.Color(20, 20, 20) : pixels.Color(0, 0, 0)));
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
    // Serial.print(camera.buff);
    // Serial.println();

    // for (int i = 0; i < 30; i++)
    // {
    //   Serial.print(camera.data[i]);
    //   Serial.print(" ");
    // }

    Serial.print(ball.visible);
    Serial.print(" ");
    Serial.print(ball.angle);
    Serial.print(" ");
    Serial.print(ball.distance);
    Serial.print(" ");
    Serial.print(ball.abs_angle);
    Serial.print(" ");
    Serial.println(ball.turn_angle);

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

  if (emitter.captured)
  {
    if (abs(gyro.angle) < 10 && kicker.current_state == 2)
    {
      motor.direction = 0;
      motor.speed = 255;
      motor.correction = 0;
      motor.run();
      motor.dribble(0);
      delay(300);
      kicker.execute = true;
      kicker.handle();
      motor.speed = 0;
      motor.run();
      delay(300);
      emitter.capture_tm = 0;
    }
    else
    {
      motor.correction = constrain(gyro.turn, -30, 30);
      motor.speed = 0;
      motor.direction = 0;
      motor.run();
      motor.dribble(255);
    }
  }
  else if (ball.lst_tm_visible + 500 > tm)
  {
    motor.correction = ball.getTurn(50);
    motor.direction = ball.turn_angle;
    motor.speed = ball.distance < 60 ? 50 : 100;
    motor.dribble(ball.distance < 60 ? 255 : 0);
  }
  else
  {
    motor.correction = gyro.turn;
    motor.direction = 0;
    motor.speed = 0;
    motor.dribble(0);
  }

  // motor.correction = yellow_goal.getTurn(50);

  // motor.dribble(ball.distance < 150 ? 255 : 0);
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

  // if (SPI1.transfer(1) == 85)
  // {                         // saw sync char?
  //   SPI1.transfer(&len, 4); // get length
  //   if (len)
  //   {
  //     SPI1.transfer(&buff, min(len, CHAR_BUF));
  //     len -= min(len, CHAR_BUF);
  //   }
  //   while (len--)
  //     SPI1.transfer(0); // eat any remaining bytes
  // }

//    digitalWrite(SS_PIN, HIGH);
//   Serial.println(buff);
//   delay(1); // Don't loop to quickly.
// }
