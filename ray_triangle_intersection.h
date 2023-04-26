#pragma once

#include "ray.h"
#include "ray_plane_intersection.h"
#include "triangle.h"
#include "vec3.h"

bool is_point_above_plane(vec3_t point, plane_t plane)
{
    return vec3_dot_product(vec3_subtract(point, plane.point), plane.normal) > 0.0;
}

bool intersect_ray_triangle(ray_t ray, triangle_t triangle)
{
    // Compute edge vectors and normal
    // TODO: pre-compute and store
    vec3_t e1 = vec3_subtract(triangle.b, triangle.a);
    vec3_t e2 = vec3_subtract(triangle.c, triangle.a);
    vec3_t e3 = vec3_subtract(triangle.c, triangle.b);
    vec3_t normal = vec3_normalized(vec3_cross_product(e1, e2));

    // Compute the triangle's 3 auxilary planes
    // (three planes with normals pointing into the inside of the triangle)
    // TODO: pre-compute and store
    plane_t p1 = {triangle.a, vec3_normalized(vec3_cross_product(e1, normal))};
    plane_t p2 = {triangle.a, vec3_normalized(vec3_cross_product(normal, e2))};
    plane_t p3 = {triangle.b, vec3_normalized(vec3_cross_product(normal, e3))};

    ray_plane_intersection_result_t r1 = intersect_ray_plane(ray, (plane_t){triangle.a, normal});

    if (r1.type == RAY_PLANE_NO_INTERSECTION)
    {
        return false;
    }

    if (r1.type == RAY_PLANE_CONTAINED)
    {
        // TODO: intersect with auxilary planes
    }

    // Single point
    vec3_t intersection_point = vec3_add(ray.origin, vec3_multiply_by_scalar(ray.direction, r1.t));

    return is_point_above_plane(intersection_point, p1) &&
           is_point_above_plane(intersection_point, p2) &&
           is_point_above_plane(intersection_point, p3);
}
