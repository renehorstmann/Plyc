#include <stdio.h>
#include <math.h>
#include <string.h>

#include <stdbool.h>

#include "plyc/ply.h"


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
        ret = ply_parse_file(&file, "data_1.ply", 4);
        if (ret) return err("parse_file 1 failed", ret);
        if (file.format != PLY_FORMAT_ASCII)
            return err("parse_file 1 failed, wrong format", "");
        if (file.elements_size != 1)
            return err("parse_file 1 failed, wrong elements_size", "");
        plyelement *vertex = ply_File_get_element(&file, "vertex");
        if (!vertex)
            return err("parse_file 1 failed, vertex element not found", "");
        if (vertex->num != 2 || vertex->properties_size != 3)
            return err("parse_file 1 failed, vertex data is wrong (num, props)", "");

        plyproperty *x = plyelement_get_property(vertex, "x");
        plyproperty *y = plyelement_get_property(vertex, "y");
        plyproperty *z = plyelement_get_property(vertex, "z");
        if (!x || !y || !z)
            return err("parse_file 1 failed, one of xyz properties not found", "");
        if (x != &vertex->properties[0] || y != &vertex->properties[1] || z != &vertex->properties[2])
            return err("parse_file 1 failed, xyz order is wrong", "");

        for (int xyz = 0; xyz < 3; xyz++) {
            if (vertex->properties[xyz].list_type != PLY_TYPE_NONE || vertex->properties[xyz].type != PLY_TYPE_FLOAT)
                return err("parse_file 1 failed, property type is wrong", "");
        }

        float points[2][4] = {{1.1f, 2.2f, 3.3f},
                              {4.4f, 5.5f, 6.6f}};

        for (int i = 0; i < vertex->num; i++) {
            for (int xyz = 0; xyz < 2; xyz++) {
                plyproperty *prop = &vertex->properties[i];
                float *value = (float *) (prop->data + prop->offset + prop->stride * i);
                if (!flt_eq(*value, points[i][xyz]))
                    return err("parse_file 1 failed, value wrong", "");
            }
        }
    }

}

