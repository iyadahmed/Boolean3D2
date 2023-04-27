#pragma once

#include <assert.h>
#include <math.h>

#include "plane.h"
#include "ray.h"
#include "vec3.h"

enum ray_plane_intersection_type_t
{
    RAY_PLANE_PARALLEL,
    RAY_PLANE_CONTAINED,
    RAY_PLANE_SINGLE_POINT,
    RAY_PLANE_NO_INTERSECTION
};

typedef struct
{
    float t;
    enum ray_plane_intersection_type_t type;
} ray_plane_intersection_result_t;

// Reference: https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection#Algebraic_form
ray_plane_intersection_result_t intersect_ray_plane(ray_t ray, plane_t plane)
{
    assert(fabsf(vec3_magnitude_squared(ray.direction) - 1.0f) < 0.00001f);
    assert(fabsf(vec3_magnitude_squared(plane.normal) - 1.0f) < 0.00001f);

    vec3_t v = vec3_subtract(plane.point, ray.origin);
    float v_dot_n = vec3_dot_product(v, plane.normal);
    float l_dot_n = vec3_dot_product(ray.direction, plane.normal);

    if (fabsf(l_dot_n) < 0.00001f)
    {
        if (fabsf(v_dot_n) < 0.00001f)
        {
            return (ray_plane_intersection_result_t){0.0f, RAY_PLANE_CONTAINED};
        }
        else
        {
            return (ray_plane_intersection_result_t){0.0f, RAY_PLANE_NO_INTERSECTION};
        }
    }

    float t = v_dot_n / l_dot_n;
    return (ray_plane_intersection_result_t){t, RAY_PLANE_SINGLE_POINT};
}
