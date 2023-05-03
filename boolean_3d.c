#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "triangle.h"
#include "triangle_triangle_intersection.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        puts("Expected arguments: path/to/mesh.stl");
        return 0;
    }

    //
    // Read mesh
    //
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL)
    {
        puts("Failed to open file");
        return 0;
    }
    char binary_header[80] = {0};
    fread(binary_header, sizeof(char), 80, file);

    uint32_t num_tris = 0;
    fread(&num_tris, sizeof(uint32_t), 1, file);

    printf("Number of triangles = %u\n", num_tris);

    triangle_ex_t *tris = malloc(sizeof(triangle_ex_t) * num_tris);

    float data[50] = {0};
    for (uint32_t i = 0; i < num_tris; i++)
    {
        fread(data, sizeof(char), 50, file);
        // 3 floats for normal followed by 3x3 floats for 3 vertices
        float *data_iter = data;
        data_iter += 3; // Skip normals

        triangle_t *t = &(tris[i].triangle);
        for (int vi = 0; vi < 3; vi++)
        {
            for (int vj = 0; vj < 3; vj++)
            {
                t->vertices[vi].as_array[vj] = *data_iter;
                data_iter++;
            }
        }
        tris[i] = compute_triangle_extra_data(*t);
    }

    for (uint32_t i = 0; i < num_tris; i++)
    {
        triangle_ex_t t1 = tris[i];
        for (uint32_t j = 0; j < num_tris; j++)
        {
            triangle_ex_t t2 = tris[j];

            puts("##################");
            for (int vi = 0; vi < 3; vi++)
            {
                ray_t ray = {t1.triangle.vertices[vi], t1.edge_vectors[vi]};
                ray_plane_intersection_result_t bar = intersect_ray_plane(ray, (plane_t){t2.triangle.a, t2.normal});
                if (bar.type == RAY_PLANE_SINGLE_POINT)
                {
                    vec3_t p = point_along_ray(ray, bar.t);
                    printf("%f, %f, %f\n", p.x, p.y, p.z);
                }
            }
        }
    }

    free(tris);
    return 0;
}