#include <string.h>
#include <stdio.h>

#include "plyc/utilc/alloc.h"
#include "plyc/ply.h"
#include "plyc/header.h"
#include "plyc/data.h"


ply_err ply_save_file(PlyFile file, const char *filename) {
    char *heap_begin;
    int size;

    ply_err err = ply_write_memory_into_heap(&heap_begin, &size, file);
    if(err)
        return err;


    FILE *f = fopen(filename, "wb");
    if (f) {
        fwrite(heap_begin, size, 1, f);
    }

    free(heap_begin);

    if (!f || fclose(f) != 0)
        return "File write error";

    return err;
}


ply_err ply_write_memory_into_heap(char **out_data_on_heap, int *out_size, PlyFile file) {
    ply_err err = PLY_Success;

    char *header_text;
    int header_text_size;
    char *data_buffer;
    int data_buffer_size;

    err = ply_header_write_to_heap(&header_text, file);
    if(err) return err;

    header_text_size= strlen(header_text);

    err = ply_data_write_to_heap(&data_buffer, &data_buffer_size, file);
    if(err) goto CLEAN_UP;

    *out_data_on_heap = TryNew(char, header_text_size + data_buffer_size);
    if(!*out_data_on_heap)
        PlySetErrGoto(err, "Allocation Error", CLEAN_UP);

    memcpy(*out_data_on_heap, header_text, header_text_size);
    memcpy(*out_data_on_heap + header_text_size, data_buffer, data_buffer_size);
    *out_size = header_text_size + data_buffer_size;

    CLEAN_UP:

    free(header_text);
    free(data_buffer);
    return err;
}
