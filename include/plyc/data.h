#ifndef PLYC_DATA_H
#define PLYC_DATA_H
#ifdef __cplusplus
extern "C" {
#endif

#include "header.h"


typedef char ply_byte;

typedef struct {
    int offset;
    int stride;
    enum ply_type type;
} plypropertydata;


float ply_data_to_float(const ply_byte *data, enum ply_type type);

int ply_data_to_int(const ply_byte *data, enum ply_type type);


/**
 * Returns the amount of bytes needed, to store one property member
 * @param max_list_size: maximum list size (list is [listsize, 0, 1, 2, 3, ..., max_list_size-1]
 */
size_t ply_data_property_size(struct plyproperty property, size_t max_list_size);


/**
 * Returns the amount of bytes needed, to store one element member
 * @param max_list_size: maximum list size (list is [listsize, 0, 1, 2, 3, ..., max_list_size-1]
 */
size_t ply_data_element_size(struct plyelement element, size_t max_list_size);

/**
 * Parses the data from ply_data into the data field out_data.
 * If an error occures,
 * an ply_err will be set as return value (see rcodes.h).
 * @param out_data: A data field with the size of the packed struct properties * num (f. e.: sizeof(float32) *3 * num)
 * @param max_list_size: maximum list size (list is [listsize, 0, 1, 2, 3, ..., max_list_size-1
 */
ply_err ply_data_parse_element(ply_byte *restrict out_data,
                               ply_byte *restrict *ply_data_begin,
                               const ply_byte *ply_data_end,
                               struct plyelement element,
                               enum ply_format format,
                               size_t max_list_size);

ply_err ply_data_write_element_to_heap(char **out_element_on_heap, struct plyelement element, enum ply_format format);


plypropertydata ply_data_get_property(plyheader header, struct plyelement element, const char *property_name,
                                      size_t max_list_size);


#ifdef __cplusplus
}
#endif
#endif //PLYC_DATA_H
