#ifndef PLYC_HEADER_H
#define PLYC_HEADER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"


/**
 * Sets out_header_end to the start of the data (header end).
 * @param header_text: The string, containing the ply header.
 * @return: A ply_err if an error occurs, such as header end not found.
 */
ply_err ply_header_get_end(char **out_header_end, const char *header_text);


/**
 * Parses the header, loaded from a ply file to memory.
 * The header information will be set to the ply_File out_header.
 * All other fields will be set to zero (data fields such as data, offset, stride, ...).
 * @param header_text: The string, containing the ply header.
 * @return: A ply_err if an error occurs, such as not a ply file.
 */
ply_err ply_header_parse(ply_File *out_header, const char *header_text);


/**
 * Writes the header of the ply_File into an allocated string on the heap.
 * @return: A ply_err if an error occurs.
 */
ply_err ply_header_write_to_heap(char **out_header_on_heap, ply_File header);

#ifdef __cplusplus
}
#endif
#endif //PLYC_HEADER_H
