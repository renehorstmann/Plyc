#ifndef PLYC_DATA_H
#define PLYC_DATA_H
#ifdef __cplusplus
extern "C" {
#endif

#include "header.h"


typedef char ply_byte;

/**
 * Returns the amount of bytes needed, to store one property member
 * @param max_list_size: maximum list size (list is [listsize, 0, 1, 2, 3, ..., max_list_size-1]
 */
size_t ply_property_size(struct plyproperty property, enum ply_format format, size_t max_list_size);

/**
 * Returns the amount of bytes needed, to store one element member
 * @param max_list_size: maximum list size (list is [listsize, 0, 1, 2, 3, ..., max_list_size-1]
 */
size_t ply_element_size(struct plyelement element, enum ply_format format, size_t max_list_size);

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


#ifdef __cplusplus
}
#endif
#endif //PLYC_DATA_H
