#ifndef PLYC_PLY_H
#define PLYC_PLY_H
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

/**
 * Loads and parses a .ply file from a file.
 * A call to this function is enough to load the header and parse the data.
 * After loading and using the PlyFile, it should be destructed with a call to ply_file_kill.
 * @param filename: The file destination.
 * @param max_list_size: Maximal size of each property list. (meshes uses a size of 3 or 4 for triangle or quads).
 * @return: A ply_err if an error occurs, such as file not found.
 */
ply_err ply_load_file(PlyFile *out_file, const char *filename, int max_list_size);

/**
 * Parses a .ply file from a string/memory.
 * A call to this function is enough to load the header and parse the data.
 * After loading and using the PlyFile, it should be destructed with a call to ply_file_kill.
 * @param ply_file_text: The string/memory that holds the whole .ply file content.
 * @param ply_file_text_size: The size of the string/memory.
 * @param max_list_size: Maximal size of each property list. (meshes uses a size of 3 or 4 for triangle or quads).
 * @return: A ply_err if an error occurs.
 */
ply_err ply_parse_memory(PlyFile *out_file, const char *ply_file_text, int ply_file_text_size, int max_list_size);

/**
 * Writes and saves a .ply file to a file.
 * @param file: The filled out PlyFile.
 * @param filename: The file destination to save the .ply file.
 * @return: A ply_err if an error occurs, such as file permission error.
 */
ply_err ply_save_file(PlyFile file, const char *filename);

/**
 * Writes and a .ply file to an allocated memory on heap (out_data_on_heap + out_data_size).
 * @param file: The filled out PlyFile.
 * @return: A ply_err if an error occurs.
 */
ply_err ply_write_memory_into_heap(char **out_data_on_heap, int *out_size, PlyFile file);



#ifdef __cplusplus
}
#endif
#endif //PLYC_PLY_H
