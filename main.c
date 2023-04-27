#include <minimal_window.h>

#include "ray_triangle_intersection.h"

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
                if (does_ray_intersect_triangle(ray, t))
                    minimal_window_draw_pixel(i, j, 255, 0, 255);
                else
                    minimal_window_draw_pixel(i, j, 0, 0, 0);
            }
        }
    }
    return 0;
}
