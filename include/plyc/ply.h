#ifndef PLYC_PLY_H
#define PLYC_PLY_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "rcodes.h"
#include "basetypes.h"

typedef struct {
    char name[PLY_MAX_NAME_LENGTH];
    enum ply_type list_type;
    enum ply_type type;
    ply_byte *data;
    int offset;
    int stride;
} plyproperty;

typedef struct {
    char name[PLY_MAX_NAME_LENGTH];
    size_t num;
    plyproperty properties[PLY_MAX_PROPERTIES];
    size_t properties_size;
} plyelement;

plyproperty *plyelement_get_property(plyelement *self, const char *property_name);

typedef struct {
    enum ply_format format;
    char comments[PLY_MAX_COMMENTS][PLY_MAX_COMMENT_LENGTH];
    size_t comments_size;
    plyelement elements[PLY_MAX_ELEMENTS];
    size_t elements_size;
} ply_File;

void ply_File_kill(ply_File *self);

plyelement *ply_File_get_element(ply_File *self, const char *element_name);


ply_err ply_parse_file(ply_File *out_file, const char *filename, int max_list_size);

ply_err ply_parse_memory(ply_File *out_file, const char *memory_begin, const char *memory_end, int max_list_size);





#ifdef __cplusplus
}
#endif
#endif //PLYC_PLY_H
