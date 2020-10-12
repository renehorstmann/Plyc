#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

#include "plyc/utilc/dynarray.h"
#include "plyc/header.h"

DynArray(char, CharArray, char_array)

static void push_string(CharArray *array, const char *string) {
    while (*string)
        char_array_push(array, *string++);
}


static void push_element(CharArray *array, PlyElement_s *element) {
    push_string(array, "element ");
    push_string(array, element->name);
    char_array_push(array, ' ');
    char num_str[12];   // max length for 32bit
    sprintf(num_str, "%d", element->num);
    push_string(array, num_str);
    char_array_push(array, '\n');
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

static void push_property(CharArray *array, PlyProperty_s *property) {

    push_string(array, "property ");

    if (property->list_type != PLY_TYPE_NONE) {
        push_string(array, "list ");
        push_type(array, property->list_type);
        char_array_push(array, ' ');
    }

    push_type(array, property->type);
    char_array_push(array, ' ');
    push_string(array, property->name);
    char_array_push(array, '\n');
}

ply_err ply_header_write_to_heap(char **out_header_on_heap, PlyFile header) {

    setlocale(LC_ALL, "C");

    ply_err err = PLY_Success;

    CharArray array = {0};
    char_array_set_capacity(&array, 128);

    push_string(&array, "ply\n");

    if (header.format == PLY_FORMAT_ASCII)
        push_string(&array, "format ascii 1.0\n");
    else if (header.format == PLY_FORMAT_BINARY_LE)
        push_string(&array, "format binary_little_endian 1.0\n");
    else if (header.format == PLY_FORMAT_BINARY_BE)
        push_string(&array, "format binary_big_endian 1.0\n");
    else PlySetErrGoto(err, "Format error", CLEAN_UP)


    if (header.comments_size > PLY_MAX_COMMENT_LENGTH) PlySetErrGoto(err, "Comments error, too many comments", CLEAN_UP)
    for (int i = 0; i < header.comments_size; i++) {
        push_string(&array, "comment ");
        push_string(&array, header.comments[i]);
        char_array_push(&array, '\n');
    }

    if (header.elements_size > PLY_MAX_ELEMENTS) PlySetErrGoto(err, "Element error, too many elements", CLEAN_UP)
    for (int i = 0; i < header.elements_size; i++) {
        PlyElement_s *element = &header.elements[i];
        if (element->properties_size > PLY_MAX_PROPERTIES) {
            PlySetErrGoto(err, "Property error, too many properties", CLEAN_UP)
        }

        for (int j = 0; j < i; j++) {
            if (strcmp(element->name, header.elements[j].name) == 0) {
                PlySetErrGoto(err, "Element name duplicate", CLEAN_UP)
            }
        }

        if (!isalpha(*element->name)) PlySetErrGoto(err, "Element name should start alpha numeric", CLEAN_UP)

        push_element(&array, element);

        for (int p = 0; p < element->properties_size; p++) {
            PlyProperty_s *property = &element->properties[p];

            for (int j = 0; j < p; j++) {
                if (strcmp(property->name, header.elements[i].properties[j].name) == 0) {
                    PlySetErrGoto(err, "Property name duplicate", CLEAN_UP)
                }
            }

            if (!isalpha(*property->name)) PlySetErrGoto(err, "Property name should start alpha numeric", CLEAN_UP)
            if (property->type == PLY_TYPE_NONE) PlySetErrGoto(err, "Property type must not be NONE", CLEAN_UP)

            push_property(&array, property);
        }
    }


    push_string(&array, "end_header\n");
    char_array_push(&array, '\0');

    // move
    *out_header_on_heap = array.array;
    array.array = NULL;

    CLEAN_UP:
    char_array_kill(&array);

    return err;
}

