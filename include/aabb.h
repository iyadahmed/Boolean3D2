#pragma once

#include <stdbool.h>

#include <common.h>

#include "vec3.h"

STRUCT(aabb_2d_t)
{
    float x_max, y_max, x_min, y_min;
};

STRUCT(aabb_3d_t)
{
    vec3_t upper, lower;
};

bool do_aabb_2d_intersect(aabb_2d_t a, aabb_2d_t b)
{
    return a.x_max >= b.x_min && a.y_max >= b.y_min
        && a.x_min <= b.x_max && a.y_max <= b.y_max;
}