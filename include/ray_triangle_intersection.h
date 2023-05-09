#pragma once

#include "ray.h"
#include "ray_plane_intersection.h"
#include "triangle.h"
#include "vec3.h"

bool does_ray_intersect_triangle(ray_t ray, triangle_ex_t triangle)
{
    ray_plane_intersection_result_t result = intersect_ray_plane(ray, (plane_t){triangle.triangle.a, triangle.normal});

    if (result.type == RAY_PLANE_CONTAINED)
    {
        return true;
    }
    else if (result.type == RAY_PLANE_SINGLE_POINT)
    {
        vec3_t intersection_point = point_along_ray(ray, result.t);
        return is_point_inside_triangle_prism(intersection_point, triangle);
    }

    return false;
}
