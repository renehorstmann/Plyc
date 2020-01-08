#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "plyc/utilc/alloc.h"
#include "plyc/utilc/dynarray.h"
#include "plyc/header.h"
#include "plyc/data.h"

#include "plyc/simple.h"


DynArrayWithoutCopy(ply_vec3i, Vec3iArray)

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
    Free0(self->data);
    self->num = 0;
}

void ply_SimpleMeshIndices_kill(ply_SimpleMeshIndices *self) {
    Free0(self->indices);
    self->num = 0;
}


ply_err ply_simple_load(ply_SimpleCloud *out_points,
                        ply_SimpleCloud *out_opt_normals,
                        ply_SimpleCloud *out_opt_colors,
                        ply_SimpleMeshIndices *out_opt_indices,
                        ply_comments *out_opt_comments,
                        const char *file_path) {
    const int max_list_size = 12;

    ply_err err = PLY_SUCCESS;

    if (!out_points)
        return PLY_ILLEGAL_DATA;

    // set output clouds (indices) to zeros
    memset(out_points, 0, sizeof(ply_SimpleCloud));
    if (out_opt_normals)
        memset(out_opt_normals, 0, sizeof(ply_SimpleCloud));
    if (out_opt_colors)
        memset(out_opt_normals, 0, sizeof(ply_SimpleCloud));
    if (out_opt_indices)
        memset(out_opt_indices, 0, sizeof(ply_SimpleMeshIndices));

    // holding heap memory
    char *file_begin = NULL;
    ply_byte *parsed_vertices_data = NULL;
    ply_byte *parsed_indices_data = NULL;

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

    if (header.elements_size == 0 || header.elements_size > 2) SetErrGoto(err, "Wrong amount of elements", CLEAN_UP)

    // look up elements
    struct plyelement *vertices = &header.elements[0];
    struct plyelement *indices = NULL;
    {
        bool contains_a_list = false;
        for (int p = 0; p < vertices->properties_size; p++) {
            if (vertices->properties[p].list_type != PLY_TYPE_NONE) {
                contains_a_list = true;
                break;
            }
        }
        if (contains_a_list) SetErrGoto(err, "Vertices element not available, must be the first element", CLEAN_UP)

        if (header.elements_size == 2) {
            if (header.elements[1].properties_size != 1
                || header.elements[1].properties[0].list_type != PLY_TYPE_NONE) {
                SetErrGoto(err, "Indices element expected, should be second with a list property", CLEAN_UP)
            }
            indices = &header.elements[1];
        }
    }


    // copy and convert data
    parsed_vertices_data = TryNew(ply_byte, ply_data_element_size(*vertices, max_list_size));
    if (!parsed_vertices_data) SetErrGoto(err, "Allocation error", CLEAN_UP)
    err = ply_data_parse_element(parsed_vertices_data, &data_begin, file_end, *vertices, header.format,
                                 max_list_size);
    if (err) goto CLEAN_UP;

    if (indices) {
        parsed_indices_data = TryNew(ply_byte, ply_data_element_size(*indices, max_list_size));
        if (!parsed_indices_data) SetErrGoto(err, "Allocation error", CLEAN_UP)
        err = ply_data_parse_element(parsed_indices_data, &data_begin, file_end, *indices, header.format,
                                     max_list_size);
        if (err) goto CLEAN_UP;
    }


    // parse xyz
    plypropertydata x = ply_data_get_property(header, *vertices, "x", max_list_size);
    if (!x.stride) SetErrGoto(err, "property x not found", CLEAN_UP)

    plypropertydata y = ply_data_get_property(header, *vertices, "y", max_list_size);
    if (!y.stride) SetErrGoto(err, "property y not found", CLEAN_UP)

    plypropertydata z = ply_data_get_property(header, *vertices, "z", max_list_size);
    if (!z.stride) SetErrGoto(err, "property z not found", CLEAN_UP)


    out_points->num = vertices->num;
    out_points->data = TryNew(ply_vec4, vertices->num);
    if (!out_points->data) SetErrGoto(err, "Allocation error", CLEAN_UP)

    for (int i = 0; i < vertices->num; i++) {
        out_points->data[i][0] = ply_data_to_float(parsed_vertices_data + x.offset + x.stride * i, x.type);
        out_points->data[i][1] = ply_data_to_float(parsed_vertices_data + y.offset + y.stride * i, y.type);
        out_points->data[i][2] = ply_data_to_float(parsed_vertices_data + z.offset + z.stride * i, z.type);
        out_points->data[i][3] = 1;
    }

    // parse normals
    if (out_opt_normals) {
        plypropertydata nx = ply_data_get_property(header, *vertices, "nx", max_list_size);
        plypropertydata ny = ply_data_get_property(header, *vertices, "ny", max_list_size);
        plypropertydata nz = ply_data_get_property(header, *vertices, "nz", max_list_size);
        if (nx.stride > 0 && ny.stride > 0 && nz.stride > 0) {
            out_opt_normals->data = TryNew(ply_vec4, vertices->num);
            if (out_opt_normals->data) {
                out_opt_normals->num = vertices->num;
                for (int i = 0; i < vertices->num; i++) {
                    out_opt_normals->data[i][0] = ply_data_to_float(parsed_vertices_data + nx.offset + nx.stride * i,
                                                                    nx.type);
                    out_opt_normals->data[i][1] = ply_data_to_float(parsed_vertices_data + ny.offset + ny.stride * i,
                                                                    ny.type);
                    out_opt_normals->data[i][2] = ply_data_to_float(parsed_vertices_data + nz.offset + nz.stride * i,
                                                                    nz.type);
                    out_opt_normals->data[i][3] = 0;
                }
            }
        }
    }

    // parse colors
    if (out_opt_colors) {
        plypropertydata red = ply_data_get_property(header, *vertices, "red", max_list_size);
        if (!red.stride)
            red = ply_data_get_property(header, *vertices, "r", max_list_size);

        plypropertydata green = ply_data_get_property(header, *vertices, "green", max_list_size);
        if (!green.stride)
            green = ply_data_get_property(header, *vertices, "g", max_list_size);

        plypropertydata blue = ply_data_get_property(header, *vertices, "blue", max_list_size);
        if (!blue.stride)
            blue = ply_data_get_property(header, *vertices, "b", max_list_size);

        plypropertydata alpha = ply_data_get_property(header, *vertices, "alpha", max_list_size);
        if (!alpha.stride)
            alpha = ply_data_get_property(header, *vertices, "a", max_list_size);

        if (red.stride > 0 && green.stride > 0 && blue.stride > 0) {
            out_opt_colors->data = TryNew(ply_vec4, vertices->num);
            if (out_opt_colors->data) {
                out_opt_colors->num = vertices->num;
                for (int i = 0; i < vertices->num; i++) {
                    out_opt_colors->data[i][0] =
                            ply_data_to_float(parsed_vertices_data + red.offset + red.stride * i, red.type);
                    out_opt_colors->data[i][1] =
                            ply_data_to_float(parsed_vertices_data + green.offset + green.stride * i, green.type);
                    out_opt_colors->data[i][2] =
                            ply_data_to_float(parsed_vertices_data + blue.offset + blue.stride * i, blue.type);
                    if (alpha.stride) {
                        out_opt_colors->data[i][3] =
                                ply_data_to_float(parsed_vertices_data + alpha.offset + alpha.stride * i, alpha.type);
                    } else
                        out_opt_colors->data[i][3] = 1;
                }
            }
        }
    }

    // parse mesh indices
    if (out_opt_indices && indices) {
        plypropertydata faces = ply_data_get_property(header, *indices, indices->properties[0].name, max_list_size);
        assert(faces.stride > 0 && "faces must be found");

        Vec3iArray array = {0};
        Vec3iArray_set_capacity(&array, indices->num);

        for (int i = 0; i < indices->num; i++) {
            ply_byte *list_data = parsed_indices_data + faces.offset + faces.stride * i;

            // parse list size
            int list_size = ply_data_to_int(list_data, faces.list_type);
            if (list_size < 3 || list_size > 4) {
                fprintf(stderr, "[Plyc] warning: parsing mesh indices got a size of %d (should be 3|4), "
                                "face will be ignored!\n", list_size);
                continue;
            }
            list_data += ply_type_size(faces.list_type);

            // copy the triangle vertex indices
            ply_vec3i *face_a = Vec3iArray_append(&array);
            for (int v = 0; v < 3; v++) {
                (*face_a)[v] = ply_data_to_int(list_data, faces.type);
                list_data += ply_type_size(faces.type);
            }

            // add second triangle to match the quad
            if (list_size == 4) {
                ply_vec3i *face_b = Vec3iArray_append(&array);
                (*face_b)[0] = (*face_a)[0];
                (*face_b)[1] = (*face_a)[2];
                (*face_b)[2] = ply_data_to_int(list_data, faces.type);
            }
        }
    }


    CLEAN_UP:
    free(file_begin);
    free(parsed_vertices_data);
    free(parsed_indices_data);

    return err;
}


