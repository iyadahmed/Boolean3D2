#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <small_array.h>

#include "deduplicate_stl_verts.h"
#include "ray.h"
#include "ray_plane_intersection.h"
#include "triangle.h"
#include "thread_safe_dynamic_array.h"


#if defined(_MSC_VER)
#define assert_unreachable() __assume(false)
#elif defined(__GNUC__)
#define assert_unreachable() __builtin_unreachable()
#else
#error "Failed to find implementation for assert_unreachable"
#endif

STRUCT(self_intersection_result_t) {
	size_t t1_index, t2_index;
	vec3_t point;
};

static int order_invariant_intersection_result_compare(const void* av, const void* bv) {
	const self_intersection_result_t* a = av;
	const self_intersection_result_t* b = bv;

	size_t a_t1_index = min(a->t1_index, a->t2_index);
	size_t a_t2_index = max(a->t1_index, a->t2_index);

	size_t b_t1_index = min(b->t1_index, b->t2_index);
	size_t b_t2_index = max(b->t1_index, b->t2_index);

	if (a_t1_index == b_t1_index && a_t2_index == b_t2_index) {
		return 0;
	}
	if (a_t1_index < b_t1_index || (a_t1_index == b_t1_index && a_t2_index < b_t2_index)) {
		return -1;
	}
	if (a_t1_index > b_t1_index || (a_t1_index == b_t1_index && a_t2_index > b_t2_index)) {
		return 1;
	}
	assert_unreachable();
	return 0;
}

static ray_t ray_from_triangle_edge(triangle_ex_t t, int edge_index)
{
	assert(edge_index == 0 || edge_index == 1 || edge_index == 2);
	return (ray_t) { t.triangle.vertices[edge_index], t.edge_vectors[edge_index] };
}

static plane_t triangle_plane(triangle_ex_t t)
{
	return (plane_t) { t.triangle.a, t.normal };
}

typedef SMALL_ARRAY_TYPE(vec3_t, 3) vec3_t_3;

vec3_t_3 intersect_triangle_edges_with_other_triangle(triangle_ex_t t1,
	triangle_ex_t t2)
{
	CREATE_SMALL_ARRAY(vec3_t_3, result);

	for (int i = 0; i < 3; i++) {
		ray_t ray = ray_from_triangle_edge(t1, i);
		plane_t plane = triangle_plane(t2);
		ray_plane_intersection_result_t intersection_result = intersect_ray_plane(ray, plane);
		if (intersection_result.type == RAY_PLANE_SINGLE_POINT) {
			vec3_t p = point_along_ray(ray, intersection_result.t);
			if (is_point_inside_triangle_prism(p, t2)) {
				PUSH_TO_SMALL_ARRAY(&result, p);
			}
		}
	}

	return result;
}

bool is_one_of_triangle_vertices(vec3_t p, triangle_t t)
{
	return vec3_almost_equal(p, t.a) || vec3_almost_equal(p, t.b) || vec3_almost_equal(p, t.c);
}

void clear_if_degenerates_to_triangle_edge_or_vertex(vec3_t_3* points,
	triangle_t t)
{
	int counter = 0;
	for (int i = 0; i < points->occupied; i++) {
		if (is_one_of_triangle_vertices(points->array[i], t)) {
			counter++;
		}
	}
	if (counter == points->occupied) {
		CLEAR_SMALL_ARRAY(points);
	}
}

vec3_t_3 deduplicate_points(vec3_t_3* points)
{
	CREATE_SMALL_ARRAY(vec3_t_3, result);
	for (int i = 0; i < points->occupied; i++) {
		vec3_t p = points->array[i];
		bool already_added = false;
		for (int j = 0; j < result.occupied; j++) {
			if (vec3_almost_equal(p, result.array[j])) {
				already_added = true;
				break;
			}
		}
		if (!already_added) {
			PUSH_TO_SMALL_ARRAY(&result, p);
		}
	}
	return result;
}

void vec3_print(vec3_t v) { printf("%f, %f, %f", v.x, v.y, v.z); }

void vec3_t_3_print(vec3_t_3* points)
{
	for (int i = 0; i < points->occupied; i++) {
		vec3_print(points->array[i]);
		putchar('\n');
	}
}

