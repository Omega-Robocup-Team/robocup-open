#include <Arduino.h>
// #include "Gyro.h"

class Motor
{
public:
    void init();
    void stop();
    void run();
    void dribble(int);
    int direction = 0, speed = 0, correction = 0;
    bool stop_flag = 0;

private:
    int MOTOR[4][2] = {{3, 2}, {5, 4}, {7, 6}, {9, 8}};
    int motor_freq = 1000;
    int DRIB[2] = {10, 11};
    int drib_freq = 1500;
};

void Motor::init()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            pinMode(MOTOR[i][j], OUTPUT);
            analogWriteFrequency(MOTOR[i][j], motor_freq);
            analogWrite(MOTOR[i][j], 0);
        }
    }
    for (int i = 0; i < 2; i++)
    {
        pinMode(DRIB[i], OUTPUT);
        analogWriteFrequency(DRIB[i], drib_freq);
        analogWrite(DRIB[i], 0);
    }
}

void Motor::stop()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            analogWrite(MOTOR[i][j], 0);
        }
    }
}

void Motor::run()
{
    if (stop_flag)
    {
        this->stop();
    }
    else
    {
        float rad = PI * (direction + 45) / 180;
        int _sp;
        int sp[4];
        sp[0] = speed * sin(rad) + correction;
        sp[3] = speed * cos(rad) + correction;
        sp[1] = speed * cos(rad) - correction;
        sp[2] = speed * sin(rad) - correction;
        for (int i = 0; i < 4; i++)
        {
            _sp = (abs(sp[i]) > 255) ? 255 : abs(sp[i]);
            if (sp[i] <= 0)
            {
                analogWrite(MOTOR[i][0], 255);
                analogWrite(MOTOR[i][1], 255 - _sp / 1);
            }
            else
            {
                analogWrite(MOTOR[i][1], 255);
                analogWrite(MOTOR[i][0], 255 - _sp / 1);
            }
        }
    }
}

void Motor::dribble(int sp)
{
    analogWrite(DRIB[1], sp);
    analogWrite(DRIB[0], 0);
}