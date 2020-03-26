#ifndef DENAN_H
#define DENAN_H

#include <cmath>
 
double DeNan(double x, double replace){
    if(x!=x || isinf(x))return replace;
    return x;
}

#endif
