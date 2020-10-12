#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <locale.h>

#include "plyc/utilc/alloc.h"
#include "plyc/utilc/dynarray.h"
#include "plyc/data.h"

#include "os_helper.h"

DynArray(char, CharArray, char_array)


static void write_type(CharArray *array, enum ply_type type, enum ply_format format, const ply_byte *data) {
    if (format == PLY_FORMAT_BINARY_LE) {
        char_array_resize(array, array->size + ply_type_size(type));
        char *buffer = &array->array[array->size - ply_type_size(type)];
        if (system_is_little_endian())
            memcpy(buffer, data, ply_type_size(type));
        else
            switch_endian(buffer, data, ply_type_size(type));
    } else if (format == PLY_FORMAT_BINARY_BE) {
        char_array_resize(array, array->size + ply_type_size(type));
        char *buffer = &array->array[array->size - ply_type_size(type)];
        if (!system_is_little_endian())
            memcpy(buffer, data, ply_type_size(type));
        else
            switch_endian(buffer, data, ply_type_size(type));
    } else {
        char buffer[64];
        if (type == PLY_TYPE_CHAR)
            sprintf(buffer, "%d", *((int8_t *) data));
        else if (type == PLY_TYPE_UCHAR)
            sprintf(buffer, "%d", *((uint8_t *) data));
        else if (type == PLY_TYPE_SHORT)
            sprintf(buffer, "%d", *((int16_t *) data));
        else if (type == PLY_TYPE_USHORT)
            sprintf(buffer, "%d", *((uint16_t *) data));
        else if (type == PLY_TYPE_INT)
            sprintf(buffer, "%d", *((int32_t *) data));
        else if (type == PLY_TYPE_UINT)
            sprintf(buffer, "%d", *((uint32_t *) data));
        else if (type == PLY_TYPE_FLOAT)
            sprintf(buffer, "%f", *((float *) data));
        else if (type == PLY_TYPE_DOUBLE)
            sprintf(buffer, "%f", *((double *) data));

        char_array_push_array(array, buffer, strlen(buffer));
        char_array_push(array, ' ');
    }
}

static void write_list(CharArray *array, enum ply_type list_type, enum ply_type type, enum ply_format format,
                       const ply_byte *data, int n) {
    char list_size[4];  // max type length for list a list size (int or uint)
    if (list_type == PLY_TYPE_CHAR)
        *((int8_t *) list_size) = (int8_t) n;
    else if (list_type == PLY_TYPE_UCHAR)
        *((uint8_t *) list_size) = (uint8_t) n;
    else if (list_type == PLY_TYPE_SHORT)
        *((int16_t *) list_size) = (int16_t) n;
    else if (list_type == PLY_TYPE_USHORT)
        *((uint16_t *) list_size) = (uint16_t) n;
    else if (list_type == PLY_TYPE_INT)
        *((int32_t *) list_size) = (int32_t) n;
    else if (list_type == PLY_TYPE_UINT)
        *((uint32_t *) list_size) = (uint32_t) n;
    else
        *((int32_t *) list_size) = 0;   // wtf

    write_type(array, list_type, format, list_size);

    for (int i = 0; i < n; i++)
        write_type(array, type, format, data + ply_type_size(type) * i);
}

static ply_err write_element_to_heap(char **out_element_on_heap, int *out_element_size,
                                     PlyElement_s element, enum ply_format format) {
    CharArray array = {0};
    char_array_set_capacity(&array, element.num * element.properties_size);  // minimal size as start size
    if (!array.array)
        return "Allocation error";

    for (int i = 0; i < element.num; i++) {
        for (int p = 0; p < element.properties_size; p++) {
            PlyProperty_s *property = &element.properties[p];
            const ply_byte *data = property->data + property->offset + property->stride * i;

            if (property->list_type == PLY_TYPE_NONE) {
                write_type(&array, property->type, format, data);
            } else {
                int size = ply_type_to_int(data, property->list_type);
                data += ply_type_size(property->list_type);
                write_list(&array, property->list_type, property->type, format, data, size);
            }
        }

        if (format == PLY_FORMAT_ASCII)
            char_array_push(&array, '\n');
    }

    *out_element_size = array.size;

    if (format == PLY_FORMAT_ASCII)
        char_array_push(&array, '\0');

    *out_element_on_heap = array.array;

    return PLY_Success;
}


ply_err ply_data_write_to_heap(char **out_data_on_heap, int *out_data_size, PlyFile file) {
    ply_err err = PLY_Success;

    if(file.elements_size <= 0)
        return "Elements error, no elements available to write";

    char *data_array[file.elements_size];
    int size_array[file.elements_size];
    int buffer_size = 0;
    int size = 0;

    for (int e = 0; e < file.elements_size; e++) {
        err = write_element_to_heap(&data_array[e], &size_array[e], file.elements[e], file.format);
        if (err)
            goto CLEAN_UP;

        buffer_size += size_array[e];
        size++;
    }

    // copy to single buffer
    *out_data_on_heap = TryNew(char, buffer_size);
    if(!*out_data_on_heap)
        PlySetErrGoto(err, "Allocation error", CLEAN_UP);

    char *actual_data = *out_data_on_heap;
    for(int i=0; i<size; i++) {
        memcpy(actual_data, data_array[i], size_array[i]);
        actual_data += size_array[i];
    }
    *out_data_size = buffer_size;

    CLEAN_UP:
    for (int i = 0; i < size; i++)
        free(data_array[i]);

    return err;
}

