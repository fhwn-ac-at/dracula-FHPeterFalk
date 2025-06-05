#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct point_t
{
    float x;
    float y;    
} point_t;

point_t random_point() {

    float x = 1 - 2 * (float)(rand() % 1001)/1000;
    float y = 1 - 2 * (float)(rand() % 1001)/1000;
    point_t point = {
        x,
        y,
    };
    return point;
}

bool is_in_unit_circle(point_t p) {
    return ((p.x * p.x) + (p.y * p.y) < 1);
}

point_t random_point_in_unit_circle() {
    point_t p;
    do {
        p = random_point();
    } while (!is_in_unit_circle(p));
    return p;
}

float approximate_pi() {
    int in_circle = 0;
    for (int i = 0; i < 1000000; i++) {
        point_t p = random_point();
        if(is_in_unit_circle(p)) {in_circle++;};
    }
    float pi = 4* ((float)in_circle/1000000);
    return pi;
}

int main() {
    
    srand(time(0));
    point_t my_point = random_point();
    printf("x: %f, y: %f\n", my_point.x, my_point.y);
    float pi = approximate_pi();
    printf("pi is: %f\n", pi);
}
