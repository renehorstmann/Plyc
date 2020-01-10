#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <locale.h>

#include "plyc/utilc/strviu.h"
#include "plyc/data.h"

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


static size_t list_size(const ply_byte *data, enum ply_type type) {
    if (type == PLY_TYPE_CHAR)
        return (size_t) (*(int8_t *) data);
    if (type == PLY_TYPE_UCHAR)
        return (size_t) (*(uint8_t *) data);
    if (type == PLY_TYPE_SHORT)
        return (size_t) (*(int16_t *) data);
    if (type == PLY_TYPE_USHORT)
        return (size_t) (*(uint16_t *) data);
    if (type == PLY_TYPE_INT)
        return (size_t) (*(int32_t *) data);
    if (type == PLY_TYPE_UINT)
        return (size_t) (*(uint32_t *) data);
    return 0;
}


static void parse_type_binary(ply_byte *restrict out_data,
                              ply_byte *restrict *ply_data,
                              enum ply_type type,
                              bool is_little_endian) {
    const ply_byte *data_as_system_endian = *ply_data;
    ply_byte switched[ply_type_size(type)];
    if (system_is_little_endian() != is_little_endian) {
        switch_endian(switched, *ply_data, ply_type_size(type));
        data_as_system_endian = switched;
    }
    memcpy(out_data, data_as_system_endian, ply_type_size(type));
    *ply_data += ply_type_size(type);
}

static void parse_type_ascii(ply_byte *restrict out_data,
                             ply_byte *restrict *ply_data,
                             enum ply_type type) {
    char *ascii = (char *) *ply_data;
    while (isspace(*ascii))
        ascii++;
    char *end = NULL;
    if (type == PLY_TYPE_CHAR) {
        int8_t *out = (int8_t *) out_data;
        *out = strtol(ascii, &end, 0);
    } else if (type == PLY_TYPE_UCHAR) {
        uint8_t *out = (uint8_t *) out_data;
        *out = strtoul(ascii, &end, 0);
    } else if (type == PLY_TYPE_SHORT) {
        int16_t *out = (int16_t *) out_data;
        *out = strtol(ascii, &end, 0);
    } else if (type == PLY_TYPE_USHORT) {
        uint16_t *out = (uint16_t *) out_data;
        *out = strtoul(ascii, &end, 0);
    } else if (type == PLY_TYPE_INT) {
        int32_t *out = (int32_t *) out_data;
        *out = strtol(ascii, &end, 0);
    } else if (type == PLY_TYPE_UINT) {
        uint32_t *out = (uint32_t *) out_data;
        *out = strtoul(ascii, &end, 0);
    } else if (type == PLY_TYPE_FLOAT) {
        float *out = (float *) out_data;
        *out = strtof(ascii, &end);
    } else if (type == PLY_TYPE_DOUBLE) {
        double *out = (double *) out_data;
        *out = strtod(ascii, &end);
    }
    if (!end || end == ascii)
        *ply_data = NULL;
    else
        *ply_data = (ply_byte *) end;
}

static void parse_type(ply_byte *restrict out_data,
                       ply_byte *restrict *ply_data,
                       enum ply_type type,
                       enum ply_format format) {
    if (format == PLY_FORMAT_ASCII)
        parse_type_ascii(out_data, ply_data, type);
    else if (format == PLY_FORMAT_BINARY_LE)
        parse_type_binary(out_data, ply_data, type, true);
    else if (format == PLY_FORMAT_BINARY_BE)
        parse_type_binary(out_data, ply_data, type, false);
    else
        *ply_data = NULL;
}

static ply_err parse_property(ply_byte *restrict out_data,
                              ply_byte *restrict *ply_data,
                              const ply_byte *ply_data_end,
                              struct plyproperty property,
                              enum ply_format format,
                              size_t max_list_size) {

    if (property.list_type != PLY_TYPE_NONE) {

        parse_type(out_data, ply_data, property.list_type, format);
        size_t size = list_size(out_data, property.list_type);
        if(size>max_list_size) {
            fprintf(stderr, "[Plyc] warning: parsing list property got a size of %zu that is above max_list_size (%zu)\n",
                    size, max_list_size);
            size = max_list_size;
        }
        out_data += ply_type_size(property.list_type);

        for (size_t i = 0; i < size; i++) {
            parse_type(out_data, ply_data, property.type, format);
            out_data += ply_type_size(property.type);
        }

        size_t remaining = max_list_size - size;
        memset(out_data, 0, remaining * ply_type_size(property.type));
    } else {
        parse_type(out_data, ply_data, property.type, format);
    }

    // ply_data should never be behind ply_data_end
    if(*ply_data > ply_data_end)
        return "Data buffer is too small";

    if (!(*ply_data)) {
        return "Data parse error";
    }

    return PLY_Success;
}


size_t ply_data_property_size(struct plyproperty property, size_t max_list_size) {
    if (property.list_type != PLY_TYPE_NONE)
        return ply_type_size(property.list_type) + max_list_size * ply_type_size(property.type);
    return ply_type_size(property.type);
}

size_t ply_data_element_size(struct plyelement element, size_t max_list_size) {
    size_t size = 0;
    for (size_t i = 0; i < element.properties_size; i++)
        size += ply_data_property_size(element.properties[i], max_list_size);

    return size * element.num;
}


ply_err ply_data_parse_element(ply_byte *restrict out_data,
                               ply_byte *restrict *ply_data_begin,
                               const ply_byte *ply_data_end,
                               struct plyelement element,
                               enum ply_format format,
                               size_t max_list_size) {
    if (element.properties_size > PLY_MAX_PROPERTIES)
        return "Property limit exceeded";

    setlocale(LC_ALL, "C");

    for (size_t i = 0; i < element.num; i++) {
        for (size_t p = 0; p < element.properties_size; p++) {
            ply_err err = parse_property(out_data, ply_data_begin, ply_data_end, element.properties[p], format, max_list_size);
            if(err) return err;
            out_data += ply_data_property_size(element.properties[p], max_list_size);
        }
    }

    return PLY_Success;
}
