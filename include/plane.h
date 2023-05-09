#pragma once

#include "vec3.h"

typedef struct
{
    vec3_t point, normal;
} plane_t;

bool is_point_above_plane(vec3_t point, plane_t plane)
{
    return vec3_dot_product(vec3_subtract(point, plane.point), plane.normal) > -0.00001;
}
