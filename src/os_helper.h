#ifndef PLYC_OS_HELPER_H
#define PLYC_OS_HELPER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>

static bool system_is_little_endian() {
    int n = 1;
    // little endian if true
    return *(char *) &n == 1;
}

static void switch_endian(ply_byte *restrict dst, const ply_byte *restrict src, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[n - i - 1];
    }
}

static void open_file_as_string(char **start, char **end, const char *filename) {
    char *text = NULL;
    long length = 0;
    FILE *file = fopen(filename, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);
        text = malloc(length + 1);
        if (text) {
            size_t chars_read = fread(text, 1, length, file);
            if(chars_read != length)
                fprintf(stderr, "open file warning, didnt read enough characters!\n");
            text[length] = '\0';
        }
        fclose(file);
    }
    *start = text;
    *end = text + length;
}

#ifdef __cplusplus
}
#endif
#endif //PLYC_OS_HELPER_H
