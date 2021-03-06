#include<stdlib.h>
#include<stdio.h>

#define PI 3.141592653589793
#define PI2 PI*2

// Generate random float between 0 and 1
float rand_f() {
    return ((float)rand()/(float)(RAND_MAX));
}

// Generate random float between a specified range
float rand_range_f(float start, float end) {
    if (end < start) {
        float t = start;
        start = end;
        end = t;
    }
    float r = rand_f();
    return start + (end - start)*r;
}

// Read an int from /dev/random
int entropy_seed() {
    FILE* source = fopen("/dev/random", "r");
    int seed;
    fread(&seed, sizeof(seed), 1, source);
    fclose(source);
    return seed;
}
