#ifndef PLYC_TYPES_H
#define PLYC_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#define PLY_MAX_NAME_LENGTH 32
#define PLY_MAX_PROPERTIES 16
#define PLY_MAX_ELEMENTS 8
#define PLY_MAX_COMMENT_LENGTH 128
#define PLY_MAX_COMMENTS 8

/** ply_err for successful operation, is a NULL pointer */
#define PLY_Success (ply_err) 0

// Macro to set err and jump to a label
#define PlySetErrGoto(err, set, label) { (err) = (set); goto label; }


typedef const char *ply_err;
typedef char ply_byte;

enum ply_format {
    PLY_FORMAT_NONE,
    PLY_FORMAT_ASCII,
    PLY_FORMAT_BINARY_LE,
    PLY_FORMAT_BINARY_BE,
    PLY_FORMAT_NUM_ELEMENTS
};

enum ply_type {
    PLY_TYPE_NONE,
    PLY_TYPE_CHAR,
    PLY_TYPE_UCHAR,
    PLY_TYPE_SHORT,
    PLY_TYPE_USHORT,
    PLY_TYPE_INT,
    PLY_TYPE_UINT,
    PLY_TYPE_FLOAT,
    PLY_TYPE_DOUBLE,
    PLY_TYPE_NUM_ELEMENTS
};

int ply_type_size(enum ply_type type);

float ply_type_to_float(const ply_byte *data, enum ply_type type);

int ply_type_to_int(const ply_byte *data, enum ply_type type);

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
    ply_byte *parsed_data_on_heap_;
} ply_File;


void ply_File_kill(ply_File *self);

plyelement *ply_File_get_element(ply_File *self, const char *element_name);


#ifdef __cplusplus
}
#endif
#endif //PLYC_TYPES_H
