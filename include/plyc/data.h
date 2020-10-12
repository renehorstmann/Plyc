#ifndef PLYC_DATA_H
#define PLYC_DATA_H
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"


/**
 * Parses the data, loaded from a ply file to memory (ply_data + ply_data_size).
 * The data information will be set to the PlyFile in_out_file.
 * For the parsing, the PlyFile in_out_file must contain all necessary information to parse the data.
 * So call ply_header_parse first.
 * The parameter max_list_size sets the maximum list_size for a list property.
 * All lists use parsed memory with the max_list_size, to get the feature of striding over a complete list.
 * (Next property is always x bytes behind the start of any list).
 * @param ply_data: The ply file data as string/memory.
 * @param ply_data_size: The size of the string/memory.
 * @param max_list_size: Maximal size of each property list. (meshes uses a size of 3 or 4 for triangle or quads).
 * @return: A ply_err if an error occurs, such as memory size is too small.
 */
ply_err ply_data_parse(PlyFile *in_out_file,
                       const ply_byte *restrict ply_data, int ply_data_size,
                       int max_list_size);

/**
 * Writes the data of the PlyFile into an allocated memory array (out_data_on_heap + out_data_size) on the heap.
 * @param file: the filled up PlyFile to write the data from.
 * @return: A ply_err if an error occurs..
 */
ply_err ply_data_write_to_heap(char **out_data_on_heap, int *out_data_size, PlyFile file);


#ifdef __cplusplus
}
#endif
#endif //PLYC_DATA_H
