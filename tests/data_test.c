#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plyc/data.h"

static int err(char *msg, const char *ret) {
    fprintf(stderr, "%s : <%s>\n", msg, ret);
    return 1;
}

static char *open_file_as_string(const char *filename) {
    char *text = NULL;
    FILE *file = fopen(filename, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        text = malloc(length+1);
        if (text) {
            fread(text, 1, length, file);
            text[length] = '\0';
        }
        fclose(file);
    }
    return text;
}

int main() {
    ply_err ret;

    // data test 1
    {
        char *header_text = open_file_as_string("data_1.txt");
        if(!header_text)
            return err("failed to load data 1", "");

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret)
            return err("data test 1 failed, parsing header failed", ret);

        struct {
            float x, y, z;
        } data[2];

        // todo:
//        ret = ply_data_parse_element(data, header.elements[0], header.format, )



    }

    return 0;
}

