#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <locale.h>

#include "plyc/utilc/strviu.h"
#include "plyc/utilc/alloc.h"
#include "plyc/data.h"

#include "os_helper.h"


static int list_size(const ply_byte *data, enum ply_type type) {
    if (type == PLY_TYPE_CHAR)
        return (int) (*(int8_t *) data);
    if (type == PLY_TYPE_UCHAR)
        return (int) (*(uint8_t *) data);
    if (type == PLY_TYPE_SHORT)
        return (int) (*(int16_t *) data);
    if (type == PLY_TYPE_USHORT)
        return (int) (*(uint16_t *) data);
    if (type == PLY_TYPE_INT)
        return (int) (*(int32_t *) data);
    if (type == PLY_TYPE_UINT)
        return (int) (*(uint32_t *) data);
    return 0;
}


static void parse_type_binary(ply_byte *restrict out_data,
                              int *out_consumed_bytes,
                              const ply_byte *restrict actual_ply_data,
                              enum ply_type type,
                              bool is_little_endian) {
    const ply_byte *data_as_system_endian = actual_ply_data;
    ply_byte switched[ply_type_size(type)];
    if (system_is_little_endian() != is_little_endian) {
        switch_endian(switched, actual_ply_data, ply_type_size(type));
        data_as_system_endian = switched;
    }
    memcpy(out_data, data_as_system_endian, ply_type_size(type));
    *out_consumed_bytes = ply_type_size(type);
}

static void parse_type_ascii(ply_byte *restrict out_data,
                             int *out_consumed_bytes,
                             const ply_byte *restrict actual_ply_data,
                             enum ply_type type) {
    const char *ascii = (const char *) actual_ply_data;
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
        *out_consumed_bytes = 0;
    else
        *out_consumed_bytes = (const ply_byte *) end - actual_ply_data;
}

static void parse_type(ply_byte *restrict out_data,
                       int *out_consumed_bytes,
                       const ply_byte *restrict actual_ply_data,
                       enum ply_type type,
                       enum ply_format format) {
    if (format == PLY_FORMAT_ASCII)
        parse_type_ascii(out_data, out_consumed_bytes, actual_ply_data, type);
    else if (format == PLY_FORMAT_BINARY_LE)
        parse_type_binary(out_data, out_consumed_bytes, actual_ply_data, type, true);
    else if (format == PLY_FORMAT_BINARY_BE)
        parse_type_binary(out_data, out_consumed_bytes, actual_ply_data, type, false);
    else
        *out_consumed_bytes = 0;
}

static ply_err parse_property(ply_byte *restrict out_data,
                              int *out_consumed_bytes,
                              const ply_byte *restrict actual_ply_data,
                              const ply_byte *ply_data_end,
                              PlyProperty_s property,
                              enum ply_format format,
                              int max_list_size) {

    if (property.list_type != PLY_TYPE_NONE) {

        int consumed_bytes = 0;
        parse_type(out_data, &consumed_bytes, actual_ply_data, property.list_type, format);
        actual_ply_data += consumed_bytes;
        *out_consumed_bytes += consumed_bytes;

        int size = list_size(out_data, property.list_type);
        if (size > max_list_size) {
            fprintf(stderr,
                    "[Plyc] warning: parsing list property got a size of %d that is above max_list_size (%d)\n",
                    size, max_list_size);
            size = max_list_size;
        }
        out_data += ply_type_size(property.list_type);

        for (int i = 0; i < size; i++) {
            consumed_bytes = 0;
            parse_type(out_data, &consumed_bytes, actual_ply_data, property.type, format);
            out_data += ply_type_size(property.type);
            actual_ply_data += consumed_bytes;
            *out_consumed_bytes += consumed_bytes;
        }

        int remaining = max_list_size - size;
        memset(out_data, 0, remaining * ply_type_size(property.type));
    } else {
        parse_type(out_data, out_consumed_bytes, actual_ply_data, property.type, format);
        actual_ply_data += *out_consumed_bytes;
    }

    // ply_data should never be behind ply_data_end
    if (actual_ply_data > ply_data_end)
        return "Data buffer is too small";

    if (*out_consumed_bytes == 0) {
        return "Data parse error";
    }

    return PLY_Success;
}


static int property_size(PlyProperty_s property, int max_list_size) {
    if (property.list_type != PLY_TYPE_NONE)
        return ply_type_size(property.list_type) + max_list_size * ply_type_size(property.type);
    return ply_type_size(property.type);
}

static int element_size(PlyElement_s element, int max_list_size) {
    int size = 0;
    for (int i = 0; i < element.properties_size; i++)
        size += property_size(element.properties[i], max_list_size);

    return size * element.num;
}


static ply_err parse_element(ply_byte *restrict out_data,
                             int *out_consumed_bytes,
                             const ply_byte *restrict actual_ply_data,
                             const ply_byte *ply_data_end,
                             PlyElement_s element,
                             enum ply_format format,
                             int max_list_size) {
    if (element.properties_size > PLY_MAX_PROPERTIES)
        return "Property limit exceeded";

    for (int i = 0; i < element.num; i++) {
        for (int p = 0; p < element.properties_size; p++) {
            int consumed_bytes = 0;
            ply_err err = parse_property(out_data, &consumed_bytes,
                                         actual_ply_data, ply_data_end, element.properties[p],
                                         format, max_list_size);
            if (err) return err;
            out_data += property_size(element.properties[p], max_list_size);
            actual_ply_data += consumed_bytes;
            *out_consumed_bytes += consumed_bytes;
        }
    }

    return PLY_Success;
}


ply_err ply_data_parse(PlyFile *in_out_file,
                       const ply_byte *restrict ply_data, int ply_data_size,
                       int max_list_size) {
    ply_err err = PLY_Success;

    setlocale(LC_ALL, "C");

    const ply_byte *ply_data_end = ply_data + ply_data_size;

    int buffer_size = 0;
    for (int e = 0; e < in_out_file->elements_size; e++)
        buffer_size += element_size(in_out_file->elements[e], max_list_size);

    ply_byte *buffer = TryNew(ply_byte, buffer_size);
    if (!buffer)
        return "Allocation error";

    ply_byte *actual_data = buffer;

    for (int e = 0; e < in_out_file->elements_size; e++) {
        PlyElement_s *element = &in_out_file->elements[e];

        int consumed_bytes = 0;
        err = parse_element(actual_data, &consumed_bytes,
                                    ply_data, ply_data_end, *element,
                                    in_out_file->format, max_list_size);
        if (err) goto CLEAN_UP;

        // set up ply properties...
        for (int p = 0; p < element->properties_size; p++) {
            PlyProperty_s *prop = &element->properties[p];
            prop->data = actual_data;

            prop->offset = 0;
            for (int i = 0; i < p; i++)
                prop->offset += property_size(element->properties[i], max_list_size);

            prop->stride = 0;
            for (int i = 0; i < element->properties_size; i++)
                prop->stride += property_size(element->properties[i], max_list_size);
        }

        ply_data += consumed_bytes;
        actual_data += element_size(in_out_file->elements[e], max_list_size);
    }

    // move
    in_out_file->parsed_data_on_heap_ = buffer;
    buffer = NULL;

    CLEAN_UP:
    free(buffer);

    return err;
}

