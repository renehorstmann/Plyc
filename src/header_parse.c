#include <locale.h>

#include "plyc/utilc/strviu.h"
#include "plyc/header.h"

static ply_err parse_element(struct plyelement *out_element, strviu viu) {
    memset(out_element, 0, sizeof(struct plyelement));

    strviu name, num;

    name = sv_next_split(viu, ' ');
    viu.begin = name.end + 1;
    viu = sv_lstrip(viu, ' ');
    num = sv_next_split(viu, ' ');
    viu.begin = num.end + 1;
    viu = sv_lstrip(viu, ' ');
    if (!sv_empty(viu) || sv_empty(name) || sv_empty(num))
        return PLY_ELEMENT_ERROR;

    if (sv_length(name) >= PLY_MAX_NAME_LENGTH - 1)
        return PLY_ELEMENT_ERROR;
    sv_cpy(out_element->name, name);

    char *end;
    long long tmp = strtoll(num.begin, &end, 0);
    if (end == viu.begin || !isspace(*end) || tmp <= 0)
        return PLY_ELEMENT_ERROR;
    out_element->num = tmp;

    return PLY_SUCCESS;
}

static enum ply_type parse_type(strviu viu) {
    if (sv_equals_cstring(viu, "char") || sv_equals_cstring(viu, "int8"))
        return PLY_TYPE_CHAR;
    else if (sv_equals_cstring(viu, "uchar") || sv_equals_cstring(viu, "uint8"))
        return PLY_TYPE_UCHAR;
    else if (sv_equals_cstring(viu, "short") || sv_equals_cstring(viu, "int16"))
        return PLY_TYPE_SHORT;
    else if (sv_equals_cstring(viu, "ushort") || sv_equals_cstring(viu, "uint16"))
        return PLY_TYPE_USHORT;
    else if (sv_equals_cstring(viu, "int") || sv_equals_cstring(viu, "int32"))
        return PLY_TYPE_INT;
    else if (sv_equals_cstring(viu, "uint") || sv_equals_cstring(viu, "uint32"))
        return PLY_TYPE_UINT;
    else if (sv_equals_cstring(viu, "float") || sv_equals_cstring(viu, "float32"))
        return PLY_TYPE_FLOAT;
    else if (sv_equals_cstring(viu, "double") || sv_equals_cstring(viu, "float64"))
        return PLY_TYPE_DOUBLE;

    return PLY_TYPE_NONE;
}

static ply_err parse_property(struct plyproperty *out_property, strviu viu) {
    memset(out_property, 0, sizeof(struct plyproperty));

    if (sv_begins_with_cstring(viu, "list ")) {
        viu.begin += strlen("list ");
        viu = sv_lstrip(viu, ' ');
        strviu list_type = sv_next_split(viu, ' ');
        viu.begin = list_type.end + 1;
        viu = sv_lstrip(viu, ' ');
        out_property->list_type = parse_type(list_type);
        if (out_property->list_type == PLY_TYPE_NONE)
            return PLY_PROPERTY_ERROR;
    }

    strviu type, name;
    type = sv_next_split(viu, ' ');
    viu.begin = type.end + 1;
    viu = sv_lstrip(viu, ' ');
    name = sv_next_split(viu, ' ');
    viu.begin = name.end + 1;
    viu = sv_lstrip(viu, ' ');

    if (!sv_empty(viu) || sv_empty(name))
        return PLY_PROPERTY_ERROR;

    out_property->type = parse_type(type);
    if (out_property->type == PLY_TYPE_NONE)
        return PLY_PROPERTY_ERROR;

    if (sv_length(name) >= PLY_MAX_NAME_LENGTH - 1)
        return PLY_PROPERTY_ERROR;
    sv_cpy(out_property->name, name);

    return PLY_SUCCESS;
}


ply_err header_parse(plyheader *out_header, const char *header_text) {
    memset(out_header, 0, sizeof(plyheader));
    
    setlocale(LC_ALL, "C");

    if(!header_text)
        return PLY_NULLPOINTER_ERROR;

    strviu viu = ToStrViu(header_text);

    if (!sv_begins_with_cstring(viu, "ply"))
        return PLY_NOT_A_PLY_FILE;
    if (!sv_ends_with_cstring(viu, "end_header"))
        return PLY_HEADER_ENDING_ERROR;

    viu.begin += strlen("ply");
    viu.end -= strlen("end_header");

    viu = sv_strip(viu, ' ');

    if (sv_begins_with_cstring(viu, "format ascii 1.0"))
        out_header->format = PLY_FORMAT_ASCII;
    else if (sv_begins_with_cstring(viu, "format binary_little_endian 1.0"))
        out_header->format = PLY_FORMAT_BINARY_LE;
    else if (sv_begins_with_cstring(viu, "format binary_big_endian 1.0"))
        out_header->format = PLY_FORMAT_BINARY_BE;
    else
        return PLY_HEADER_FORMAT_ERROR;

    viu = sv_eat_until(viu, '\n');
    viu = sv_lstrip(viu, ' ');

    // now each line could be one of [comment, element, property] (property before element is an error)
    struct plyelement *actual_element = NULL;

    while (viu.begin < viu.end) {
        strviu line = sv_next_split(viu, '\n');
        viu.begin = line.end + 1;

        line = sv_strip(line, ' ');
        if (sv_empty(line))
            continue;

        if (sv_begins_with_cstring(line, "comment")) {
            if (out_header->comments_size >= PLY_MAX_COMMENTS)
                continue;
            line.begin += strlen("comment");
            line = sv_lstrip(line, ' ');
            size_t len = sv_length(line);
            if (len == 0 || len >= PLY_MAX_COMMENT_LENGTH - 1)
                continue;
            sv_cpy(out_header->comments[out_header->comments_size++], line);

            continue;
        }

        if (sv_begins_with_cstring(line, "element ")) {
            if (out_header->elements_size >= PLY_MAX_ELEMENTS)
                return PLY_TOO_MUCH_ELEMENTS;
            line.begin += strlen("element ");
            line = sv_lstrip(line, ' ');
            actual_element = &out_header->elements[out_header->elements_size++];
            ply_err err = parse_element(actual_element, line);
            if (err)
                return err;

            continue;
        }

        if (sv_begins_with_cstring(line, "property ")) {
            if (!actual_element)
                return PLY_PROPERTY_BEFORE_ELEMENT;
            if (actual_element->properties_size >= PLY_MAX_PROPERTIES)
                return PLY_TOO_MUCH_PROPERTIES;
            line.begin += strlen("property ");
            line = sv_lstrip(line, ' ');
            struct plyproperty *actual_property = &actual_element->properties[actual_element->properties_size++];
            ply_err err = parse_property(actual_property, line);
            if (err)
                return err;

            continue;
        }

        return PLY_HEADER_UNKNOWN_ITEM;
    }


    return PLY_SUCCESS;
}







