#include <minimal_window.h>

#include "ray_triangle_intersection.h"

int main()
{
    int width = 640, height = 480;
    minimal_window_create_fixed_size_window(width, height);

    triangle_t triangle = {.a = {width / 2, 0, 0}, .b = {width, height, 0}, .c = {0, height, 0}};
    vec3_t ray_direction = {0, 0, -1};

    bool use_antialiasing = true;

    while (minimal_window_process_events())
    {
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {

                if (use_antialiasing)
                {
                    float sample_spacing = 1.0f / 3.0f;
                    vec3_t sample_start = {(float)i + sample_spacing, (float)j + sample_spacing, 10};

                    float avg = 0.0f;
                    for (int io = 0; io < 3; io++)
                    {
                        for (int jo = 0; jo < 3; jo++)
                        {
                            vec3_t offset = {io * sample_spacing, jo * sample_spacing, 0.0f};
                            vec3_t ray_origin = vec3_add(sample_start, offset);
                            ray_t ray = {ray_origin, ray_direction};
                            avg += (float)does_ray_intersect_triangle(ray, triangle) / 9.0f;
                        }
                    }

                    minimal_window_draw_pixel(i, j, (uint8_t)(avg * 255.0f), 0, (uint8_t)(avg * 255.0f));
                }
                else
                {
                    ray_t ray = {.origin = {(float)i + 0.5f, (float)j + 0.5f, 10}, .direction = {0, 0, -1}};
                    if (does_ray_intersect_triangle(ray, triangle))
                    {
                        minimal_window_draw_pixel(i, j, 255, 0, 255);
                    }
                    else
                    {
                        minimal_window_draw_pixel(i, j, 0, 0, 0);
                    }
                }
            }
        }
    }
    return 0;
}