void triangle_triangle_intersection_non_coplanar(triangle_ex_t t1,
	triangle_ex_t t2)
{
	// Forward and reverse intersection points
	vec3_t_3 forward = intersect_triangle_edges_with_other_triangle(t1, t2);
	vec3_t_3 reverse = intersect_triangle_edges_with_other_triangle(t2, t1);
	assert(forward.occupied <= 2);
	assert(reverse.occupied <= 2);

	// Clear small arrays if they are just edges that already exist
	if (forward.occupied == 2) {
		clear_if_degenerates_to_triangle_edge_or_vertex(&forward, t2.triangle);
	}
	if (reverse.occupied == 2) {
		clear_if_degenerates_to_triangle_edge_or_vertex(&reverse, t1.triangle);
	}

	// Deduplicate points (should help in degenerate cases)
	forward = deduplicate_points(&forward);
	reverse = deduplicate_points(&reverse);

	if (forward.occupied > 0 || reverse.occupied > 0) {
		puts("Forward:");
		vec3_t_3_print(&forward);
		puts("Reverse:");
		vec3_t_3_print(&reverse);
	}
	// TODO: continue
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		puts("Expected arguments: path/to/mesh.stl");
		return 0;
	}

	//
	// Read mesh
	//
	FILE* file = fopen(argv[1], "rb");
	if (file == NULL) {
		puts("Failed to open file");
		return 0;
	}
	char binary_header[80] = { 0 };
	fread(binary_header, sizeof(char), 80, file);

	uint32_t num_triangles = 0;
	fread(&num_triangles, sizeof(uint32_t), 1, file);

	printf("Number of triangles = %u\n", num_triangles);

	triangle_ex_t* triangles = malloc(sizeof(triangle_ex_t) * num_triangles);

	if (triangles == NULL) {
		puts("Failed to allocate memory");
		return 0;
	}

	float data[50] = { 0 };
	for (uint32_t i = 0; i < num_triangles; i++) {
		fread(data, sizeof(char), 50, file);
		// 3 floats for normal followed by 3x3 floats for 3 vertices
		float* data_iter = data;
		data_iter += 3; // Skip normals

		triangle_t* t = &(triangles[i].triangle);
		for (int vi = 0; vi < 3; vi++) {
			for (int vj = 0; vj < 3; vj++) {
				t->vertices[vi].as_array[vj] = *data_iter;
				data_iter++;
			}
		}
		triangles[i] = compute_triangle_extra_data(*t);
	}

	//
	// Create dynamic array to store result of intersections
	//
	OpenMPDynamicArray results;
	if (!openmp_dynamic_array_init(&results, sizeof(self_intersection_result_t))) {
		puts("Failed to create dynamic array");
		return 0;
	}

	//
	// Do self intersection
	//

	for (uint32_t i = 0; i < num_triangles; i++) {
		triangle_ex_t t1 = triangles[i];
		for (uint32_t j = 0; j < num_triangles; j++) {

			// Algorithm 1, Do full triangle-triangle intersections, too much duplicated
			// work
			triangle_ex_t t2 = triangles[j];
#if 0
			puts("############");
			printf("T1: %d, T2: %d\n", i, j);
			triangle_triangle_intersection_non_coplanar(t1, t2);
#else // Algorithm 2: Only do edge-triangle intersections
			for (int ei = 0; ei < 3; ei++) {
				//vec3_print(t1.triangle.vertices[ei]);
				//puts("");

				ray_t ray = { t1.triangle.vertices[ei], t1.edge_vectors[ei] };
				plane_t t2_plane = { t2.triangle.a, t2.normal };
				ray_plane_intersection_result_t result = intersect_ray_plane(ray, t2_plane);
				if (result.type == RAY_PLANE_CONTAINED) {
					// Coplanar case
				}
				else if (result.type == RAY_PLANE_SINGLE_POINT) {

					self_intersection_result_t r;
					r.point = point_along_ray(ray, result.t);

					if (vec3_almost_equal(r.point, t2.triangle.a) ||
						vec3_almost_equal(r.point, t2.triangle.b) ||
						vec3_almost_equal(r.point, t2.triangle.c)) {
					}
					else
					{
						r.t1_index = i;
						r.t2_index = j;
						openmp_dynamic_array_append(&results, &r);
					}
				}
			}
#endif
		}
	}

	//for (size_t i = 0; i < results.size; i++)
	//{
	//	self_intersection_result_t* sr = openmp_dynamic_array_get(&results, i);
	//	printf("(%llu, %llu) ", sr->t1_index, sr->t2_index);
	//	//printf("%f, %f, %f\n", sr->point.x, sr->point.y, sr->point.z);
	//}
	//puts("");

	// Sort results array
	qsort(results.data, results.size, sizeof(self_intersection_result_t), order_invariant_intersection_result_compare);

	//for (size_t i = 0; i < results.size; i++)
	//{
	//	self_intersection_result_t* sr = openmp_dynamic_array_get(&results, i);
	//	printf("(%llu, %llu) ", sr->t1_index, sr->t2_index);
	//	vec3_print(sr->point);
	//	puts("");
	//	//printf("%f, %f, %f\n", sr->point.x, sr->point.y, sr->point.z);
	//}
	//puts("");

	printf("%llu\n", results.size);


	// Cluster
	size_t i = 0;
	size_t n = results.size;
	while (i < n)
	{
		size_t j = i + 1;
		while (j < n &&
			(order_invariant_intersection_result_compare(openmp_dynamic_array_get(&results, i), openmp_dynamic_array_get(&results, j)) == 0)) {
			j++;
		}
		puts("#########");

		// Triangulate
		for (int k = i; k < j; k++) {
			self_intersection_result_t* sr = openmp_dynamic_array_get(&results, k);
			printf("(%llu, %llu) ", sr->t1_index, sr->t2_index);
			vec3_print(sr->point);
			puts("");
			// TODO, triangulate once for t1 and once for t2
		}

		puts("#########");

		i = j;
	}

	openmp_dynamic_array_destroy(&results);

	free(triangles);
	return 0;
}
