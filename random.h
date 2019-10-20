#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random
{
    static std::random_device rd;
    static std::mt19937 gen;
    static std::uniform_real_distribution<float> fdis;
public:
    static float randomUnit();
    static float random(float min, float max);
    static int random(int high, int low = 0);
    static void setSeed(int seed);
};

#endif // RANDOM_H
