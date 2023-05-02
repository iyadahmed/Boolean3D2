#pragma once

#include "vec3.h"

typedef struct
{
    vec3_t origin, direction;
} ray_t;

vec3_t point_along_ray(ray_t ray, float t)
{
    return vec3_add(ray.origin, vec3_multiply_by_scalar(ray.direction, t));
}
