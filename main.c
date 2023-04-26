#include <minimal_window.h>

#include "ray_triangle_intersection.h"

bool ray_triangle_intersection(ray_t ray, triangle_t triangle)
{
    // Find vectors for two edges of the triangle
    vec3_t edge1 = vec3_subtract(triangle.b, triangle.a);
    vec3_t edge2 = vec3_subtract(triangle.c, triangle.a);

    // Calculate the determinant and inverse of matrix formed by the two edge vectors
    float determinant = vec3_cross_product(edge1, edge2).z;

    // If determinant is zero, then the three points are co-linear, so skip the triangle
    if (determinant == 0)
        return false;

    float inverse_determinant = 1.0f / determinant;

    // Calculate the distance from the origin to the plane defined by the triangle
    vec3_t triangle_origin = triangle.a;
    vec3_t diff = vec3_subtract(triangle_origin, ray.origin);
    float u = vec3_cross_product(edge2, diff).z * inverse_determinant;
    if (u < 0.0f || u > 1.0f)
        return false;

    float w = vec3_cross_product(diff, edge1).z * inverse_determinant;
    if (w < 0.0f || u + w > 1.0f)
        return false;

    // Calculate the distance to the intersection point
    float t = vec3_cross_product(edge1, ray.direction).z * inverse_determinant;
    if (t < 0)
        return false;

    return true;
}

int main()
{
    int width = 640, height = 480;
    minimal_window_create_fixed_size_window(width, height);
    while (minimal_window_process_events())
    {
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                ray_t ray = {.origin = {i, j, 10}, .direction = {0, 0, -1}};
                triangle_t t = {.a = {width / 2, 0, 0}, .b = {width, height, 0}, .c = {0, height, 0}};
                if (intersect_ray_triangle(ray, t))
                    minimal_window_draw_pixel(i, j, 255, 0, 255);
                else
                    minimal_window_draw_pixel(i, j, 0, 0, 0);
            }
        }
    }
    return 0;
}
