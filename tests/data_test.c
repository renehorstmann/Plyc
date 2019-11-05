#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "plyc/data.h"

#define struct_packed struct __attribute__((__packed__))

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
            fread(text, 1, length, file);
            text[length] = '\0';
        }
        fclose(file);
    }
    *start = text;
    *end = text+length;
}

static bool flt_eq(float a, float b) {
    if (isnan(a) == isnan(b))
        return true;
    return fabsf(a - b) < 0.001f;
}

int main() {
    ply_err ret;

    // data test 1
    {
        char *header_text;
        char *data_end;
        open_file_as_string(&header_text, &data_end, "data_1.txt");
        if (!header_text)
            return err("failed to load data 1", "");

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret)
            return err("data test 1 failed, parsing header failed", ret);

        char *ply_data;
        ret = ply_header_get_end(&ply_data, header_text);
        if (ret)
            return err("data test 1 failed, wtf (header end not found)", "");

        struct_packed point {
            float x, y, z;
        };

        struct point data[2];
        if(sizeof(data) != ply_element_size(header.elements[0], header.format, 0))
            return err("data test 1 failed, needed element_size is wrong", "");

        ret = ply_data_parse_element((ply_byte *) data, &ply_data, data_end, header.elements[0], header.format, 0);
        if (ret)
            return err("data test 1 failed, parsing data failed", ret);

        struct point exp_res[2] = {
                {1.1f, 2.2f, 3.3f},
                {4.4f, 5.5f, 6.6f}
        };

        for(int i=0; i<2; i++) {
            if(!flt_eq(data[i].x, exp_res[i].x)
            || !flt_eq(data[i].y, exp_res[i].y)
            || !flt_eq(data[i].z, exp_res[i].z))
                return err("data test 1 failed, wrong expected result", "");
        }

    }


    // data test 2
    {
        char *header_text;
        char *data_end;
        open_file_as_string(&header_text, &data_end, "data_2.txt");
        if (!header_text)
            return err("failed to load data 2", "");

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret)
            return err("data test 2 failed, parsing header failed", ret);


        char *ply_data;
        ret = ply_header_get_end(&ply_data, header_text);
        if (ret)
            return err("data test 2 failed, wtf (header end not found)", "");

        struct_packed point {
            float x, y, z;
        };

        struct point data[8];
        if(sizeof(data) != ply_element_size(header.elements[0], header.format, 0))
            return err("data test 2 failed, needed element_size is wrong", "");

        ret = ply_data_parse_element((ply_byte *) data, &ply_data, data_end, header.elements[0], header.format, 0);
        if (ret)
            return err("data test 2 failed, parsing data failed", ret);

        struct point exp_res[8] = {
                {0, 10, 10},
                {0, 10, 0},
                {0, 0, 10},
                {0, 0, 0},
                {10, 0, 10},
                {10, 10, 0},
                {10, 10, 10},
                {10, 0, 0}
        };

        for(int i=0; i<8; i++) {
            if(!flt_eq(data[i].x, exp_res[i].x)
               || !flt_eq(data[i].y, exp_res[i].y)
               || !flt_eq(data[i].z, exp_res[i].z))
                return err("data test 2 failed, wrong expected result", "");
        }

        struct_packed list {
            uint8_t num;
            int32_t element[8];
        };

        struct list list_data[12];
        if(sizeof(list_data) != ply_element_size(header.elements[1], header.format, 8))
            return err("data test 2 failed, needed element_size for list is wrong", "");

        ret = ply_data_parse_element((ply_byte *) list_data, &ply_data, data_end, header.elements[1], header.format, 8);
        if (ret)
            return err("data test 2 failed, parsing list_data failed", ret);

        struct list exp_list_res[12] = {
                {3, {0, 1, 2}},
                {3, {1, 3, 2}},
                {3, {4, 5, 6}},
                {3, {4, 7, 5}},
                {3, {3, 7, 4}},
                {3, {2, 3, 4}},
                {3, {6, 5, 1}},
                {3, {6, 1, 0}},
                {3, {3, 1, 5}},
                {3, {7, 3, 5}},
                {3, {6, 0, 2}},
                {3, {6, 2, 4}}
        };

        for(int i=0; i<12; i++) {
            if(list_data[i].num != exp_list_res[i].num)
                return err("data test 2 failed, wrong expected list_data num", "");
            for(int li=0; li<8; li++) {
                if(list_data[i].element[li] != exp_list_res[i].element[li])
                    return err("data test 2 failed, wrong expected list_data", "");
            }
        }
    }



    // data fail test 3
    {
        char *header_text;
        char *data_end;
        open_file_as_string(&header_text, &data_end, "data_fail_3.txt");
        if (!header_text)
            return err("failed to load data fail 3", "");

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret)
            return err("data fail test 3 failed, parsing header failed", ret);

        char *ply_data;
        ret = ply_header_get_end(&ply_data, header_text);
        if (ret)
            return err("data fail test 3 failed, wtf (header end not found)", "");

        struct_packed point {
            float x, y, z;
        };

        struct point data[2];
        if(sizeof(data) != ply_element_size(header.elements[0], header.format, 0))
            return err("data fail test 3 failed, needed element_size is wrong", "");

        ret = ply_data_parse_element((ply_byte *) data, &ply_data, data_end, header.elements[0], header.format, 0);
        if (ret!=PLY_DATA_PARSE_ERROR) // in ascii, the parsing fails
            return err("data fail test 3 failed, parsing data failed", ret);

    }


    // data fail test 2
    {
        char *header_text;
        char *data_end;
        open_file_as_string(&header_text, &data_end, "data_fail_4.txt");
        if (!header_text)
            return err("failed to load data fail 4", "");

        plyheader header;
        ret = ply_header_parse(&header, header_text);
        if (ret)
            return err("data fail test 4 failed, parsing header failed", ret);


        char *ply_data;
        ret = ply_header_get_end(&ply_data, header_text);
        if (ret)
            return err("data fail test 4 failed, wtf (header end not found)", "");

        struct_packed point {
            float x, y, z;
        };

        struct point data[8];
        if(sizeof(data) != ply_element_size(header.elements[0], header.format, 0))
            return err("data fail test 4 failed, needed element_size is wrong", "");

        ret = ply_data_parse_element((ply_byte *) data, &ply_data, data_end, header.elements[0], header.format, 0);
        if (ret)
            return err("data fail test 4 failed, parsing data failed", ret);

        struct point exp_res[8] = {
                {0, 10, 10},
                {0, 10, 0},
                {0, 0, 10},
                {0, 0, 0},
                {10, 0, 10},
                {10, 10, 0},
                {10, 10, 10},
                {10, 0, 0}
        };

        for(int i=0; i<8; i++) {
            if(!flt_eq(data[i].x, exp_res[i].x)
               || !flt_eq(data[i].y, exp_res[i].y)
               || !flt_eq(data[i].z, exp_res[i].z))
                return err("data fail test 4 failed, wrong expected result", "");
        }

        struct_packed list {
            uint8_t num;
            int32_t element[8];
        };

        struct list list_data[12];
        if(sizeof(list_data) != ply_element_size(header.elements[1], header.format, 8))
            return err("data fail test 4 failed, needed element_size for list is wrong", "");

        ret = ply_data_parse_element((ply_byte *) list_data, &ply_data, data_end, header.elements[1], header.format, 8);
        if (ret != PLY_NOT_ENOUGH_DATA)
            return err("data fail test 4 failed, parsing list_data failed", ret);

    }

    return 0;
}

