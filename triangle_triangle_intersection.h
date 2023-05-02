#pragma once

#include <assert.h>
#include <math.h>

#include "plane.h"
#include "ray_plane_intersection.h"
#include "triangle.h"

#define MAX_NUM_EDGE_NET_EDGES 6
#define MAX_NUM_EDGE_NET_VERTICES 6

typedef struct
{
    int a, b;
} edge_t;

typedef struct
{
    edge_t edges[MAX_NUM_EDGE_NET_EDGES];
    vec3_t vertices[MAX_NUM_EDGE_NET_VERTICES];
    int num_edges, num_vertices;
} edge_net_t;

edge_net_t intersect_triangle_triangle(triangle_ex_t t1, triangle_ex_t t2)
{
    bool is_coplanar = false;
    if (fabsf(vec3_dot_product(t1.normal, t2.normal)) < 0.00001f)
    {
        // Possibly coplanar, but can be parallel
        {
            vec3_t v = vec3_normalized(vec3_subtract(t1.triangle.a, t2.triangle.a));
            if (fabsf(vec3_dot_product(v, t2.normal)) < 0.00001f)
            {
                // Coplanar
                is_coplanar = true;
            }
            else
            {
                // Parallel
                return (edge_net_t){{}, {}, 0, 0};
            }
        }
    }

    if (is_coplanar)
    {
    }
    else
    {
        // Intersect the edges of each triangle with the the plane of the other triangle
        vec3_t ints1[3];
        int num_ints1 = 0;
        vec3_t ints2[3];
        int num_ints2 = 0;
        for (int i = 0; i < 3; i++)
        {
            ray_t ray = {t1.triangle.vertices[i], t1.edge_vectors[i]};
            ray_plane_intersection_result_t result = intersect_ray_plane(ray, (plane_t){t2.triangle.a, t2.normal});
            if (result.type == RAY_PLANE_SINGLE_POINT)
            {
                ints1[num_ints1] = point_along_ray(ray, result.t);
                num_ints1 += 1;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            ray_t ray = {t2.triangle.vertices[i], t2.edge_vectors[i]};
            ray_plane_intersection_result_t result = intersect_ray_plane(ray, (plane_t){t1.triangle.a, t1.normal});
            if (result.type == RAY_PLANE_SINGLE_POINT)
            {
                ints2[num_ints2] = point_along_ray(ray, result.t);
                num_ints2 += 1;
            }
        }

        // Deduplicate results
        vec3_t unique_ints1[3];
        int num_unique_ints1 = 0;
        vec3_t unique_ints2[3];
        int num_unique_ints2 = 0;
        for (int i = 0; i < num_ints1; i++)
        {
            bool already_exists = false;
            for (int j = 0; j < num_ints1; j++)
            {
                if (vec3_almost_equal(ints1[i], ints1[j]))
                {
                    already_exists = true;
                    break;
                }
            }
            if (!already_exists)
            {
                unique_ints1[num_unique_ints1] = ints1[i];
                num_unique_ints1 += 1;
            }
        }
        for (int i = 0; i < num_ints2; i++)
        {
            bool already_exists = false;
            for (int j = 0; j < num_ints1; j++)
            {
                if (vec3_almost_equal(ints1[i], ints1[j]))
                {
                    already_exists = true;
                    break;
                }
            }
            if (!already_exists)
            {
                unique_ints2[num_unique_ints2] = ints1[i];
                num_unique_ints2 += 1;
            }
        }

        // Assert some assumptions
        assert((num_unique_ints1 <= 1 && num_unique_ints2 <= 1) ||
               (num_unique_ints1 <= 2 && num_unique_ints2 == 0) ||
               (num_unique_ints1 == 0 && num_unique_ints2 <= 2));

        // Only include points that are inside triangle
        vec3_t ints1_final[3];
        int num_ints1_final = 0;
        vec3_t ints2_final[3];
        int num_ints2_final = 0;

        for (int i = 0; i < num_unique_ints1; i++)
        {
            if (is_point_inside_triangle(unique_ints1[i], t1))
            {
                ints1_final[num_ints1_final] = unique_ints1[i];
                num_ints1_final += 1;
            }
        }

        for (int i = 0; i < num_unique_ints2; i++)
        {
            if (is_point_inside_triangle(unique_ints2[i], t1))
            {
                ints2_final[num_ints2_final] = unique_ints2[i];
                num_ints2_final += 1;
            }
        }

        // Join the two arrays together
        vec3_t final_result[6];
        int num_final_result = 0;
        for (int i = 0; i < num_ints1_final; i++)
        {
            final_result[num_final_result] = unique_ints1[i];
            num_final_result += 1;
        }
        for (int i = 0; i < num_ints2_final; i++)
        {
            final_result[num_final_result] = unique_ints2[i];
            num_final_result += 1;
        }

        // Another assertion though not strictly needed, but good to add
        assert(num_final_result <= 2);

        if (num_final_result == 2)
        {
            return (edge_net_t){
                .edges = {{0, 1}},
                .vertices = {final_result[0], final_result[1]},
                .num_edges = 1,
                .num_vertices = 2};
        }
        else if (num_final_result = 1)
        {
            return (edge_net_t){
                .edges = {},
                .vertices = {final_result[0]},
                .num_edges = 0,
                .num_vertices = 1};
        }
        else
        {
            return (edge_net_t){{}, {}, 0, 0};
        }
    }
}
