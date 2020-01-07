#ifndef PLYC_SIMPLE_LOAD_H
#define PLYC_SIMPLE_LOAD_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "header.h"


typedef float ply_vec4[4];

typedef struct {
    ply_vec4 *points;
    ply_vec4 *normals;
    ply_vec4 *colors;
    float *curvatures;
    size_t size;
} ply_SimpleCloud;


void ply_SimpleCloud_kill(ply_SimpleCloud *self);


typedef struct {
    char comments[PLY_MAX_COMMENTS][PLY_MAX_COMMENT_LENGTH];
    size_t comments_size;
} ply_comments;

ply_err ply_simple_load_cloud(ply_SimpleCloud *out_cloud, ply_comments *out_opt_comments, const char *file_path);

ply_err ply_simple_save_cloud(ply_SimpleCloud cloud, const char *file_path, enum ply_format format,
                              ply_comments *opt_comments);

#ifdef __cplusplus
}
#endif
#endif //PLYC_SIMPLE_LOAD_H
