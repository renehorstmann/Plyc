#include <string.h>
#include <stdio.h>

#include <plyc/utilc/alloc.h>
#include "plyc/ply.h"

#include "plyc/header.h"
#include "plyc/data.h"

static void open_file_as_string(char **start, char **end, const char *filename) {
    char *text = NULL;
    long length = 0;
    FILE *file = fopen(filename, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);
        text = malloc(length + 1);
        if (text) {
            size_t chars_read = fread(text, 1, length, file);
            if(chars_read != length)
                fprintf(stderr, "open file warning, didnt read enough characters!\n");
            text[length] = '\0';
        }
        fclose(file);
    }
    *start = text;
    *end = text + length;
}

plyproperty *plyelement_get_property(plyelement *self, const char *property_name) {
    for(int i=0; i<self->properties_size; i++) {
        if(strcmp(self->properties[i].name, property_name) == 0)
            return &self->properties[i];
    }
    return NULL;
}

void ply_File_kill(ply_File *self) {
    // buffer start begins in the first element (same for all properties)
    if(self->elements_size>=1 && self->elements[0].properties_size>=1)
        Free0(self->elements[0].properties[0].data);
    self->elements_size = 0;
    self->comments_size = 0;
}

plyelement *ply_File_get_element(ply_File *self, const char *element_name) {
    for(int i=0; i<self->elements_size; i++) {
        if(strcmp(self->elements[i].name, element_name) == 0)
            return &self->elements[i];
    }
    return NULL;
}

ply_err ply_parse_file(ply_File *out_file, const char *filename, int max_list_size) {
    char *memory_begin, *memory_end;
    open_file_as_string(&memory_begin, &memory_end, filename);
    ply_err res = ply_parse_memory(out_file, memory_begin, memory_end, max_list_size);
    free(memory_begin);
    return res;
}

ply_err ply_parse_memory(ply_File *out_file, const char *memory_begin, const char *memory_end, int max_list_size) {
    ply_err err;

    // parse header
    struct plyheader header;
    err = ply_header_parse(&header, memory_begin);
    if (err) return err;

    // copy header infos
    out_file->format = header.format;
    out_file->comments_size = header.comments_size;
    memcpy(out_file->comments, header.comments, sizeof(out_file->comments));
    out_file->elements_size = header.elements_size;
    for (int e = 0; e < header.elements_size; e++) {
        plyelement *to_element = &out_file->elements[e];
        struct plyheaderelement *from_element = &header.elements[e];
        strcpy(to_element->name, from_element->name);
        to_element->num = from_element->num;
        to_element->properties_size = from_element->properties_size;
        for (int p = 0; p < from_element->properties_size; p++) {
            plyproperty *to_property = &to_element->properties[p];
            struct plyheaderproperty *from_property = &from_element->properties[p];
            strcpy(to_property->name, from_property->name);
            to_property->list_type = from_property->list_type;
            to_property->type = from_property->type;
        }
    }

    // get data begin
    char *data_begin;
    err = ply_header_get_end(&data_begin, memory_begin);
    if (err) return err;

    // sum memory size for one large memory block
    size_t buffer_size = 0;
    for (int i = 0; i < header.elements_size; i++) {
        buffer_size += ply_data_element_size(header.elements[i], max_list_size);
    }
    if (buffer_size <= 0)
        return "Empty elements";

    ply_byte *buffer = TryNew(ply_byte, buffer_size);
    if (!buffer)
        return "Allocation error";

    // parse data into the memory block and set up the elements
    char *active_buffer_block = buffer;
    for (int e = 0; e < header.elements_size; e++) {
        err = ply_data_parse_element(active_buffer_block, &data_begin, memory_end,
                                     header.elements[e], header.format, max_list_size);
        if (err) goto CLEAN_UP;

        for (int p = 0; p < header.elements[e].properties_size; p++) {
            struct plydataproperty from_property = ply_data_get_property(header, header.elements[e],
                                                                          header.elements[e].properties[p].name,
                                                                          max_list_size);
            assert(from_property.stride != 0 && "property not found with its own name");
            plyproperty *to_property = &out_file->elements[e].properties[p];
            to_property->data = active_buffer_block;
            to_property->stride = from_property.stride;
            to_property->offset = from_property.offset;
        }

        active_buffer_block += ply_data_element_size(header.elements[e], max_list_size);
    }

    // move
    // buffer is set to the first element
    assert(buffer == out_file->elements[0].properties[0].data);
    buffer = NULL;

    CLEAN_UP:
    free(buffer);

    return err;
}
