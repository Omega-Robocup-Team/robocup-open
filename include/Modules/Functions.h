#pragma once 
#include <Arduino.h>
#include "TeensyThreads.h"

// experimental
ThreadWrap(Serial, SerialXtra);
#define Serial ThreadClone(SerialXtra)

int thread_id = 0, start_n = 0;
bool thread_flag = false;

uint64_t fps_tm = 0;

void start(void *func, int n = 0)
{
  if (start_n == n)
  {
    if (threads.getState(thread_id) != threads.RUNNING || !thread_flag)
    {
      thread_id = threads.addThread((ThreadFunction)func);
      thread_flag = true;
    }
  }
}

double trim(double angle)
{
  while (angle < 0)
    angle += 360;
  while (angle > 360)
    angle -= 360;
  return angle;
}

double min_angle(double first_angle, double second_angle)
{
  first_angle = trim(first_angle);
  second_angle = trim(second_angle);
  double res = min(abs(first_angle - second_angle), 360 - abs(first_angle - second_angle));
  res *= (abs(first_angle - second_angle) < 180 ? 1 : -1);
  res *= (first_angle < second_angle ? 1 : -1);
  return res;
}

int delay_fps(float fps)
{
  int res = max(0, 1000000.0 / fps - (micros() - fps_tm));
  delayMicroseconds(res);
  fps_tm = micros();
  return res;
}

void get_coord(double *x, double *y, double angle, double dist, double ref_x, double ref_y)
{
  *x = ref_x - dist * sin(angle / 180.0 * M_PI);
  *y = ref_y - dist * cos(angle / 180.0 * M_PI);
}

double get_coord_direction(double x1, double y1, double x2, double y2)
{
  return atan2(x2 - x1, y2 - y1) / M_PI * 180;
}

double get_coord_dist(double x1, double y1, double x2, double y2)
{
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}