#include "header.hpp"

static double random_unity()
{
    double random = (float) rand()/(float) RAND_MAX;
    return random;
}

static double random_range(double max,double min)
{
    double range = max-min;
    double random = min + (random_unity()*range);
    return random;
}
