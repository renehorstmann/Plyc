#ifndef PLYC_HEADER_H
#define PLYC_HEADER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include "rcodes.h"
#include "basetypes.h"

struct plyheaderproperty {
    char name[PLY_MAX_NAME_LENGTH];
    enum ply_type list_type;
    enum ply_type type;
} plyheaderproperty;

struct plyheaderelement {
    char name[PLY_MAX_NAME_LENGTH];
    size_t num;
    struct plyheaderproperty properties[PLY_MAX_PROPERTIES];
    size_t properties_size;
};

struct plyheader {
    enum ply_format format;
    char comments[PLY_MAX_COMMENTS][PLY_MAX_COMMENT_LENGTH];
    size_t comments_size;
    struct plyheaderelement elements[PLY_MAX_ELEMENTS];
    size_t elements_size;
};


/** Sets out_header_end to the start of the data (header end) */
ply_err ply_header_get_end(char **out_header_end, const char *header_text);


/**
 * Parses the header, loaded from a ply file to memory.
 * If an error occurs (such that the given file is not a ply file),
 * an ply_err will be set as return value (see rcodes.h).
 */
ply_err ply_header_parse(struct plyheader *out_header, const char *header_text);



/**
 * Writes the header into an allocates string on the heap
 * If an error occurs,
 * an ply_err will be set as return value (see rcodes.h).
 */
ply_err ply_header_write_to_heap(char **out_header_on_heap, struct plyheader header);


/**
 * Writes the header into the string buffer out_header
 * @param buffer_size: The max. size of the buffer/string out_header.
 * If an error occurs (such that the buffer is to small),
 * an ply_err will be set as return value (see rcodes.h).
 */
ply_err ply_header_write_into(char *out_header, int buffer_size, struct plyheader header);

#ifdef __cplusplus
}
#endif
#endif //PLYC_HEADER_H
