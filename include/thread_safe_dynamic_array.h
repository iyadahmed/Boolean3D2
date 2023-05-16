#pragma once

#include <assert.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    size_t size;
    size_t capacity;
    void *data;
    size_t element_size;
} OpenMPDynamicArray;

static const size_t INITIAL_CAPACITY = 16;

bool openmp_dynamic_array_init(OpenMPDynamicArray *array, size_t element_size)
{
    array->data = calloc(INITIAL_CAPACITY, element_size);
    if (!array->data)
    {
        return false;
    }

    array->size = 0;
    array->capacity = INITIAL_CAPACITY;
    array->element_size = element_size;
    return true;
}

void openmp_dynamic_array_destroy(OpenMPDynamicArray *array)
{
    free(array->data);
}

static bool openmp_dynamic_array_resize(OpenMPDynamicArray *array, size_t new_capacity)
{
    void *new_data = calloc(new_capacity, array->element_size);
    if (!new_data)
    {
        return false;
    }

    size_t size = array->size;
    memcpy(new_data, array->data, size * array->element_size);
    free(array->data);

    array->data = new_data;
    array->capacity = new_capacity;
    return true;
}

bool openmp_dynamic_array_append(OpenMPDynamicArray *array, void *value)
{
    size_t size;
    size_t capacity;

#pragma omp atomic read
    size = array->size;

    capacity = array->capacity;

    if (size == capacity)
    {
#pragma omp critical(resize)
        {
            capacity = array->capacity;
            if (size == capacity)
            {
                if (!openmp_dynamic_array_resize(array, 2 * capacity))
                {
                    return false;
                }
            }
        }
    }

#pragma omp atomic capture
    size = array->size++;

    memcpy((char *)array->data + size * array->element_size, value, array->element_size);

    return true;
}

void *openmp_dynamic_array_get(OpenMPDynamicArray *array, size_t index)
{
#ifndef NDEBUG
#pragma omp atomic read
    size_t size = array->size;
    assert(index < size);
#endif

    return (char *)array->data + index * array->element_size;
}

bool openmp_dynamic_array_set(OpenMPDynamicArray *array, size_t index, void *value)
{
    size_t size;

#pragma omp atomic read
    size = array->size;

    if (index >= size)
    {
        return false;
    }

    memcpy((char *)array->data + index * array->element_size, value, array->element_size);
    return true;
}