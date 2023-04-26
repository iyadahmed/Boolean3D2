#pragma once

#include "vec3.h"

typedef union
{
    struct
    {
        vec3_t a, b, c;
    };
    vec3_t vertices[3];
} triangle_t;
