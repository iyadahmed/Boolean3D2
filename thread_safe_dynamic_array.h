#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <stdbool.h>

typedef struct
{
    size_t size;
    size_t capacity;
    int *data;
} OpenMPDynamicArray;

static const size_t INITIAL_CAPACITY = 16;

bool openmp_dynamic_array_init(OpenMPDynamicArray *array)
{
    array->data = calloc(INITIAL_CAPACITY, sizeof(int));
    if (!array->data)
    {
        return false;
    }

    array->size = 0;
    array->capacity = INITIAL_CAPACITY;
    return true;
}

void openmp_dynamic_array_destroy(OpenMPDynamicArray *array)
{
    free(array->data);
}

static bool openmp_dynamic_array_resize(OpenMPDynamicArray *array, size_t new_capacity)
{
    int *new_data = calloc(new_capacity, sizeof(int));
    if (!new_data)
    {
        return false;
    }

    size_t size = array->size;
    memcpy(new_data, array->data, size * sizeof(int));
    free(array->data);

    array->data = new_data;
    array->capacity = new_capacity;
    return true;
}

bool openmp_dynamic_array_append(OpenMPDynamicArray *array, int value)
{
    size_t size;
    size_t capacity;

#pragma omp atomic capture
    size = array->size++;

    capacity = array->capacity;

    if (size == capacity)
    {
#pragma omp critical(resize)
        {
            // Check capacity again to ensure another thread has not already resized the array
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

    array->data[size] = value;
    return true;
}

int openmp_dynamic_array_get(OpenMPDynamicArray *array, size_t index)
{
    size_t size;

#pragma omp atomic read
    size = array->size;

    assert(index < size);
    return array->data[index];
}

bool openmp_dynamic_array_set(OpenMPDynamicArray *array, size_t index, int value)
{
    size_t size;

#pragma omp atomic read
    size = array->size;

    if (index >= size)
    {
        return false;
    }

    array->data[index] = value;
    return true;
}