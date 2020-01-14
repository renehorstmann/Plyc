#ifndef PLYC_DATA_H
#define PLYC_DATA_H
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"


ply_err ply_data_parse(ply_File *in_out_file,
                       const ply_byte *restrict ply_data, size_t ply_data_size,
                       size_t max_list_size);

ply_err ply_data_write_to_heap(char **out_data_on_heap, size_t *out_data_size, ply_File file);


#ifdef __cplusplus
}
#endif
#endif //PLYC_DATA_H
