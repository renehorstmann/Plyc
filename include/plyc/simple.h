#ifndef PLYC_SIMPLE_LOAD_H
#define PLYC_SIMPLE_LOAD_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "header.h"


typedef float ply_vec4[4];
typedef int ply_vec3i[3];

typedef struct {
    ply_vec4 *points;
    ply_vec4 *normals;
    ply_vec4 *colors;
    size_t num;

    ply_vec3i *indices;
    size_t indices_size;

    char comments[PLY_MAX_COMMENTS][PLY_MAX_COMMENT_LENGTH];
    size_t comments_size;

    bool holds_heap_memory_;
} ply_Simple;

// only kill if not custom created
void ply_Simple_kill(ply_Simple *self);


ply_err ply_simple_load(ply_Simple *out_simple,
                        const char *filename);

ply_err ply_simple_save(ply_Simple simple,
                        const char *filename,
                        enum ply_format format);

#ifdef __cplusplus
}
#endif
#endif //PLYC_SIMPLE_LOAD_H
