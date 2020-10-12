#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "plyc/utilc/alloc.h"
#include "plyc/ply.h"

#include "plyc/simple.h"

PlyFile header_from_simple(PlySimple simple, enum ply_format format) {
    PlyFile header = {0};
    header.format = format;
    header.comments_size = simple.comments_size;
    memcpy(header.comments, simple.comments, sizeof(simple.comments));

    header.elements_size = 1;
    PlyElement_s *vertex = &header.elements[0];
    strcpy(vertex->name, "vertex");
    vertex->num = simple.num;

    vertex->properties_size = 0;
    for (int i = 0; i < 3; i++) {
        int index = vertex->properties_size++;
        PlyProperty_s *property = &vertex->properties[index];
        strcpy(property->name, (const char *[]) {"x", "y", "z"}[i]);
        property->list_type = PLY_TYPE_NONE;
        property->type = PLY_TYPE_FLOAT;
        property->data = (ply_byte *) simple.points;
        property->offset = (int) (i * sizeof(float));
        property->stride = (int) ((char *) &simple.points[1][0] - (char *) &simple.points[0][0]);
    }

    if (simple.normals) {
        for (int i = 0; i < 3; i++) {
            int index = vertex->properties_size++;
            PlyProperty_s *property = &vertex->properties[index];
            strcpy(property->name, (const char *[]) {"nx", "ny", "nz"}[i]);
            property->list_type = PLY_TYPE_NONE;
            property->type = PLY_TYPE_FLOAT;
            property->data = (ply_byte *) simple.normals;
            property->offset = (int) (i * sizeof(float));
            property->stride = (int) ((char *) &simple.normals[1][0] - (char *) &simple.normals[0][0]);
        }
    }

    if (simple.colors) {
        for (int i = 0; i < 3; i++) {
            int index = vertex->properties_size++;
            PlyProperty_s *property = &vertex->properties[index];
            strcpy(property->name, (const char *[]) {"red", "green", "blue"}[i]);
            property->list_type = PLY_TYPE_NONE;
            property->type = PLY_TYPE_FLOAT;
            property->data = (ply_byte *) simple.colors;
            property->offset = (int) (i * sizeof(float));
            property->stride = (int) ((char *) &simple.colors[1][0] - (char *) &simple.colors[0][0]);
        }
    }

    if (simple.indices && simple.indices_size > 0) {
        header.elements_size = 2;
        PlyElement_s *face = &header.elements[1];
        strcpy(face->name, "face");
        face->num = simple.indices_size;
        face->properties_size = 1;
        PlyProperty_s *vertex_indices = &face->properties[0];
        strcpy(vertex_indices->name, "vertex_indices");
        vertex_indices->list_type = PLY_TYPE_UCHAR;
        vertex_indices->type = PLY_TYPE_INT;
    }

    return header;
}

ply_err add_indices_data(PlyFile *file, PlySimple simple) {
    ply_err err = PLY_Success;

    if(file->elements_size != 2)
        return PLY_Success;

    int stride = ply_type_size(PLY_TYPE_UCHAR) + 3 * ply_type_size(PLY_TYPE_INT);
    int buffer_size = simple.indices_size * stride;
    file->parsed_data_on_heap_ = TryNew(ply_byte, buffer_size);
    if(!file->parsed_data_on_heap_)
        return "Allocation error";

    file->elements[1].properties[0].data = file->parsed_data_on_heap_;
    file->elements[1].properties[0].offset = 0;
    file->elements[1].properties[0].stride = stride;

    for(int i=0; i<simple.indices_size; i++) {
        uint8_t *size = (uint8_t *) (file->parsed_data_on_heap_ + stride * i);
        *size = 3;
        size++;
        int32_t *data = (int32_t *) size;
        for(int abc=0; abc<3; abc++) {
            *data = simple.indices[i][abc];
            data++;
        }
    }

    return err;
}

ply_err ply_simple_save(PlySimple self,
                        const char *filename,
                        enum ply_format format) {
    ply_err err = PLY_Success;

    PlyFile file = header_from_simple(self, format);

    // adds tmp indices to files heap field
    err = add_indices_data(&file, self);
    if (err)
        return err;

    err = ply_save_file(file, filename);
    ply_file_kill(&file);   // to kill the tmp indices on its heap field
    return err;
}

