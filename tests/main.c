#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include <plyc/header.h>

typedef float ply_vec4[4];

typedef struct {
    ply_vec4 *points;
    ply_vec4 *normals;
    ply_vec4 *colors;
    float *curvatures;
    size_t size;
} ply_SimpleCloud;

void ply_SimpleCloud_kill(ply_SimpleCloud *self) {

}

ply_err ply_load_simple_cloud(ply_SimpleCloud *out_cloud, const char *file_path);


int main() {

}
