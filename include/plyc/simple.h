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
    ply_vec4 *data;
    size_t num;
} ply_SimpleCloud;

void ply_SimpleCloud_kill(ply_SimpleCloud *self);

typedef struct {
    ply_vec3i *indices;
    size_t num;
} ply_SimpleMeshIndices;

void ply_SimpleMeshIndices_kill(ply_SimpleMeshIndices *self);


typedef struct {
    char comments[PLY_MAX_COMMENTS][PLY_MAX_COMMENT_LENGTH];
    size_t comments_size;
} ply_comments;

ply_err ply_simple_load(ply_SimpleCloud *out_points,
                        ply_SimpleCloud *out_opt_normals,
                        ply_SimpleCloud *out_opt_colors,
                        ply_SimpleMeshIndices *out_opt_indices,
                        ply_comments *out_opt_comments,
                        const char *file_path);

ply_err ply_simple_save(ply_SimpleCloud points,
                        ply_SimpleCloud *opt_normals,
                        ply_SimpleCloud *opt_colors,
                        ply_SimpleMeshIndices *opt_indices,
                        ply_comments *opt_comments,
                        const char *file_path,
                        enum ply_format format);

#ifdef __cplusplus
}
#endif
#endif //PLYC_SIMPLE_LOAD_H
