#ifndef PLYC_PLY_H
#define PLYC_PLY_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "rcodes.h"
#include "basetypes.h"

struct plyproperty {
    char name[PLY_MAX_NAME_LENGTH];
    enum ply_type list_type;
    enum ply_type type;
    ply_byte *data;
    int offset;
    int stride;
};

struct plyelement {
    char name[PLY_MAX_NAME_LENGTH];
    size_t num;
    struct plyproperty properties[PLY_MAX_PROPERTIES];
    size_t properties_size;
};

typedef struct {
    enum ply_format format;
    char comments[PLY_MAX_COMMENTS][PLY_MAX_COMMENT_LENGTH];
    size_t comments_size;
    struct plyelement elements[PLY_MAX_ELEMENTS];
    size_t elements_size;
} plyfile;


ply_err ply_parse(plyfile *out_file, const char *filedata_begin, const char *filedata_end);

struct plyproperty *ply_plyelement_get_property(struct plyelement element, const char *property_name);

struct plyelement *ply_plyfile_get_element(struct plyfile file, const char *element_name);



#ifdef __cplusplus
}
#endif
#endif //PLYC_PLY_H
