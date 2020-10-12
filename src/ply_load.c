#include <string.h>
#include <stdio.h>

#include "plyc/utilc/alloc.h"
#include "plyc/ply.h"

#include "plyc/header.h"
#include "plyc/data.h"

#include "os_helper.h"

ply_err ply_load_file(PlyFile *out_file, const char *filename, int max_list_size) {
    char *memory_begin, *memory_end;
    open_file_as_string(&memory_begin, &memory_end, filename);
    ply_err res = ply_parse_memory(out_file, memory_begin, memory_end-memory_begin, max_list_size);
    free(memory_begin);
    return res;
}

ply_err ply_parse_memory(PlyFile *out_file, const char *ply_file_text, int ply_file_text_size, int max_list_size) {
    ply_err err;

    *out_file = (PlyFile) {0};

    const char *memory_end = ply_file_text + ply_file_text_size;

    // parse header
    err = ply_header_parse(out_file, ply_file_text);
    if (err) return err;

    // get data begin
    ply_byte *data_begin;
    err = ply_header_get_end(&data_begin, ply_file_text);
    if (err) return err;


    err = ply_data_parse(out_file, data_begin, memory_end - data_begin, max_list_size);
    if(err) return err;

    return PLY_Success;
}
