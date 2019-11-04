#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plyc/header.h"

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

    // header test 1
    {
        char *header_text = open_file_as_string("header_1.txt");
        if(!header_text)
            return err("failed to load header 1", "");

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret)
            return err("header test 1 failed", ret);

        if (header.format != PLY_FORMAT_ASCII)
            return err("header test 1 failed, format wrong", "");

        if (header.comments_size != 2)
            return err("header test 1 failed, should have 2 comments", "");
        if (strcmp(header.comments[0], "made by Greg Turk") != 0)
            return err("header test 1 failed, comment 0 wrong", "");
        if (strcmp(header.comments[1], "this file is a cube") != 0)
            return err("header test 1 failed, comment 1 wrong", "");

        if (header.elements_size != 2)
            return err("header test 1 failed, should have 2 elements", "");
        if (strcmp(header.elements[0].name, "vertex") != 0)
            return err("header test 1 failed, element name 0 wrong", "");
        if (header.elements[0].num != 8)
            return err("header test 1 failed, element num 0 wrong", "");
        if (strcmp(header.elements[1].name, "face") != 0)
            return err("header test 1 failed, element name 1 wrong", "");
        if (header.elements[1].num != 6)
            return err("header test 1 failed, element num 1 wrong", "");

        if (header.elements[0].properties_size != 3)
            return err("header test 1 failed, element 0 should have 3 properties", "");
        if (strcmp(header.elements[0].properties[0].name, "x") != 0)
            return err("header test 1 failed, element 0 x name wrong", "");
        if (strcmp(header.elements[0].properties[1].name, "y") != 0)
            return err("header test 1 failed, element 0 y name wrong", "");
        if (strcmp(header.elements[0].properties[2].name, "z") != 0)
            return err("header test 1 failed, element 0 z name wrong", "");
        for (int i = 0; i < 3; i++) {
            if (header.elements[0].properties[i].list_type != PLY_TYPE_NONE
                || header.elements[0].properties[i].type != PLY_TYPE_FLOAT)
                return err("header test 1 failed, element 0 property type wrong", "");
        }

        if (header.elements[1].properties_size != 1)
            return err("header test 1 failed, element 1 should have 1 properties", "");
        if (strcmp(header.elements[1].properties[0].name, "vertex_indices") != 0)
            return err("header test 1 failed, element 1 vertex_indices name wrong", "");
        if (header.elements[1].properties[0].list_type != PLY_TYPE_UCHAR
            || header.elements[1].properties[0].type != PLY_TYPE_INT)
            return err("header test 1 failed, element 1 property type wrong", "");

        char *written_header;
        ret = ply_header_write_to_heap(&written_header, header);
        if (ret)
            return err("header test 1 failed to write", ret);
        size_t len = strlen(header_text);
        if(len != strlen(written_header)-1) // without newline
            return err("header test 1 failed to write, different result size", "");
        int s = strncmp(header_text, written_header, len);
        if (s!= 0)
            return err("header test 1 failed to write, different result", "");
    }

    // header test 2
    {
        char *header_text = open_file_as_string("header_2.txt");
        if(!header_text)
            return err("failed to load header 2", "");

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret)
            return err("header test 2 failed", ret);

        if (header.format != PLY_FORMAT_ASCII)
            return err("header test 2 failed, format wrong", "");

        if (header.comments_size != 1)
            return err("header test 2 failed, should have 1 comments", "");
        if (strcmp(header.comments[0], "Created by Blender 2.79 (sub 0) - www.blender.org, source file: ''") != 0)
            return err("header test 2 failed, comment 0 wrong", "");

        if (header.elements_size != 2)
            return err("header test 2 failed, should have 2 elements", "");
        if (strcmp(header.elements[0].name, "vertex") != 0)
            return err("header test 2 failed, element name 0 wrong", "");
        if (header.elements[0].num != 24)
            return err("header test 2 failed, element num 0 wrong", "");
        if (strcmp(header.elements[1].name, "face") != 0)
            return err("header test 2 failed, element name 1 wrong", "");
        if (header.elements[1].num != 6)
            return err("header test 2 failed, element num 1 wrong", "");

        if (header.elements[0].properties_size != 9)
            return err("header test 2 failed, element 0 should have 9 properties", "");
        if (strcmp(header.elements[0].properties[0].name, "x") != 0)
            return err("header test 2 failed, element 0 x name wrong", "");
        if (strcmp(header.elements[0].properties[1].name, "y") != 0)
            return err("header test 2 failed, element 0 y name wrong", "");
        if (strcmp(header.elements[0].properties[2].name, "z") != 0)
            return err("header test 2 failed, element 0 z name wrong", "");
        if (strcmp(header.elements[0].properties[3].name, "nx") != 0)
            return err("header test 2 failed, element 0 nx name wrong", "");
        if (strcmp(header.elements[0].properties[4].name, "ny") != 0)
            return err("header test 2 failed, element 0 ny name wrong", "");
        if (strcmp(header.elements[0].properties[5].name, "nz") != 0)
            return err("header test 2 failed, element 0 nx name wrong", "");
        if (strcmp(header.elements[0].properties[6].name, "red") != 0)
            return err("header test 2 failed, element 0 red name wrong", "");
        if (strcmp(header.elements[0].properties[7].name, "green") != 0)
            return err("header test 2 failed, element 0 green name wrong", "");
        if (strcmp(header.elements[0].properties[8].name, "blue") != 0)
            return err("header test 2 failed, element 0 blue name wrong", "");
        for (int i = 0; i < 6; i++) {
            if (header.elements[0].properties[i].list_type != PLY_TYPE_NONE
                || header.elements[0].properties[i].type != PLY_TYPE_FLOAT)
                return err("header test 2 failed, element 0 property type wrong", "");
        }
        for (int i = 6; i < 9; i++) {
            if (header.elements[0].properties[i].list_type != PLY_TYPE_NONE
                || header.elements[0].properties[i].type != PLY_TYPE_UCHAR)
                return err("header test 2 failed, element 0 property color type wrong", "");
        }

        if (header.elements[1].properties_size != 1)
            return err("header test 2 failed, element 1 should have 1 properties", "");
        if (strcmp(header.elements[1].properties[0].name, "vertex_indices") != 0)
            return err("header test 2 failed, element 1 vertex_indices name wrong", "");
        if (header.elements[1].properties[0].list_type != PLY_TYPE_UCHAR
            || header.elements[1].properties[0].type != PLY_TYPE_UINT)
            return err("header test 2 failed, element 1 property type wrong", "");

        char *written_header;
        ret = ply_header_write_to_heap(&written_header, header);
        if (ret)
            return err("header test 2 failed to write", ret);
        size_t len = strlen(header_text);
        if(len != strlen(written_header)-1) // without newline
            return err("header test 2 failed to write, different result size", "");
        int s = strncmp(header_text, written_header, len);
        if (s!= 0)
            return err("header test 2 failed to write, different result", "");
    }



    // header test 3
    {
        char *header_text = open_file_as_string("header_3.txt");
        if(!header_text)
            return err("failed to load header 3", "");

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret)
            return err("header test 3 failed", ret);

        if (header.format != PLY_FORMAT_BINARY_LE)
            return err("header test 3 failed, format wrong", "");

        if (header.comments_size != 1)
            return err("header test 3 failed, should have 1 comments", "");
        if (strcmp(header.comments[0], "Created by FreeCAD <http://www.freecadweb.org>") != 0)
            return err("header test 3 failed, comment 0 wrong", "");

        if (header.elements_size != 2)
            return err("header test 3 failed, should have 2 elements", "");
        if (strcmp(header.elements[0].name, "vertex") != 0)
            return err("header test 3 failed, element name 0 wrong", "");
        if (header.elements[0].num != 8)
            return err("header test 3 failed, element num 0 wrong", "");
        if (strcmp(header.elements[1].name, "face") != 0)
            return err("header test 3 failed, element name 1 wrong", "");
        if (header.elements[1].num != 12)
            return err("header test 3 failed, element num 1 wrong", "");

        if (header.elements[0].properties_size != 3)
            return err("header test 3 failed, element 0 should have 3 properties", "");
        if (strcmp(header.elements[0].properties[0].name, "x") != 0)
            return err("header test 3 failed, element 0 x name wrong", "");
        if (strcmp(header.elements[0].properties[1].name, "y") != 0)
            return err("header test 3 failed, element 0 y name wrong", "");
        if (strcmp(header.elements[0].properties[2].name, "z") != 0)
            return err("header test 3 failed, element 0 z name wrong", "");
        for (int i = 0; i < 3; i++) {
            if (header.elements[0].properties[i].list_type != PLY_TYPE_NONE
                || header.elements[0].properties[i].type != PLY_TYPE_FLOAT)
                return err("header test 3 failed, element 0 property type wrong", "");
        }

        if (header.elements[1].properties_size != 1)
            return err("header test 3 failed, element 1 should have 1 properties", "");
        if (strcmp(header.elements[1].properties[0].name, "vertex_index") != 0)
            return err("header test 3 failed, element 1 vertex_index name wrong", "");
        if (header.elements[1].properties[0].list_type != PLY_TYPE_UCHAR
            || header.elements[1].properties[0].type != PLY_TYPE_INT)
            return err("header test 3 failed, element 1 property type wrong", "");

        header_text = open_file_as_string("header_3_res.txt"); // float32 -> float
        if(!header_text)
            return err("failed to load header 3 res", "");

        char *written_header;
        ret = ply_header_write_to_heap(&written_header, header);
        if (ret)
            return err("header test 3 failed to write", ret);
        size_t len = strlen(header_text);
        if(len != strlen(written_header)-1) // without newline
            return err("header test 3 failed to write, different result size", "");
        int s = strncmp(header_text, written_header, len);
        if (s!= 0)
            return err("header test 3 failed to write, different result", "");
    }
    
    
    // header fail test 1
    {
        char *header_text = "plx\n....";

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret != PLY_NOT_A_PLY_FILE)
            return err("header fail test 1 failed", ret);
    }

    // header fail test 2
    {
        char *header_text = "ply\n...\nend_head";

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret != PLY_HEADER_ENDING_ERROR)
            return err("header fail test 2 failed", ret);
    }

    // header fail test 3
    {
        char *header_text = "ply\nformat UTF8\n...\nend_header";

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret != PLY_HEADER_FORMAT_ERROR)
            return err("header fail test 3 failed", ret);
    }

    // header fail test 4
    {
        char *header_text = open_file_as_string("header_fail_4.txt");
        if(!header_text)
            return err("failed to load header fail 4", "");

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret != PLY_ELEMENT_ERROR)
            return err("header fail test 4 failed", ret);
    }

    // header fail test 5
    {
        char *header_text = open_file_as_string("header_fail_5.txt");
        if(!header_text)
            return err("failed to load header fail 5", "");

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret != PLY_PROPERTY_ERROR)
            return err("header fail test 5 failed", ret);
    }

    return 0;
}

