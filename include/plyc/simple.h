#ifndef PLYC_SIMPLE_LOAD_H
#define PLYC_SIMPLE_LOAD_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "types.h"

/** typedef for a float vector with 4 elements (xyzw, rgba, ...) */
typedef float ply_vec4[4];

/** typedef for an int vector with 3 elements (triangle abc indices) */
typedef int ply_vec3i[3];

/**
 * Class struct that contains the loaded simple cloud from a .ply file.
 * The field points, normals and colors contains the point values.
 * The field num is the number of points.
 * indices are the mesh indices that describe each mesh triangle with 3 point indices from above.
 * indices_size is the number of triangles.
 * comments and comments_size are the line comments of the ply header.
 * The field holds_heap_memory_ is set to true, if the loaded data is allocated on the heap (from ply_simple_load).
 * If ply_Simple is loaded by ply_simple_load, destruct it after usage with ply_Simple_kill.
 */
typedef struct ply_Simple {
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

/**
 * Destructor for ply_Simple.
 * Frees points, normals, colors and indices, if holds_heap_memory is true (from ply_simple_load).
 */
void ply_Simple_kill(ply_Simple *self);

/**
 * Loads and parses a .ply file.
 * @param filename: The file destination to load the .ply file
 * @return: A ply_err if an error occurs, such as file not found.
 */
ply_err ply_simple_load(ply_Simple *out_simple,
                        const char *filename);

/**
 * Writes and saves a .ply file.
 * @param simple: The filled up simple cloud.
 * @param filename: The file destination to load the .ply file
 * @param format: The ply format to save the data with
 * (one of PLY_FORMAT_ASCII, PLY_FORMAT_BINARY_LE, PLY_FORMAT_BINARY_BE).
 * @return: A ply_err if an error occurs, such as file permission error.
 */
ply_err ply_simple_save(ply_Simple simple,
                        const char *filename,
                        enum ply_format format);

#ifdef __cplusplus
}
#endif
#endif //PLYC_SIMPLE_LOAD_H
