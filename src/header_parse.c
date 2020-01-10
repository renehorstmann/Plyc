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
        return "Element error, should only consist of name and num";

    if (sv_length(name) >= PLY_MAX_NAME_LENGTH - 1)
        return "Element error, name is to large";
    sv_cpy(out_element->name, name);

    char *end;
    long long tmp = strtoll(num.begin, &end, 0);
    if (end == viu.begin || !isspace(*end) || tmp <= 0)
        return "Element error, could not parse num";
    out_element->num = tmp;

    return PLY_Success;
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
            return "Property error, could not parse list type";
    }

    strviu type, name;
    type = sv_next_split(viu, ' ');
    viu.begin = type.end + 1;
    viu = sv_lstrip(viu, ' ');
    name = sv_next_split(viu, ' ');
    viu.begin = name.end + 1;
    viu = sv_lstrip(viu, ' ');

    if (!sv_empty(viu) || sv_empty(name))
        return "Property error, should only consist of a name";

    out_property->type = parse_type(type);
    if (out_property->type == PLY_TYPE_NONE)
        return "Property error, could not parse type";

    if (sv_length(name) >= PLY_MAX_NAME_LENGTH - 1)
        return "Property error, name is to large";
    sv_cpy(out_property->name, name);

    return PLY_Success;
}

ply_err ply_header_get_end(char **out_header_end, const char *header_text) {
    *out_header_end = strstr(header_text, "end_header");
    if(!*out_header_end)
        return "Could not find end_header";
    *out_header_end += strlen("end_header");
    while(*(*out_header_end)++ != '\n') {
        if(!**out_header_end)
            return "File ends behind end_header";
    }
    return PLY_Success;
}

ply_err ply_header_parse(plyheader *out_header, const char *header_text) {
    memset(out_header, 0, sizeof(plyheader));
    
    setlocale(LC_ALL, "C");

    if(strncmp(header_text, "ply", 3) != 0)
        return "Not a ply file";
    header_text += 3;

    char *header_text_end = strstr(header_text, "end_header");
    if(!header_text_end)
        return "Could not find end_header";

    strviu viu = {(char *) header_text, header_text_end};


    viu = sv_strip(viu, ' ');

    if (sv_begins_with_cstring(viu, "format ascii 1.0"))
        out_header->format = PLY_FORMAT_ASCII;
    else if (sv_begins_with_cstring(viu, "format binary_little_endian 1.0"))
        out_header->format = PLY_FORMAT_BINARY_LE;
    else if (sv_begins_with_cstring(viu, "format binary_big_endian 1.0"))
        out_header->format = PLY_FORMAT_BINARY_BE;
    else
        return "Error parsing format";

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
                return "Element error, too many elements";
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
                return "A property was parsed without an element";
            if (actual_element->properties_size >= PLY_MAX_PROPERTIES)
                return "Property error, too many properties";
            line.begin += strlen("property ");
            line = sv_lstrip(line, ' ');
            struct plyproperty *actual_property = &actual_element->properties[actual_element->properties_size++];
            ply_err err = parse_property(actual_property, line);
            if (err)
                return err;

            continue;
        }

        return "Unknown item in header";
    }


    return PLY_Success;
}







