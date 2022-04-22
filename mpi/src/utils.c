#include<stdlib.h>
#include<stdio.h>

#define PI 3.141592653589793
#define PI2 PI*2

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

int entropy_seed() {
    FILE* source = fopen("/dev/random", "r");
    int seed;
    fread(&seed, sizeof(seed), 1, source);
    fclose(source);
    return seed;
}