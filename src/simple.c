#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "plyc/utilc/alloc.h"
#include "plyc/header.h"
#include "plyc/data.h"

#include "plyc/simple.h"

static void open_file_as_string(char **start, char **end, const char *filename) {
    char *text = NULL;
    long length = 0;
    FILE *file = fopen(filename, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);
        text = malloc(length + 1);
        if (text) {
            fread(text, 1, length, file);
            text[length] = '\0';
        }
        fclose(file);
    }
    *start = text;
    *end = text + length;
}

void ply_SimpleCloud_kill(ply_SimpleCloud *self) {
    Free0(self->points);
    Free0(self->normals);
    Free0(self->colors);
    Free0(self->curvatures);
    self->size = 0;
}


ply_err ply_simple_load_cloud(ply_SimpleCloud *out_cloud, ply_comments *out_opt_comments, const char *file_path) {
    const int max_list_size = 4;

    ply_err err = PLY_SUCCESS;

    memset(out_cloud, 0, sizeof(ply_SimpleCloud));

    // holding heap memory
    char *file_begin = NULL;
    ply_byte *parsed_data = NULL;

    // load file into memory
    char *file_end;
    open_file_as_string(&file_begin, &file_end, file_path);
    if (!file_begin)
        return PLY_FILE_NOT_FOUND;

    // parse header
    char *data_begin;
    err = ply_header_get_end(&data_begin, file_begin);
    if (err) goto CLEAN_UP;

    plyheader header;
    err = ply_header_parse(&header, file_begin);
    if (err) goto CLEAN_UP;

    // parse data
    struct plyelement *vertices = NULL;
    if (header.elements_size == 1)
        vertices = &header.elements[0];
    else {
        struct plyelement *el = &header.elements[0];
        bool contains_a_list = false;
        for (int p = 0; p < el->properties_size; p++) {
            if (el->properties[p].list_type != PLY_TYPE_NONE) {
                contains_a_list = true;
                break;
            }
        }
        if (!contains_a_list)
            vertices = el;
    }
    if (!vertices) SetErrGoto(err, "Vertices element not found, must be the first element", CLEAN_UP)

    plypropertydata x = ply_data_get_property(header, *vertices, "x", max_list_size);
    if (!x.stride) SetErrGoto(err, "property x not found", CLEAN_UP)

    plypropertydata y = ply_data_get_property(header, *vertices, "y", max_list_size);
    if (!y.stride) SetErrGoto(err, "property y not found", CLEAN_UP)

    plypropertydata z = ply_data_get_property(header, *vertices, "z", max_list_size);
    if (!z.stride) SetErrGoto(err, "property z not found", CLEAN_UP)

    plypropertydata nx = ply_data_get_property(header, *vertices, "nx", max_list_size);
    plypropertydata ny = ply_data_get_property(header, *vertices, "y", max_list_size);
    plypropertydata nz = ply_data_get_property(header, *vertices, "z", max_list_size);
    plypropertydata curvature = ply_data_get_property(header, *vertices, "curvature", max_list_size);
    plypropertydata red = ply_data_get_property(header, *vertices, "red", max_list_size);
    plypropertydata green = ply_data_get_property(header, *vertices, "green", max_list_size);
    plypropertydata blue = ply_data_get_property(header, *vertices, "blue", max_list_size);
    plypropertydata alpha = ply_data_get_property(header, *vertices, "alpha", max_list_size);




    // copy and convert data
    parsed_data = TryNew(ply_byte, ply_data_element_size(*vertices, max_list_size));
    if (!parsed_data) SetErrGoto(err, "Allocation error", CLEAN_UP)
    err = ply_data_parse_element(parsed_data, &data_begin, file_end, *vertices, header.format,
                                 max_list_size);
    if (err) goto CLEAN_UP;

    out_cloud->size = vertices->num;
    out_cloud->points = TryNew(ply_vec4, vertices->num);
    if (!out_cloud->points) SetErrGoto(err, "Allocation error", CLEAN_UP)
    if (nx.stride > 0 && ny.stride > 0 && nz.stride > 0)
        out_cloud->normals = TryNew(ply_vec4, vertices->num);
    if (red.stride > 0 && green.stride > 0 && blue.stride > 0)
        out_cloud->colors = TryNew(ply_vec4, vertices->num);
    if (curvature.stride > 0)
        out_cloud->curvatures = TryNew(float, vertices->num);


    for (int i = 0; i < vertices->num; i++) {
        out_cloud->points[i][0] = ply_data_to_float(parsed_data + x.offset + x.stride * i, x.type);
        out_cloud->points[i][1] = ply_data_to_float(parsed_data + y.offset + y.stride * i, y.type);
        out_cloud->points[i][2] = ply_data_to_float(parsed_data + z.offset + z.stride * i, z.type);
        out_cloud->points[i][3] = 1;
    }

    if(out_cloud->normals) {
        for (int i = 0; i < vertices->num; i++) {
            out_cloud->normals[i][0] = ply_data_to_float(parsed_data + nx.offset + nx.stride * i, nx.type);
            out_cloud->normals[i][1] = ply_data_to_float(parsed_data + ny.offset + ny.stride * i, ny.type);
            out_cloud->normals[i][2] = ply_data_to_float(parsed_data + nz.offset + nz.stride * i, nz.type);
            out_cloud->normals[i][3] = 0;
        }
    }

    if(out_cloud->colors) {
        for (int i = 0; i < vertices->num; i++) {
            out_cloud->colors[i][0] = ply_data_to_float(parsed_data + red.offset + red.stride * i, red.type);
            out_cloud->colors[i][1] = ply_data_to_float(parsed_data + green.offset + green.stride * i, green.type);
            out_cloud->colors[i][2] = ply_data_to_float(parsed_data + blue.offset + blue.stride * i, blue.type);
            out_cloud->colors[i][3] = 1;
        }
        if(alpha.stride>0) {
            for (int i = 0; i < vertices->num; i++)
                out_cloud->colors[i][3] =
                        ply_data_to_float(parsed_data + alpha.offset + alpha.stride * i, alpha.type);
        }
    }

    if(out_cloud->curvatures) {
        for (int i = 0; i < vertices->num; i++)
            out_cloud->curvatures[i] =
                    ply_data_to_float(parsed_data + curvature.offset + curvature.stride * i, curvature.type);
    }

    CLEAN_UP:
    free(file_begin);
    free(parsed_data);

    return err;
}
