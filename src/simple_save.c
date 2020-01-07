#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "plyc/utilc/alloc.h"
#include "plyc/header.h"
#include "plyc/data.h"

#include "plyc/simple.h"


static plyheader create_header(ply_SimpleCloud cloud, enum ply_format format, ply_comments *opt_comments) {
    plyheader header;
    header.format = format;
    if (opt_comments) {
        memcpy(header.comments, opt_comments->comments, sizeof(header.comments));
        header.comments_size = opt_comments->comments_size;
    } else
        header.comments_size = 0;
    header.elements_size = 1;
    struct plyelement *element = &header.elements[0];
    strcpy(element->name, "vertices");
    element->num = cloud.size;
    element->properties_size = 0;

    for (int i = 0; i < 3; i++) {
        struct plyproperty *property = &element->properties[element->properties_size++];
        strcpy(property->name, (char *[]) {"x", "y", "z"}[i]);
        property->list_type = PLY_TYPE_NONE;
        property->type = PLY_TYPE_FLOAT;
    }

    if (cloud.normals) {
        for (int i = 0; i < 3; i++) {
            struct plyproperty *property = &element->properties[element->properties_size++];
            strcpy(property->name, (char *[]) {"nx", "ny", "nz"}[i]);
            property->list_type = PLY_TYPE_NONE;
            property->type = PLY_TYPE_FLOAT;
        }
    }

    if (cloud.colors) {
        for (int i = 0; i < 4; i++) {
            struct plyproperty *property = &element->properties[element->properties_size++];
            strcpy(property->name, (char *[]) {"red", "green", "blue", "alpha"}[i]);
            property->list_type = PLY_TYPE_NONE;
            property->type = PLY_TYPE_FLOAT;
        }
    }

    if (cloud.curvatures) {
        struct plyproperty *property = &element->properties[element->properties_size++];
        strcpy(property->name, "curvature");
        property->list_type = PLY_TYPE_NONE;
        property->type = PLY_TYPE_FLOAT;
    }

    return header;
}


static plyelementdata create_elementdata(ply_SimpleCloud cloud) {
    plyelementdata elementdata;
    elementdata.num = cloud.size;
    elementdata.properties_size = 0;

    for (int i = 0; i < 3; i++) {
        int index = elementdata.properties_size++;
        plypropertydata *property = &elementdata.properties[index];
        property->list_type = PLY_TYPE_NONE;
        property->type = PLY_TYPE_FLOAT;
        property->offset = i * sizeof(float);
        property->stride = 4 * sizeof(float);
        elementdata.properties_data[index] = (ply_byte *) cloud.points;
    }

    if (cloud.normals) {
        for (int i = 0; i < 3; i++) {
            int index = elementdata.properties_size++;
            plypropertydata *property = &elementdata.properties[index];
            property->list_type = PLY_TYPE_NONE;
            property->type = PLY_TYPE_FLOAT;
            property->offset = i * sizeof(float);
            property->stride = 4 * sizeof(float);
            elementdata.properties_data[index] = (ply_byte *) cloud.normals;
        }
    }

    if (cloud.colors) {
        for (int i = 0; i < 3; i++) {
            int index = elementdata.properties_size++;
            plypropertydata *property = &elementdata.properties[index];
            property->list_type = PLY_TYPE_NONE;
            property->type = PLY_TYPE_FLOAT;
            property->offset = i * sizeof(float);
            property->stride = 4 * sizeof(float);
            elementdata.properties_data[index] = (ply_byte *) cloud.colors;
        }
    }

    if (cloud.curvatures) {
        int index = elementdata.properties_size++;
        plypropertydata *property = &elementdata.properties[index];
        property->list_type = PLY_TYPE_NONE;
        property->type = PLY_TYPE_FLOAT;
        property->offset = 0;
        property->stride = sizeof(float);
        elementdata.properties_data[index] = (ply_byte *) cloud.curvatures;
    }

    return elementdata;
}

ply_err ply_simple_save_cloud(ply_SimpleCloud cloud, const char *file_path, enum ply_format format,
                              ply_comments *opt_comments) {
    if (!cloud.points)
        return PLY_ILLEGAL_DATA;

    ply_err err = PLY_SUCCESS;

    char *header_text = NULL;
    char *data_text = NULL;
    FILE *file = NULL;

    plyheader header = create_header(cloud, format, opt_comments);
    err = ply_header_write_to_heap(&header_text, header);
    if (err)
        return err;

    plyelementdata elementdata = create_elementdata(cloud);

    size_t data_size;
    err = ply_data_write_element_to_heap(&data_text, &data_size, elementdata, format);
    if (err) goto CLEAN_UP;

    file = fopen(file_path, "wb");
    if (!file) SetErrGoto(err, PLY_FILE_NOT_FOUND, CLEAN_UP)

    fwrite(header_text, strlen(header_text), 1, file);
    fwrite(data_text, data_size, 1, file);

    CLEAN_UP:
    free(header_text);
    free(data_text);
    if(file && fclose(file) != 0)
        return PLY_FILE_WRITE_ERROR;

    return err;
}

