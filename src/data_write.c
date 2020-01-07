#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <locale.h>

#include "plyc/utilc/strviu.h"
#include "plyc/utilc/dynarray.h"
#include "plyc/data.h"

DynArray(char, CharArray)

static bool system_is_little_endian() {
    int n = 1;
    // little endian if true
    return *(char *) &n == 1;
}

static void switch_endian(ply_byte *restrict dst, const ply_byte *restrict src, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[n - i - 1];
    }
}


static int type_size(enum ply_type type) {
    if (type == PLY_TYPE_CHAR || type == PLY_TYPE_UCHAR)
        return 1;
    if (type == PLY_TYPE_SHORT || type == PLY_TYPE_USHORT)
        return 2;
    if (type == PLY_TYPE_INT || type == PLY_TYPE_UINT || type == PLY_TYPE_FLOAT)
        return 4;
    if (type == PLY_TYPE_DOUBLE)
        return 8;
    return 0;
}


//static size_t list_size(const ply_byte *data, enum ply_type type) {
//    if (type == PLY_TYPE_CHAR)
//        return (size_t) (*(int8_t *) data);
//    if (type == PLY_TYPE_UCHAR)
//        return (size_t) (*(uint8_t *) data);
//    if (type == PLY_TYPE_SHORT)
//        return (size_t) (*(int16_t *) data);
//    if (type == PLY_TYPE_USHORT)
//        return (size_t) (*(uint16_t *) data);
//    if (type == PLY_TYPE_INT)
//        return (size_t) (*(int32_t *) data);
//    if (type == PLY_TYPE_UINT)
//        return (size_t) (*(uint32_t *) data);
//    return 0;
//}


static void write_type(CharArray *array, enum ply_type type, enum ply_format format, const ply_byte *data) {
    if (format == PLY_FORMAT_BINARY_LE) {
        CharArray_resize(array, array->size + type_size(type));
        char *buffer = &array->array[array->size - type_size(type)];
        if (system_is_little_endian())
            memcpy(buffer, data, type_size(type));
        else
            switch_endian(buffer, data, type_size(type));
    } else if (format == PLY_FORMAT_BINARY_BE) {
        CharArray_resize(array, array->size + type_size(type));
        char *buffer = &array->array[array->size - type_size(type)];
        if (!system_is_little_endian())
            memcpy(buffer, data, type_size(type));
        else
            switch_endian(buffer, data, type_size(type));
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

        CharArray_push_array(array, buffer, strlen(buffer));
        CharArray_push(array, ' ');
    }
}

static void write_list(CharArray *array, enum ply_type list_type, enum ply_type type, enum ply_format format,
                       const ply_byte *data, size_t n) {
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
        write_type(array, type, format, data + type_size(type) * i);
}

ply_err ply_data_write_element_to_heap(char **out_element_on_heap, size_t *out_element_size,
                                       plyelementdata element, enum ply_format format) {
    CharArray array = {0};
    CharArray_set_capacity(&array, element.num * element.properties_size);  // minimal size as start size

    for(size_t i=0; i<element.num; i++) {
        for(size_t p=0; p<element.properties_size; p++) {

            const ply_byte *data = element.properties_data[p] + element.properties[p].offset;
            element.properties[p].offset += element.properties[p].stride;

            if(element.properties[p].list_type == PLY_TYPE_NONE) {
                write_type(&array, element.properties[p].type, format, data);
            } else {
                int size = ply_data_to_int(data, element.properties[p].list_type);
                data += type_size(element.properties[p].list_type);
                write_list(&array, element.properties[p].list_type, element.properties[p].type, format, data, size);
            }
        }

        if(format == PLY_FORMAT_ASCII)
            CharArray_push(&array, '\n');
    }

    *out_element_size = array.size;

    if(format == PLY_FORMAT_ASCII)
        CharArray_push(&array, '\0');

    *out_element_on_heap = array.array;

    return PLY_SUCCESS;
}

