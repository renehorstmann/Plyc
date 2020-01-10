#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "plyc/utilc/alloc.h"
#include "plyc/header.h"
#include "plyc/data.h"

#include "plyc/simple.h"

static void add_vertex_property(struct plyheader *header, const char *name) {
    struct plyheaderproperty *property = &header->elements[0].properties[header->elements[0].properties_size++];
    strcpy(property->name, name);
    property->list_type = PLY_TYPE_NONE;
    property->type = PLY_TYPE_FLOAT;
}

static void append_indices_element(struct plyheader *header, ply_SimpleMeshIndices indices) {
    header->elements_size++;
    strcpy(header->elements[1].name, "face"); // naming important (e. g. meshlab)
    header->elements[1].num = indices.num;
    header->elements[1].properties_size = 1;
    strcpy(header->elements[1].properties[0].name, "vertex_index"); // naming important (e. g. meshlab)
    header->elements[1].properties[0].list_type = PLY_TYPE_UCHAR;
    header->elements[1].properties[0].type = PLY_TYPE_INT;
}

static struct plyheader
create_minimal_header(ply_SimpleCloud points, enum ply_format format, ply_comments *opt_comments) {
    struct plyheader header;
    header.format = format;
    if (opt_comments) {
        memcpy(header.comments, opt_comments->comments, sizeof(header.comments));
        header.comments_size = opt_comments->comments_size;
    } else
        header.comments_size = 0;
    header.elements_size = 1;
    struct plyheaderelement *element = &header.elements[0];
    strcpy(element->name, "vertex"); // naming important (e. g. meshlab)
    element->num = points.num;
    element->properties_size = 0;

    add_vertex_property(&header, "x");
    add_vertex_property(&header, "y");
    add_vertex_property(&header, "z");

    return header;
}

static void add_vertex_propertydata(struct plydataelement *elementdata, size_t offset, size_t stride, ply_byte *data) {
    int index = elementdata->properties_size++;
    struct plydataproperty *property = &elementdata->properties[index];
    property->list_type = PLY_TYPE_NONE;
    property->type = PLY_TYPE_FLOAT;
    property->offset = (int) (offset * sizeof(float));
    property->stride = (int) (stride * sizeof(float));
    elementdata->properties_data[index] = (ply_byte *) data;
}

static struct plydataelement create_minimal_verticesdata(ply_SimpleCloud points) {
    struct plydataelement elementdata;
    elementdata.num = points.num;
    elementdata.properties_size = 0;

    add_vertex_propertydata(&elementdata, 0, 4, (ply_byte *) points.data);
    add_vertex_propertydata(&elementdata, 1, 4, (ply_byte *) points.data);
    add_vertex_propertydata(&elementdata, 2, 4, (ply_byte *) points.data);

    return elementdata;
}

static ply_err write_indices_to_heap(ply_byte **out_data_on_heap, size_t *out_element_size,
                                     ply_SimpleMeshIndices indices, int max_list_size, enum ply_format format) {
    struct plydataelement elementdata;
    elementdata.num = indices.num;
    elementdata.properties_size = 1;
    elementdata.properties[0].list_type = PLY_TYPE_UCHAR;
    elementdata.properties[0].type = PLY_TYPE_INT;
    elementdata.properties[0].offset = 0;
    elementdata.properties[0].stride = ply_type_size(PLY_TYPE_UCHAR) + max_list_size * ply_type_size(PLY_TYPE_INT);

    size_t buffer_size = elementdata.properties[0].stride * indices.num;
    char buffer[buffer_size];
    for (int i = 0; i < indices.num; i++) {
        char *it = buffer + elementdata.properties[0].stride * i;
        uint8_t *list_size = (uint8_t *) it;
        int32_t *data = (int32_t *) (it + sizeof(uint8_t));

        *list_size = 3;
        data[0] = indices.indices[i][0];
        data[1] = indices.indices[i][1];
        data[2] = indices.indices[i][2];
    }

    elementdata.properties_data[0] = (ply_byte *) buffer;

    return ply_data_write_element_to_heap(out_data_on_heap, out_element_size, elementdata, format);
}

ply_err ply_simple_save(ply_SimpleCloud points,
                        ply_SimpleCloud *opt_normals,
                        ply_SimpleCloud *opt_colors,
                        ply_SimpleMeshIndices *opt_indices,
                        ply_comments *opt_comments,
                        const char *file_path,
                        enum ply_format format) {
    const int max_list_size = 12;

    if (!points.data || points.num == 0)
        return "Points XYZ are missing";
    if (opt_normals) {
        if (!opt_normals->data || opt_normals->num != points.num)
            return "Normals are not valid";
    }
    if (opt_colors) {
        if (!opt_colors->data || opt_colors->num != points.num)
            return "Colors are not valid";
    }
    if (opt_indices) {
        if (!opt_indices->indices || opt_indices->num == 0)
            return "Indices are not valid";
    }

    ply_err err = PLY_Success;

    char *header_text = NULL;
    char *vertices_data_text = NULL;
    char *indices_data_text = NULL;
    FILE *file = NULL;


    struct plyheader header = create_minimal_header(points, format, opt_comments);
    struct plydataelement verticesdata = create_minimal_verticesdata(points);

    if (opt_normals) {
        add_vertex_property(&header, "nx");
        add_vertex_property(&header, "ny");
        add_vertex_property(&header, "nz");
        add_vertex_propertydata(&verticesdata, 0, 4, (ply_byte *) opt_normals->data);
        add_vertex_propertydata(&verticesdata, 1, 4, (ply_byte *) opt_normals->data);
        add_vertex_propertydata(&verticesdata, 2, 4, (ply_byte *) opt_normals->data);
    }
    if (opt_colors) {
        add_vertex_property(&header, "red");
        add_vertex_property(&header, "green");
        add_vertex_property(&header, "blue");
        add_vertex_propertydata(&verticesdata, 0, 4, (ply_byte *) opt_colors->data);
        add_vertex_propertydata(&verticesdata, 1, 4, (ply_byte *) opt_colors->data);
        add_vertex_propertydata(&verticesdata, 2, 4, (ply_byte *) opt_colors->data);
    }
    if (opt_indices) {
        append_indices_element(&header, *opt_indices);
    }

    err = ply_header_write_to_heap(&header_text, header);
    if (err)
        return err;

    size_t vertices_data_size;
    err = ply_data_write_element_to_heap(&vertices_data_text, &vertices_data_size, verticesdata, format);
    if (err) goto CLEAN_UP;

    size_t indices_data_size;
    if (opt_indices) {
        err = write_indices_to_heap(&indices_data_text, &indices_data_size, *opt_indices, max_list_size, format);
        if (err) goto CLEAN_UP;
    }

    file = fopen(file_path, "wb");
    if (!file) SetErrGoto(err, "Could not open file for writing", CLEAN_UP)

    fwrite(header_text, strlen(header_text), 1, file);
    fwrite(vertices_data_text, vertices_data_size, 1, file);
    if (opt_indices)
        fwrite(indices_data_text, indices_data_size, 1, file);

    CLEAN_UP:
    free(header_text);
    free(vertices_data_text);
    free(indices_data_text);
    if (file && fclose(file) != 0)
        return "File write error";

    return err;
}

