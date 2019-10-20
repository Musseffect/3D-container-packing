#include "random.h"

float Random::randomUnit()
{
    return fdis(gen);
}
float Random::random(float min,float max)
{
    return fdis(gen)*(max-min)+min;
}
int Random::random(int high, int low)
{
    return low + (int)round((high - low)*randomUnit());
}
void Random::setSeed(int seed)
{
    gen.seed(seed);
}
std::random_device Random::rd;
std::mt19937 Random::gen = std::mt19937(Random::rd());
std::uniform_real_distribution<float> Random::fdis(0.0f, 1.0f);

