#pragma once

#include <stdbool.h>
#include <math.h>

#include "vec3.h"
#include "plane.h"
#include "aabb.h"

typedef union
{
    struct
    {
        vec3_t a, b, c;
    };
    vec3_t vertices[3];
} triangle_t;

// Extended triangle struct with extra data, useful for computing various types of intersections
typedef struct
{
    triangle_t triangle;
    vec3_t edge_vectors[3]; // Normalized edge vectors
    vec3_t normal;
    plane_t auxilary_planes[3]; // Planes with normals pointing towards the inside of the triangle
    aabb_2d_t aabb_2d;
} triangle_ex_t;

triangle_ex_t compute_triangle_extra_data(triangle_t triangle)
{
    // Compute edge vectors and normal
    vec3_t e1 = vec3_normalized(vec3_subtract(triangle.b, triangle.a));
    vec3_t e2 = vec3_normalized(vec3_subtract(triangle.c, triangle.b));
    vec3_t e3 = vec3_normalized(vec3_subtract(triangle.a, triangle.c));
    vec3_t normal = vec3_normalized(vec3_cross_product(e1, e2));

    // Compute the triangle's 3 auxilary planes
    // (three planes with normals pointing into the inside of the triangle),
    // Note: there is no need to normalize result of cross product if input vectors are unit vectors and orthogonal:
    // https://math.stackexchange.com/a/23261/691043
    plane_t p1 = {triangle.a, vec3_cross_product(normal, e1)};
    plane_t p2 = {triangle.b, vec3_cross_product(normal, e2)};
    plane_t p3 = {triangle.c, vec3_cross_product(normal, e3)};

    aabb_2d_t aabb_2d;
    aabb_2d.x_max = max(max(triangle.a.x, triangle.b.x), triangle.c.x);
    aabb_2d.x_min = min(min(triangle.a.x, triangle.b.x), triangle.c.x);

    aabb_2d.y_max = max(max(triangle.a.y, triangle.b.y), triangle.c.y);
    aabb_2d.y_min = min(min(triangle.a.y, triangle.b.y), triangle.c.y);

    return (triangle_ex_t){triangle, {e1, e2, e3}, normal, {p1, p2, p3}, aabb_2d};
}

bool is_point_inside_triangle_prism(vec3_t p, triangle_ex_t t)
{
    return is_point_above_plane(p, t.auxilary_planes[0]) &&
           is_point_above_plane(p, t.auxilary_planes[1]) &&
           is_point_above_plane(p, t.auxilary_planes[2]);
}
