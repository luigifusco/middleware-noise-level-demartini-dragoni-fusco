#include<stdlib.h>

float rand_f() {
    return ((float)rand()/(float)(RAND_MAX));
}

float rand_range_f(float start, float end) {
    if (end < start) {
        float t = start;
        start = end;
        end = t;
    }
    float r = rand_f();
    return start + (end - start)*r;
}