#ifndef PLYC_RCODES_H
#define PLYC_RCODES_H
#ifdef __cplusplus
extern "C" {
#endif


#define SetErrGoto(err, set, label) { (err) = (set); goto label; }

typedef const char *ply_err;

extern ply_err PLY_SUCCESS;
extern ply_err PLY_NULLPOINTER_ERROR;
extern ply_err PLY_ILLEGAL_DATA;

extern ply_err PLY_FILE_NOT_FOUND;
extern ply_err PLY_FILE_WRITE_ERROR;
extern ply_err PLY_NOT_A_PLY_FILE;
extern ply_err PLY_HEADER_ENDING_ERROR;
extern ply_err PLY_HEADER_FORMAT_ERROR;
extern ply_err PLY_HEADER_UNKNOWN_ITEM;
extern ply_err PLY_PROPERTY_BEFORE_ELEMENT;
extern ply_err PLY_TOO_MUCH_ELEMENTS;
extern ply_err PLY_TOO_MUCH_PROPERTIES;
extern ply_err PLY_ELEMENT_ERROR;
extern ply_err PLY_PROPERTY_ERROR;
extern ply_err PLY_HEADER_BUFFER_TO_SMALL;
extern ply_err PLY_ELEMENT_NAME_ERROR;
extern ply_err PLY_PROPERTY_NAME_ERROR;
extern ply_err PLY_HEADER_ILLEGAL_NAME;
extern ply_err PLY_DATA_PARSE_ERROR;
extern ply_err PLY_LIST_SIZE_TO_BIG;
extern ply_err PLY_NOT_ENOUGH_DATA;


#ifdef __cplusplus
}
#endif
#endif //PLYC_RCODES_H
