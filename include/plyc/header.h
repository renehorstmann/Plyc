#ifndef PLYC_HEADER_H
#define PLYC_HEADER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"


/**
 * Sets out_header_end to the start of the data (header end).
 * header_text must be a string, containing the ply header.
 * If not found, an ply_err will be set as return value.
 */
ply_err ply_header_get_end(char **out_header_end, const char *header_text);


/**
 * Parses the header, loaded from a ply file to memory.
 * If an error occurs (such that the given file is not a ply file),
 * an ply_err will be set as return value.
 */
ply_err ply_header_parse(ply_File *out_header, const char *header_text);


/**
 * Writes the header into an allocated string on the heap.
 * If an error occurs,
 * an ply_err will be set as return value.
 */
ply_err ply_header_write_to_heap(char **out_header_on_heap, ply_File header);

#ifdef __cplusplus
}
#endif
#endif //PLYC_HEADER_H
