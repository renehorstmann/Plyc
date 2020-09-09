#include "plyc/header.h"
#include "plyc/data.h"
#include "test_helper.h"

#define struct_packed struct __attribute__((__packed__))


int main() {
    ply_err ret;

    // data test 1
    {
        char *header_text;
        char *data_end;
        open_file_as_string(&header_text, &data_end, "data_1.ply");
        if (!header_text)
            return err("failed to load data 1", "");

        PlyFile file;
        ret = ply_header_parse(&file, header_text);
        if (ret)
            return err("data test 1 failed, parsing header failed", ret);

        ply_byte *ply_data;
        ret = ply_header_get_end(&ply_data, header_text);
        if (ret)
            return err("data test 1 failed, wtf (header end not found)", "");

        struct_packed point {
            float x, y, z;
        };

        ret = ply_data_parse(&file, ply_data, data_end - ply_data, 0);
        if (ret)
            return err("data test 1 failed, parsing data failed", ret);

        struct point exp_res[2] = {
                {1.1f, 2.2f, 3.3f},
                {4.4f, 5.5f, 6.6f}
        };

        struct point *data = (struct point *) file.parsed_data_on_heap_;

        for (int i = 0; i < 2; i++) {
            if (!flt_eq(data[i].x, exp_res[i].x)
                || !flt_eq(data[i].y, exp_res[i].y)
                || !flt_eq(data[i].z, exp_res[i].z))
                return err("data test 1 failed, wrong expected result", "");
        }

        free(header_text);
        ply_file_kill(&file);
    }


    // data test 2
    {
        char *header_text;
        char *data_end;
        open_file_as_string(&header_text, &data_end, "data_2.ply");
        if (!header_text)
            return err("failed to load data 2", "");

        PlyFile file;
        ret = ply_header_parse(&file, header_text);
        if (ret)
            return err("data test 2 failed, parsing header failed", ret);


        ply_byte *ply_data;
        ret = ply_header_get_end(&ply_data, header_text);
        if (ret)
            return err("data test 2 failed, wtf (header end not found)", "");

        struct_packed point {
            float x, y, z;
        };

        ret = ply_data_parse(&file, ply_data, data_end - ply_data, 8);
        if (ret)
            return err("data test 2 failed, parsing data failed", ret);

        struct point exp_res[8] = {
                {0,  10, 10},
                {0,  10, 0},
                {0,  0,  10},
                {0,  0,  0},
                {10, 0,  10},
                {10, 10, 0},
                {10, 10, 10},
                {10, 0,  0}
        };

        struct point *data = (struct point *) file.elements[0].properties[0].data;

        for (int i = 0; i < 8; i++) {
            if (!flt_eq(data[i].x, exp_res[i].x)
                || !flt_eq(data[i].y, exp_res[i].y)
                || !flt_eq(data[i].z, exp_res[i].z))
                return err("data test 2 failed, wrong expected result", "");
        }

        struct_packed list {
            uint8_t num;
            int32_t element[8];
        };

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

        struct list *list_data = (struct list *) file.elements[1].properties[0].data;

        for (int i = 0; i < 12; i++) {
            if (list_data[i].num != exp_list_res[i].num)
                return err("data test 2 failed, wrong expected list_data num", "");
            for (int li = 0; li < 8; li++) {
                if (list_data[i].element[li] != exp_list_res[i].element[li])
                    return err("data test 2 failed, wrong expected list_data", "");
            }
        }

        free(header_text);
        ply_file_kill(&file);
    }



    // data fail test 3
    {
        char *header_text;
        char *data_end;
        open_file_as_string(&header_text, &data_end, "data_fail_3.ply");
        if (!header_text)
            return err("failed to load data fail 3", "");

        PlyFile file;
        ret = ply_header_parse(&file, header_text);
        if (ret)
            return err("data fail test 3 failed, parsing header failed", ret);

        char *ply_data;
        ret = ply_header_get_end(&ply_data, header_text);
        if (ret)
            return err("data fail test 3 failed, wtf (header end not found)", "");

        ret = ply_data_parse(&file, ply_data, data_end - ply_data, 0);
        if (strcmp(ret, "Data parse error") != 0) // in ascii, the parsing fails
            return err("data fail test 3 failed, parsing data failed", ret);

        free(header_text);
    }


    // data fail test 2
    {
        char *header_text;
        char *data_end;
        open_file_as_string(&header_text, &data_end, "data_fail_4.ply");
        if (!header_text)
            return err("failed to load data fail 4", "");

        PlyFile file;
        ret = ply_header_parse(&file, header_text);
        if (ret)
            return err("data fail test 4 failed, parsing header failed", ret);


        char *ply_data;
        ret = ply_header_get_end(&ply_data, header_text);
        if (ret)
            return err("data fail test 4 failed, wtf (header end not found)", "");

        struct_packed point {
            float x, y, z;
        };

        ret = ply_data_parse(&file, ply_data, data_end - ply_data, 8);
        if (strcmp(ret, "Data buffer is too small") != 0)
            return err("data fail test 4 failed, parsing list_data failed", ret);

        free(header_text);
    }

    return 0;
}

