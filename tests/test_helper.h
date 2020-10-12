#ifndef PLYC_TEST_HELPER_H
#define PLYC_TEST_HELPER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

static int err(char *msg, const char *ret) {
    fprintf(stderr, "%s : <%s>\n", msg, ret);
    return 1;
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
            int chars_read = fread(text, 1, length, file);
            if (chars_read != length)
                fprintf(stderr, "open file warning, didnt read enough characters!\n");
            text[length] = '\0';
        }
        fclose(file);
    }
    *start = text;
    *end = text + length;
}

static bool flt_eq(float a, float b) {
    if (isnan(a) && isnan(b))
        return true;
    return fabsf(a - b) < 0.001f;
}

static bool vec_check(const float *a, const float *b, int n, float eps) {
    for (int i = 0; i < n; i++) {
        if (isnan(b[i]) != isnan(a[i]) || fabsf(a[i] - b[i]) > eps)
            return true;
    }
    return false;
}

static bool veci_check(const int *a, const int *b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i])
            return true;
    }
    return false;
}

#ifdef __cplusplus
}
#endif
#endif //PLYC_TEST_HELPER_H
