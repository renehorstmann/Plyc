#ifndef PLYC_BASETYPES_H
#define PLYC_BASETYPES_H
#ifdef __cplusplus
extern "C" {
#endif

#define PLY_MAX_NAME_LENGTH 32
#define PLY_MAX_PROPERTIES 16
#define PLY_MAX_ELEMENTS 8
#define PLY_MAX_COMMENT_LENGTH 128
#define PLY_MAX_COMMENTS 8


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


#ifdef __cplusplus
}
#endif
#endif //PLYC_BASETYPES_H
