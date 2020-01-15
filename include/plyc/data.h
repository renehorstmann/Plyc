#ifndef PLYC_DATA_H
#define PLYC_DATA_H
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"


/**
 * Parses the data, loaded from a ply file to memory (ply_data + ply_data_size).
 * The data information will be set to the ply_File in_out_file.
 * For the parsing, the ply_File in_out_file must contain all necessary information to parse the data.
 * So call ply_header_parse first.
 * The parameter max_list_size sets the maximum list_size for a list property.
 * All lists use parsed memory with the max_list_size, to get the feature of striding over a complete list.
 * (Next property is always x bytes behind the start of any list).
 * If an error occurs (such that the given memory size is to small,
 * an ply_err will be set as return value.
 */
ply_err ply_data_parse(ply_File *in_out_file,
                       const ply_byte *restrict ply_data, size_t ply_data_size,
                       size_t max_list_size);

/**
 * Writes the data of the ply_File into an allocated memory array (out_data_on_heap + out_data_size) on the heap.
 * If an error occurs,
 * an ply_err will be set as return value.
 */
ply_err ply_data_write_to_heap(char **out_data_on_heap, size_t *out_data_size, ply_File file);


#ifdef __cplusplus
}
#endif
#endif //PLYC_DATA_H
