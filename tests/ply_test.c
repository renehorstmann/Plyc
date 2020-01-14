#include <stdio.h>
#include <math.h>
#include <string.h>

#include <stdbool.h>
#include <stdint.h>

#include "plyc/ply.h"

#define struct_packed struct __attribute__((__packed__))

static int err(const char *msg, const char *ret) {
    fprintf(stderr, "%s : <%s>\n", msg, ret);
    return 1;
}


static bool flt_eq(float a, float b) {
    if (isnan(a) == isnan(b))
        return true;
    return fabsf(a - b) < 0.001f;
}


int main() {
    ply_err ret;


    // points
    {
        ply_File file;
        ret = ply_load_file(&file, "data_1.ply", 4);
        if (ret) return err("load_file 1 failed", ret);
        if (file.format != PLY_FORMAT_ASCII)
            return err("load_file 1 failed, wrong format", "");
        if (file.elements_size != 1)
            return err("load_file 1 failed, wrong elements_size", "");
        plyelement *vertex = ply_File_get_element(&file, "vertex");
        if (!vertex)
            return err("load_file 1 failed, vertex element not found", "");
        if (vertex->num != 2 || vertex->properties_size != 3)
            return err("load_file 1 failed, vertex data is wrong (num, props)", "");

        plyproperty *x = plyelement_get_property(vertex, "x");
        plyproperty *y = plyelement_get_property(vertex, "y");
        plyproperty *z = plyelement_get_property(vertex, "z");
        if (!x || !y || !z)
            return err("load_file 1 failed, one of xyz properties not found", "");
        if (x != &vertex->properties[0] || y != &vertex->properties[1] || z != &vertex->properties[2])
            return err("load_file 1 failed, xyz order is wrong", "");

        for (int xyz = 0; xyz < 3; xyz++) {
            if (vertex->properties[xyz].list_type != PLY_TYPE_NONE || vertex->properties[xyz].type != PLY_TYPE_FLOAT)
                return err("load_file 1 failed, property type is wrong", "");
        }

        float points[2][3] = {{1.1f, 2.2f, 3.3f},
                              {4.4f, 5.5f, 6.6f}};

        for (int i = 0; i < vertex->num; i++) {
            for (int xyz = 0; xyz < 3; xyz++) {
                plyproperty *prop = &vertex->properties[xyz];
                float *value = (float *) (prop->data + prop->offset + prop->stride * i);
                if (!flt_eq(*value, points[i][xyz]))
                    return err("load_file 1 failed, value wrong", "");
            }
        }

        char *written_begin;
        size_t written_size;
        ret = ply_write_memory_into_heap(&written_begin, &written_size, file);
        if(ret) return err("write_memory 1 failed", ret);

        ply_File_kill(&file);

        ret = ply_parse_memory(&file, written_begin, written_size, 0);
        if(ret) return err("parse_memory 1 failed", ret);

        free(written_begin);

        if (file.format != PLY_FORMAT_ASCII)
            return err("load_file 1 failed, wrong format", "");
        if (file.elements_size != 1)
            return err("load_file 1 failed, wrong elements_size", "");
        vertex = ply_File_get_element(&file, "vertex");
        if (!vertex)
            return err("load_file 1 failed, vertex element not found", "");
        if (vertex->num != 2 || vertex->properties_size != 3)
            return err("load_file 1 failed, vertex data is wrong (num, props)", "");

        x = plyelement_get_property(vertex, "x");
        y = plyelement_get_property(vertex, "y");
        z = plyelement_get_property(vertex, "z");
        if (!x || !y || !z)
            return err("load_file 1 failed, one of xyz properties not found", "");
        if (x != &vertex->properties[0] || y != &vertex->properties[1] || z != &vertex->properties[2])
            return err("load_file 1 failed, xyz order is wrong", "");

        for (int xyz = 0; xyz < 3; xyz++) {
            if (vertex->properties[xyz].list_type != PLY_TYPE_NONE || vertex->properties[xyz].type != PLY_TYPE_FLOAT)
                return err("load_file 1 failed, property type is wrong", "");
        }

        for (int i = 0; i < vertex->num; i++) {
            for (int xyz = 0; xyz < 3; xyz++) {
                plyproperty *prop = &vertex->properties[xyz];
                float *value = (float *) (prop->data + prop->offset + prop->stride * i);
                if (!flt_eq(*value, points[i][xyz]))
                    return err("load_file 1 failed, value wrong", "");
            }
        }

        ply_File_kill(&file);
    }

    // mesh
    {
        ply_File file;
        ret = ply_load_file(&file, "data_2.ply", 8);
        if (ret) return err("load_file 2 failed", ret);
        if (file.format != PLY_FORMAT_BINARY_LE)
            return err("load_file 2 failed, wrong format", "");
        if (file.elements_size != 2)
            return err("load_file 2 failed, wrong elements_size", "");
        plyelement *vertex = ply_File_get_element(&file, "vertex");
        if (!vertex)
            return err("load_file 2 failed, vertex element not found", "");
        if (vertex->num != 8 || vertex->properties_size != 3)
            return err("load_file 2 failed, vertex data is wrong (num, props)", "");

        plyproperty *x = plyelement_get_property(vertex, "x");
        plyproperty *y = plyelement_get_property(vertex, "y");
        plyproperty *z = plyelement_get_property(vertex, "z");
        if (!x || !y || !z)
            return err("load_file 2 failed, one of xyz properties not found", "");
        if (x != &vertex->properties[0] || y != &vertex->properties[1] || z != &vertex->properties[2])
            return err("load_file 2 failed, xyz order is wrong", "");

        for (int xyz = 0; xyz < 3; xyz++) {
            if (vertex->properties[xyz].list_type != PLY_TYPE_NONE || vertex->properties[xyz].type != PLY_TYPE_FLOAT)
                return err("load_file 2 failed, property type is wrong", "");
        }

        float points[8][3] = {
                {0,  10, 10},
                {0,  10, 0},
                {0,  0,  10},
                {0,  0,  0},
                {10, 0,  10},
                {10, 10, 0},
                {10, 10, 10},
                {10, 0,  0}
        };

        for (int i = 0; i < vertex->num; i++) {
            for (int xyz = 0; xyz < 3; xyz++) {
                plyproperty *prop = &vertex->properties[xyz];
                float *value = (float *) (prop->data + prop->offset + prop->stride * i);
                if (!flt_eq(*value, points[i][xyz]))
                    return err("load_file 2 failed, value wrong", "");
            }
        }

        plyelement *face = ply_File_get_element(&file, "face");
        if (!face)
            return err("load_file 2 failed, face element not found", "");
        if (face->num != 12 || face->properties_size != 1)
            return err("load_file 2 failed, face data is wrong (num, props)", "");

        plyproperty *vertex_index = plyelement_get_property(face, "vertex_index");
        if (!vertex_index)
            return err("load_file 2 failed, vertex_index property not found", "");

        if (vertex_index->list_type != PLY_TYPE_UCHAR || vertex_index->type != PLY_TYPE_INT)
            return err("load_file 2 failed, property list type is wrong", "");


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

        for (int i = 0; i < face->num; i++) {
            uint8_t *size = (uint8_t *) (vertex_index->data + vertex_index->offset + vertex_index->stride * i);
            if(*size != exp_list_res[i].num)
                return err("load_file 2 failed, wrond list num", "");
            size++;
            int *data = (int *) size;
            for(int item=0; item<8; item++) {
                if (*data != exp_list_res[i].element[item])
                    return err("load_file 2 failed, list value wrong", "");
                data++;
            }
        }

        ply_File_kill(&file);
    }

}

