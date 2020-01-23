#ifndef PLYC_TYPES_H
#define PLYC_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/** Maximal name length for an header item */
#define PLY_MAX_NAME_LENGTH 32
/** Maximal amount of properties for each element */
#define PLY_MAX_PROPERTIES 16
/** Maximal amount of elements */
#define PLY_MAX_ELEMENTS 8
/** Maximal size of a line comment -> 127 chars + \0 */
#define PLY_MAX_COMMENT_LENGTH 128
/** Maximal amount of comment lines in the header file */
#define PLY_MAX_COMMENTS 8


/** ply_err for successful operation (NULL pointer) */
#define PLY_Success (ply_err) 0

/** Macro to set err and jump to a label.
 * @param err: ply_err to set.
 * @param set: the message to set in err (err = set).
 * @param label: the label to jump to (goto label).
 */
#define PlySetErrGoto(err, set, label) { (err) = (set); goto label; }

/**
 * An ply error is a pointer to a stack string.
 * If an error occurs, the user can print the error message and also see it directly in the debug session.
 * @example:
 * ply_err err = ply_example_function();
 * if(err) {
 *      fprintf(stderr, "error: %s", err);
 *      exit(EXIT_FAILURE);
 * }
 */
typedef const char *ply_err;

/** Simple type redefinition to distinguish between characters and bytes */
typedef char ply_byte;

/** Enumeration of usable ply file formats */
enum ply_format {
    PLY_FORMAT_NONE,
    PLY_FORMAT_ASCII,
    PLY_FORMAT_BINARY_LE,
    PLY_FORMAT_BINARY_BE,
    PLY_FORMAT_NUM_ELEMENTS
};

/** Enumeration of all usable ply value types */
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

/** Returns the sizeof the ply type. */
int ply_type_size(enum ply_type type);

/**
 * Converts any type to a float.
 * @param data: A pointer to the source value.
 * @param type: The type of the source value.
 * @return: The source value casted to a float.
 */
float ply_type_to_float(const ply_byte *data, enum ply_type type);

/**
 * Converts any type to an int.
 * @param data: A pointer to the source value.
 * @param type: The type of the source value.
 * @return: The source value casted to a int.
 */
int ply_type_to_int(const ply_byte *data, enum ply_type type);

/**
 * Autotype struct for a ply property.
 * If the property is a list property, list_type is not PLY_TYPE_NONE and defines the type for the list size.
 * If the property is completely parsed (header + data), the data, offset and stride members are filled,
 * so one can use them to iterate over the data (data + offset + stride * i).
 */
typedef struct {
    char name[PLY_MAX_NAME_LENGTH];
    enum ply_type list_type;
    enum ply_type type;
    ply_byte *data;
    int offset;
    int stride;
} ply_property;

/**
 * Autotype struct for a ply element.
 * An element exist of a number of points and a list of ply properties.
 */
typedef struct {
    char name[PLY_MAX_NAME_LENGTH];
    size_t num;
    ply_property properties[PLY_MAX_PROPERTIES];
    size_t properties_size;
} ply_element;

/** Returns the ply property with the given name, or NULL if not found */
ply_property *plyelement_get_property(ply_element *self, const char *property_name);

/**
 * Class struct for a parsed ply file.
 * The file contains all needed information and a list of elements and comments.
 * In the field parsed_data_on_heap_ is the complete parsed data of the ply file.
 * Because this field uses an heap array, one must call ply_File_kill after usage.
 */
typedef struct {
    enum ply_format format;
    char comments[PLY_MAX_COMMENTS][PLY_MAX_COMMENT_LENGTH];
    size_t comments_size;
    ply_element elements[PLY_MAX_ELEMENTS];
    size_t elements_size;
    ply_byte *parsed_data_on_heap_;
} ply_File;

/** Destructor of the ply file (to free field parsed_data_on_heap_) */
void ply_File_kill(ply_File *self);

/** Returns the ply element with the given name, or NULL if not found */
ply_element *ply_File_get_element(ply_File *self, const char *element_name);


#ifdef __cplusplus
}
#endif
#endif //PLYC_TYPES_H
