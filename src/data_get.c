#include <string.h>
#include <stdint.h>

#include "plyc/data.h"


float ply_data_to_float(const ply_byte *data, enum ply_type type) {
    if(type == PLY_TYPE_FLOAT)
        return (float) *((float *) data);
    if(type == PLY_TYPE_DOUBLE)
        return (float) *((double *) data);
    if(type == PLY_TYPE_CHAR)
        return (float) *((int8_t *) data);
    if(type == PLY_TYPE_UCHAR)
        return (float) *((uint8_t *) data);
    if(type == PLY_TYPE_SHORT)
        return (float) *((int16_t *) data);
    if(type == PLY_TYPE_USHORT)
        return (float) *((uint16_t *) data);
    if(type == PLY_TYPE_INT)
        return (float) *((int32_t *) data);
    if(type == PLY_TYPE_UINT)
        return (float) *((uint32_t *) data);

    return 0;
}

int ply_data_to_int(const ply_byte *data, enum ply_type type) {
    if(type == PLY_TYPE_INT)
        return (int) *((int32_t *) data);
    if(type == PLY_TYPE_UINT)
        return (int) *((uint32_t *) data);
    if(type == PLY_TYPE_CHAR)
        return (int) *((int8_t *) data);
    if(type == PLY_TYPE_UCHAR)
        return (int) *((uint8_t *) data);
    if(type == PLY_TYPE_SHORT)
        return (int) *((int16_t *) data);
    if(type == PLY_TYPE_USHORT)
        return (int) *((uint16_t *) data);
    if(type == PLY_TYPE_FLOAT)
        return (int) *((float *) data);
    if(type == PLY_TYPE_DOUBLE)
        return (int) *((double *) data);

    return 0;
}


plypropertydata ply_data_get_property(plyheader header, struct plyelement element, const char *property_name,
                                      size_t max_list_size) {
    int property_index = 0;
    struct plyproperty *property = NULL;
    for(int i=0; i < element.properties_size; i++) {
        if(strcmp(element.properties[i].name, property_name) == 0) {
            property_index = i;
            property = &element.properties[i];
            break;
        }
    }
    if(!property)
        return (plypropertydata) {0};


    plypropertydata res = {0};

    res.type = property->type;
    res.list_type = property->list_type;

    for(int i=0; i<property_index; i++)
        res.offset += ply_data_property_size(element.properties[i], max_list_size);

    for(int i=0; i<element.properties_size; i++)
        res.stride += ply_data_property_size(element.properties[i], max_list_size);

    return res;
}


