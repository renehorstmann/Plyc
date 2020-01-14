#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "plyc/utilc/alloc.h"
#include "plyc/utilc/dynarray.h"
#include "plyc/ply.h"

#include "plyc/simple.h"


DynArrayWithoutCopy(ply_vec3i, Vec3iArray)


void ply_Simple_kill(ply_Simple *self) {
    if (self->holds_heap_memory_) {
        Free0(self->points);
        Free0(self->normals);
        Free0(self->colors);
        Free0(self->indices);
        self->num = 0;
        self->indices_size = 0;
        self->comments_size = 0;
    }
}


ply_err ply_simple_load(ply_Simple *out_simple,
                        const char *filename) {
    const int max_list_size = 4;

    ply_err err = PLY_Success;


    ply_File file;
    err = ply_load_file(&file, filename, max_list_size);
    if (err) return err;


    ply_Simple simple = {0};
    simple.holds_heap_memory_ = true;
    simple.comments_size = file.comments_size;
    memcpy(simple.comments, file.comments, sizeof(file.comments));

    if (file.elements_size < 1 || file.elements_size > 2) {
        PlySetErrGoto(err, "Wrong number of elements, should be 1 or 2", CLEAN_UP)
    }

    plyelement *vertex = &file.elements[0];
    plyelement *face = NULL;
    if (file.elements_size == 2)
        face = &file.elements[1];

    plyproperty *x = plyelement_get_property(vertex, "x");
    plyproperty *y = plyelement_get_property(vertex, "y");
    plyproperty *z = plyelement_get_property(vertex, "z");
    plyproperty *nx = plyelement_get_property(vertex, "nx");;
    plyproperty *ny = plyelement_get_property(vertex, "ny");;
    plyproperty *nz = plyelement_get_property(vertex, "nz");;
    plyproperty *r = plyelement_get_property(vertex, "red");
    plyproperty *g = plyelement_get_property(vertex, "green");
    plyproperty *b = plyelement_get_property(vertex, "blue");

    if (!x || !y || !z) {
        PlySetErrGoto(err, "XYZ are missing in the ply file", CLEAN_UP)
    }

    simple.num = vertex->num;
    simple.points = TryNew(ply_vec3, vertex->num);
    if (!simple.points) {
        PlySetErrGoto(err, "Allocation Error", CLEAN_UP);
    }
    for (int i = 0; i < vertex->num; i++) {
        simple.points[i][0] = ply_type_to_float(x->data + x->offset + x->stride * i, x->type);
        simple.points[i][1] = ply_type_to_float(y->data + y->offset + y->stride * i, y->type);
        simple.points[i][2] = ply_type_to_float(z->data + z->offset + z->stride * i, z->type);
    }

    if (nx && ny && nz) {
        simple.normals = TryNew(ply_vec3, vertex->num);
        if (simple.normals) {
            for (int i = 0; i < vertex->num; i++) {
                simple.normals[i][0] = ply_type_to_float(nx->data + nx->offset + nx->stride * i, nx->type);
                simple.normals[i][1] = ply_type_to_float(ny->data + ny->offset + ny->stride * i, ny->type);
                simple.normals[i][2] = ply_type_to_float(nz->data + nz->offset + nz->stride * i, nz->type);
            }
        }
    }

    if (r && g && b) {
        simple.colors = TryNew(ply_vec3, vertex->num);
        if (simple.colors) {
            float scale = 1;
            if (r->type != PLY_TYPE_DOUBLE && r->type != PLY_TYPE_FLOAT)
                scale /= 255;
            for (int i = 0; i < vertex->num; i++) {
                simple.colors[i][0] = scale * ply_type_to_float(r->data + r->offset + r->stride * i, r->type);
                simple.colors[i][1] = scale * ply_type_to_float(g->data + g->offset + g->stride * i, g->type);
                simple.colors[i][2] = scale * ply_type_to_float(b->data + b->offset + b->stride * i, b->type);
            }
        }
    }

    if (face && face->properties_size == 1 && face->properties[0].list_type != PLY_TYPE_NONE) {
        plyproperty *indices = &face->properties[0];
        Vec3iArray array = {0};
        Vec3iArray_set_capacity(&array, face->num);
        if (array.array) {
            for (int i = 0; i < face->num; i++) {
                ply_byte *list_data = indices->data + indices->offset + indices->stride * i;

                // parse list size
                int list_size = ply_type_to_int(list_data, indices->list_type);
                if (list_size < 3 || list_size > 4) {
                    fprintf(stderr, "[Plyc] warning: parsing mesh indices got a size of %d (should be 3|4), "
                                    "face will be ignored!\n", list_size);
                    continue;
                }
                list_data += ply_type_size(indices->list_type);

                // copy the triangle vertex indices
                ply_vec3i *face_a = Vec3iArray_append(&array);
                for (int v = 0; v < 3; v++) {
                    (*face_a)[v] = ply_type_to_int(list_data, indices->type);
                    list_data += ply_type_size(indices->type);
                }

                // add second triangle to match the quad
                if (list_size == 4) {
                    ply_vec3i *face_b = Vec3iArray_append(&array);
                    (*face_b)[0] = (*face_a)[0];
                    (*face_b)[1] = (*face_a)[2];
                    (*face_b)[2] = ply_type_to_int(list_data, indices->type);
                }
            }

            // move array
            simple.indices = array.array;
            simple.indices_size = array.size;
        }
    }

    // move
    *out_simple = simple;
    simple = (ply_Simple) {0};

    CLEAN_UP:
    ply_File_kill(&file);
    free(simple.points);
    free(simple.normals);
    free(simple.colors);
    free(simple.indices);
    return err;
}


