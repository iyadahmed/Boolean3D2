#pragma once

#include <math.h>
#include <stdbool.h>

#include "aabb.h"
#include "triangle.h"
#include "vec3.h"

bool does_aabb_3d_intersect_aabb_2d(aabb_3d_t a, aabb_2d_t b)
{
    return a.upper.x >= b.x_min && a.upper.y >= b.y_min && a.lower.x <= b.x_max && a.lower.y <= b.y_max;
}

float plane_side(vec3_t p, vec3_t plane_point, vec3_t plane_normal)
{
    return vec3_dot_product(vec3_subtract(p, plane_point));
}

bool does_aabb_intersect_triangle(aabb_3d_t aabb, triangle_ex_t t)
{
    if (does_aabb_3d_intersect_aabb_2d(aabb, t.aabb_2d)) {
        // If so check if all AABB points (enough to check maxima and minima points) lie on the same side of the triangle's plane
        // NOTE: signbit treats 0 and positive values the same, maybe modify code to treat 0 and negative the same instead,
        // since it would mean touching the triangle's plane, but we add tolerance values anyway in other places
        return ((bool)signbit(plane_side(aabb.upper, t.triangle.a, t.normal)))
            == ((bool)signbit(plane_side(aabb.lower, t.triangle.a, t.normal)));
    }
    return false;
}