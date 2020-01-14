#ifndef PLYC_PLY_H
#define PLYC_PLY_H
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"


ply_err ply_load_file(ply_File *out_file, const char *filename, int max_list_size);

ply_err ply_parse_memory(ply_File *out_file, const char *ply_file_text, size_t ply_file_text_size, int max_list_size);

ply_err ply_save_file(ply_File file, const char *filename);

ply_err ply_write_memory_into_heap(char **out_data_on_heap, size_t *out_size, ply_File file);



#ifdef __cplusplus
}
#endif
#endif //PLYC_PLY_H
