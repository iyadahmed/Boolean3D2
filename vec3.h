#pragma once

#include <math.h>

typedef union
{
    struct
    {
        float x, y, z;
    };
    float as_array[3];
} vec3_t;

vec3_t vec3_add(vec3_t a, vec3_t b)
{
    return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3_t vec3_subtract(vec3_t a, vec3_t b)
{
    return (vec3_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3_t vec3_divide_by_scalar(vec3_t v, float s)
{
    return (vec3_t){v.x / s, v.y / s, v.z / s};
}

vec3_t vec3_multiply_by_scalar(vec3_t v, float s)
{
    return (vec3_t){v.x * s, v.y * s, v.z * s};
}

float vec3_dot_product(vec3_t a, vec3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vec3_magnitude_squared(vec3_t v)
{
    return vec3_dot_product(v, v);
}

float vec3_magnitude(vec3_t v)
{
    return sqrtf(vec3_magnitude_squared(v));
}

vec3_t vec3_normalized(vec3_t v)
{
    return vec3_divide_by_scalar(v, vec3_magnitude(v));
}

// Reference: https://en.wikipedia.org/wiki/Cross_product#Computing
vec3_t vec3_cross_product(vec3_t a, vec3_t b)
{
    float x = a.y * b.z - a.z * b.y;
    float y = a.z * b.x - a.x * b.z;
    float z = a.x * b.y - a.y * b.x;
    return (vec3_t){x, y, z};
}
