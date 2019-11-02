#ifndef PLYC_RCODES_H
#define PLYC_RCODES_H
#ifdef __cplusplus
extern "C" {
#endif

typedef const char *ply_err;

extern ply_err PLY_SUCCESS;
extern ply_err PLY_NULLPOINTER_ERROR;
extern ply_err PLY_NOT_A_PLY_FILE;
extern ply_err PLY_HEADER_ENDING_ERROR;
extern ply_err PLY_HEADER_FORMAT_ERROR;
extern ply_err PLY_HEADER_UNKNOWN_ITEM;
extern ply_err PLY_PROPERTY_BEFORE_ELEMENT;
extern ply_err PLY_TOO_MUCH_ELEMENTS;
extern ply_err PLY_TOO_MUCH_PROPERTIES;
extern ply_err PLY_ELEMENT_ERROR;
extern ply_err PLY_PROPERTY_ERROR;

#ifdef __cplusplus
}
#endif
#endif //PLYC_RCODES_H
