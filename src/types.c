#include <string.h>
#include <stdint.h>

#include "plyc/utilc/alloc.h"
#include "plyc/types.h"

int ply_type_size(enum ply_type type) {
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

float ply_type_to_float(const ply_byte *data, enum ply_type type) {
    if (type == PLY_TYPE_FLOAT)
        return (float) *((float *) data);
    if (type == PLY_TYPE_DOUBLE)
        return (float) *((double *) data);
    if (type == PLY_TYPE_CHAR)
        return (float) *((int8_t *) data);
    if (type == PLY_TYPE_UCHAR)
        return (float) *((uint8_t *) data);
    if (type == PLY_TYPE_SHORT)
        return (float) *((int16_t *) data);
    if (type == PLY_TYPE_USHORT)
        return (float) *((uint16_t *) data);
    if (type == PLY_TYPE_INT)
        return (float) *((int32_t *) data);
    if (type == PLY_TYPE_UINT)
        return (float) *((uint32_t *) data);

    return 0;
}

int ply_type_to_int(const ply_byte *data, enum ply_type type) {
    if (type == PLY_TYPE_INT)
        return (int) *((int32_t *) data);
    if (type == PLY_TYPE_UINT)
        return (int) *((uint32_t *) data);
    if (type == PLY_TYPE_CHAR)
        return (int) *((int8_t *) data);
    if (type == PLY_TYPE_UCHAR)
        return (int) *((uint8_t *) data);
    if (type == PLY_TYPE_SHORT)
        return (int) *((int16_t *) data);
    if (type == PLY_TYPE_USHORT)
        return (int) *((uint16_t *) data);
    if (type == PLY_TYPE_FLOAT)
        return (int) *((float *) data);
    if (type == PLY_TYPE_DOUBLE)
        return (int) *((double *) data);

    return 0;
}

ply_property *plyelement_get_property(ply_element *self, const char *property_name) {
    for(int i=0; i<self->properties_size; i++) {
        if(strcmp(self->properties[i].name, property_name) == 0)
            return &self->properties[i];
    }
    return NULL;
}

void ply_File_kill(ply_File *self) {
    Free0(self->parsed_data_on_heap_);
    self->elements_size = 0;
    self->comments_size = 0;
}

ply_element *ply_File_get_element(ply_File *self, const char *element_name) {
    for(int i=0; i<self->elements_size; i++) {
        if(strcmp(self->elements[i].name, element_name) == 0)
            return &self->elements[i];
    }
    return NULL;
}
