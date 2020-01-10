#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

#include "plyc/utilc/dynarray.h"
#include "plyc/header.h"

DynArray(char, CharArray)

static void push_string(CharArray *array, const char *string) {
    while (*string)
        CharArray_push(array, *string++);
}


static void push_element(CharArray *array, struct plyheaderelement *element) {
    push_string(array, "element ");
    push_string(array, element->name);
    CharArray_push(array, ' ');
    char num_str[12];   // max length for 32bit
    sprintf(num_str, "%zu", element->num);
    push_string(array, num_str);
    CharArray_push(array, '\n');
}

static void push_type(CharArray *array, enum ply_type type) {
    if (type == PLY_TYPE_CHAR)
        push_string(array, "char");
    else if (type == PLY_TYPE_UCHAR)
        push_string(array, "uchar");
    else if (type == PLY_TYPE_SHORT)
        push_string(array, "short");
    else if (type == PLY_TYPE_USHORT)
        push_string(array, "ushort");
    else if (type == PLY_TYPE_INT)
        push_string(array, "int");
    else if (type == PLY_TYPE_UINT)
        push_string(array, "uint");
    else if (type == PLY_TYPE_FLOAT)
        push_string(array, "float");
    else if (type == PLY_TYPE_DOUBLE)
        push_string(array, "double");
}

static void push_property(CharArray *array, struct plyheaderproperty *property) {

    push_string(array, "property ");

    if (property->list_type != PLY_TYPE_NONE) {
        push_string(array, "list ");
        push_type(array, property->list_type);
        CharArray_push(array, ' ');
    }

    push_type(array, property->type);
    CharArray_push(array, ' ');
    push_string(array, property->name);
    CharArray_push(array, '\n');
}

ply_err ply_header_write_into(char *out_header, int buffer_size, struct plyheader header) {
    char *header_on_heap;
    ply_err err = ply_header_write_to_heap(&header_on_heap, header);
    if (err)
        return err;

    size_t len = strlen(header_on_heap);
    if (len >= buffer_size) {
        free(header_on_heap);
        return "Header buffer is to small";
    }
    strcpy(out_header, header_on_heap);
    free(header_on_heap);
    return PLY_Success;
}

ply_err ply_header_write_to_heap(char **out_header_on_heap, struct plyheader header) {

    setlocale(LC_ALL, "C");

    ply_err err = PLY_Success;

    CharArray array = {0};
    CharArray_set_capacity(&array, 128);

    push_string(&array, "ply\n");

    if (header.format == PLY_FORMAT_ASCII)
        push_string(&array, "format ascii 1.0\n");
    else if (header.format == PLY_FORMAT_BINARY_LE)
        push_string(&array, "format binary_little_endian 1.0\n");
    else if (header.format == PLY_FORMAT_BINARY_BE)
        push_string(&array, "format binary_big_endian 1.0\n");
    else SetErrGoto(err, "Format error", CLEAN_UP)


    if (header.comments_size > PLY_MAX_COMMENT_LENGTH) SetErrGoto(err, "Comments error, too many comments", CLEAN_UP)
    for (size_t i = 0; i < header.comments_size; i++) {
        push_string(&array, "comment ");
        push_string(&array, header.comments[i]);
        CharArray_push(&array, '\n');
    }

    if (header.elements_size > PLY_MAX_ELEMENTS) SetErrGoto(err, "Element error, too many elements", CLEAN_UP)
    for (size_t i = 0; i < header.elements_size; i++) {
        struct plyheaderelement *element = &header.elements[i];
        if (element->properties_size > PLY_MAX_PROPERTIES) {
            SetErrGoto(err, "Property error, too many properties", CLEAN_UP)
        }

        for (size_t j = 0; j < i; j++) {
            if (strcmp(element->name, header.elements[j].name) == 0) {
                SetErrGoto(err, "Element name duplicate", CLEAN_UP)
            }
        }

        if (!isalpha(*element->name)) SetErrGoto(err, "Element name should start alpha numeric", CLEAN_UP)

        push_element(&array, element);

        for (size_t p = 0; p < element->properties_size; p++) {
            struct plyheaderproperty *property = &element->properties[p];

            for (size_t j = 0; j < p; j++) {
                if (strcmp(property->name, header.elements[i].properties[j].name) == 0) {
                    SetErrGoto(err, "Property name duplicate", CLEAN_UP)
                }
            }

            if (!isalpha(*property->name)) SetErrGoto(err, "Property name should start alpha numeric", CLEAN_UP)
            if (property->type == PLY_TYPE_NONE) SetErrGoto(err, "Property type must not be NONE", CLEAN_UP)

            push_property(&array, property);
        }
    }


    push_string(&array, "end_header\n");
    CharArray_push(&array, '\0');

    // move
    *out_header_on_heap = array.array;
    array.array = NULL;

    CLEAN_UP:
    CharArray_kill(&array);

    return err;
}

