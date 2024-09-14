#include <Arduino.h>

class Motor
{
public:
    void init();
    void run();
    int direction = 0, speed = 0, correction = 0;
    bool flag = 0;

private:
    int PIN[2][4][2] = {{{3, 2}, {5, 4}, {7, 6}, {9, 8}}, {{2, 3}, {4, 5}, {7, 6}, {9, 8}}};
    int freq = 1000;
};

void Motor::init()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            pinMode(PIN[ROBOT][i][j], OUTPUT);
            analogWriteFrequency(PIN[ROBOT][i][j], freq);
            analogWrite(PIN[ROBOT][i][j], 0);
        }
    }
}

void Motor::run()
{
    if (flag)
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
                analogWrite(PIN[ROBOT][i][0], 255);
                analogWrite(PIN[ROBOT][i][1], 255 - _sp / 1);
            }
            else
            {
                analogWrite(PIN[ROBOT][i][1], 255);
                analogWrite(PIN[ROBOT][i][0], 255 - _sp / 1);
            }
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            analogWrite(PIN[ROBOT][i][0], 0);
            analogWrite(PIN[ROBOT][i][1], 0);
        }
    }
}