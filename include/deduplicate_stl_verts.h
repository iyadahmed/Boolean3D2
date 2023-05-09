#pragma once

#include <common.h>
#include <map.h>

STRUCT(index_hash_map_t)
{
    map_t map;
    size_t num_added_vertices;
};

index_hash_map_t create_index_hash_map()
{
    return (index_hash_map_t) { create_map(100), 0 };
}