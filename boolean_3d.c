#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "triangle.h"
#include "small_array.h"
#include "ray.h"
#include "ray_plane_intersection.h"

static ray_t ray_from_triangle_edge(triangle_ex_t t, int edge_index)
{
    assert(edge_index == 0 || edge_index == 1 || edge_index == 2);
    return (ray_t){t.triangle.vertices[edge_index], t.edge_vectors[edge_index]};
}

static plane_t triangle_plane(triangle_ex_t t)
{
    return (plane_t){t.triangle.a, t.normal};
}

typedef SMALL_ARRAY_TYPE(vec3_t, 3) vec3_t_3;

vec3_t_3 intersect_triangle_edges_with_other_triangle(triangle_ex_t t1, triangle_ex_t t2)
{
    CREATE_SMALL_ARRAY(vec3_t_3, result);

    for (int i = 0; i < 3; i++)
    {
        ray_t ray = ray_from_triangle_edge(t1, i);
        plane_t plane = triangle_plane(t2);
        ray_plane_intersection_result_t intersection_result = intersect_ray_plane(ray, plane);
        if (intersection_result.type == RAY_PLANE_SINGLE_POINT)
        {
            vec3_t p = point_along_ray(ray, intersection_result.t);
            PUSH_TO_SMALL_ARRAY(&result, p);
            printf("%f, %f, %f\n", p.x, p.y, p.z);
        }
    }

    return result;
}

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

    uint32_t num_triangles = 0;
    fread(&num_triangles, sizeof(uint32_t), 1, file);

    printf("Number of triangles = %u\n", num_triangles);

    triangle_ex_t *triangles = malloc(sizeof(triangle_ex_t) * num_triangles);

    float data[50] = {0};
    for (uint32_t i = 0; i < num_triangles; i++)
    {
        fread(data, sizeof(char), 50, file);
        // 3 floats for normal followed by 3x3 floats for 3 vertices
        float *data_iter = data;
        data_iter += 3; // Skip normals

        triangle_t *t = &(triangles[i].triangle);
        for (int vi = 0; vi < 3; vi++)
        {
            for (int vj = 0; vj < 3; vj++)
            {
                t->vertices[vi].as_array[vj] = *data_iter;
                data_iter++;
            }
        }
        triangles[i] = compute_triangle_extra_data(*t);
    }

    //
    // Do self intersection
    //

    for (uint32_t i = 0; i < num_triangles; i++)
    {
        triangle_ex_t t1 = triangles[i];
        for (uint32_t j = 0; j < num_triangles; j++)
        {
            triangle_ex_t t2 = triangles[j];

            vec3_t_3 intersection_points_forward = intersect_triangle_edges_with_other_triangle(t1, t2);
            assert(intersection_points_forward.occupied <= 2);
            vec3_t_3 intersection_points_reverse = intersect_triangle_edges_with_other_triangle(t2, t1);
            assert(intersection_points_reverse.occupied <= 2);
        }
    }

    free(triangles);
    return 0;
}