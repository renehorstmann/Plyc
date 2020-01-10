#include <stdio.h>
#include <math.h>
#include <string.h>


#include "plyc/simple.h"


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

static int err(const char *msg, const char *ret) {
    fprintf(stderr, "%s : <%s>\n", msg, ret);
    return 1;
}


int main() {
    ply_err ret;

    // fail test
    {
        ret = ply_simple_load(NULL, NULL, NULL, NULL, NULL, "data_1.ply");
        if (strcmp(ret, "SimpleCloud out_points are necessary and must not be NULL") != 0)
            return err("simple_load fail test failed (points missing test)", ret);

        ply_SimpleCloud points;
        ret = ply_simple_load(&points, NULL, NULL, NULL, NULL, "data_not_existing.fail");
        if (strcmp(ret, "File not found") != 0)
            return err("simple_load fail test failed (file not found test)", ret);
    }

    // points
    {
        ply_SimpleCloud points;
        ret = ply_simple_load(&points, NULL, NULL, NULL, NULL, "data_1.ply");
        if (ret) return err("simple_load 1 failed", ret);
        if (points.num != 2)
            return err("simple_load 1 failed, wrong loaded data fields", "");
        if (vec_check(points.data[0], (float[]) {1.1, 2.2, 3.3, 1}, 4, 0.001))
            return err("simple_load 1 failed, wrong loaded point 1", "");
        if (vec_check(points.data[1], (float[]) {4.4, 5.5, 6.6, 1}, 4, 0.001))
            return err("simple_load 1 failed, wrong loaded point 2", "");

        ret = ply_simple_save(points, NULL, NULL, NULL, NULL, "savetest_1.ply", PLY_FORMAT_ASCII);
        if (ret) return err("simple_save 1 failed", ret);

        ply_comments comments, comments_load_check;
        comments.comments_size = 1;
        strcpy(comments.comments[0], "testi test");
        ret = ply_simple_save(points, NULL, NULL, NULL, &comments, "savetest_2.ply", PLY_FORMAT_BINARY_BE);
        if (ret) return err("simple_save 2 failed", ret);

        ply_SimpleCloud_kill(&points);

        ret = ply_simple_load(&points, NULL, NULL, NULL, &comments_load_check, "savetest_2.ply");
        if (ret) return err("simple_load 2 failed", ret);
        if (comments_load_check.comments_size != 1 && strcmp(comments_load_check.comments[0], "testi test") != 0)
            return err("simple_load 2 failed, wrong comments", "");
        if (points.num != 2)
            return err("simple_load 2 failed, wrong loaded data fields", "");
        if (vec_check(points.data[0], (float[]) {1.1, 2.2, 3.3, 1}, 4, 0.001))
            return err("simple_load 2 failed, wrong loaded point 1", "");
        if (vec_check(points.data[1], (float[]) {4.4, 5.5, 6.6, 1}, 4, 0.001))
            return err("simple_load 2 failed, wrong loaded point 2", "");

        ply_SimpleCloud_kill(&points);
    }

    // mesh
    {
        ply_SimpleCloud points;
        ply_SimpleMeshIndices indices;
        ret = ply_simple_load(&points, NULL, NULL, &indices, NULL, "data_2.ply");
        if (ret) return err("simple_load 3 failed", ret);
        if (!points.data || points.num != 8 || !indices.indices || indices.num != 12)
            return err("simple_load 3 failed, wrong loaded sizes", "");

        float points_data[8][4] = {{0,  10, 10, 1},
                                   {0,  10, 0,  1},
                                   {0,  0,  10, 1},
                                   {0,  0,  0,  1},
                                   {10, 0,  10, 1},
                                   {10, 10, 0,  1},
                                   {10, 10, 10, 1},
                                   {10, 0,  0,  1}};

        int indices_data[12][3] = {{0, 1, 2},
                                   {1, 3, 2},
                                   {4, 5, 6},
                                   {4, 7, 5},
                                   {3, 7, 4},
                                   {2, 3, 4},
                                   {6, 5, 1},
                                   {6, 1, 0},
                                   {3, 1, 5},
                                   {7, 3, 5},
                                   {6, 0, 2},
                                   {6, 2, 4}};

        for (int i = 0; i < 8; i++) {
            if (vec_check(points.data[i], points_data[i], 4, 0.01f))
                return err("simple_load 3 failed, points wrong", "");
        }
        for (int i = 0; i < 12; i++) {
            if (veci_check(indices.indices[i], indices_data[i], 3))
                return err("simple_load 3 failed, indices wrong", "");
        }

        ret = ply_simple_save(points, NULL, NULL, &indices, NULL, "savetest_3.ply", PLY_FORMAT_ASCII);
        if (ret) return err("simple_save 3 failed", ret);

        ply_SimpleCloud_kill(&points);
        ply_SimpleMeshIndices_kill(&indices);

        ret = ply_simple_load(&points, NULL, NULL, &indices, NULL, "savetest_3.ply");
        if (ret) return err("simple_load 4 failed", ret);
        if (!points.data || points.num != 8 || !indices.indices || indices.num != 12)
            return err("simple_load 4 failed, wrong loaded sizes", "");

        for (int i = 0; i < 8; i++) {
            if (vec_check(points.data[i], points_data[i], 4, 0.01f))
                return err("simple_load 4 failed, points wrong", "");
        }
        for (int i = 0; i < 12; i++) {
            if (veci_check(indices.indices[i], indices_data[i], 3))
                return err("simple_load 4 failed, indices wrong", "");
        }

        ply_SimpleCloud_kill(&points);
        ply_SimpleMeshIndices_kill(&indices);
    }


    // mesh with normals and colors
    {
        ply_SimpleCloud points, normals, colors;
        ply_SimpleMeshIndices indices;
        ply_comments comments;
        ret = ply_simple_load(&points, &normals, &colors, &indices, &comments, "data_3.ply");
        if (ret) return err("simple_load 4 failed", ret);
        if (comments.comments_size != 1
            || strcmp(comments.comments[0], "Created by Blender 2.79 (sub 0) - www.blender.org, source file: ''") != 0)
            return err("simple_load 4 failed, comment wrong", "");
        if (!points.data || points.num != 24
            || !normals.data || normals.num != 24
            || !colors.data || colors.num != 24
            || !indices.indices || indices.num != 12)
            return err("simple_load 4 failed, wrong loaded sizes", "");

        float points_data[24][4] = {{1,            1,            -1, 1},
                                    {1,            -1,           -1, 1},
                                    {-1,           -1,           -1, 1},
                                    {-1,           1,            -1, 1},
                                    {1,            0.999998987f, 1,  1},
                                    {-1,           1,            1,  1},
                                    {-1,           -1,           1,  1},
                                    {0.999998987f, -1.00000095f, 1,  1},
                                    {1,            1,            -1, 1},
                                    {1,            0.999998987f, 1,  1},
                                    {0.999998987f, -1.00000095f, 1,  1},
                                    {1,            -1,           -1, 1},
                                    {1,            -1,           -1, 1},
                                    {0.999998987f, -1.00000095f, 1,  1},
                                    {-1,           -1,           1,  1},
                                    {-1,           -1,           -1, 1},
                                    {-1,           -1,           -1, 1},
                                    {-1,           -1,           1,  1},
                                    {-1,           1,            1,  1},
                                    {-1,           1,            -1, 1},
                                    {1,            0.999998987f, 1,  1},
                                    {1,            1,            -1, 1},
                                    {-1,           1,            -1, 1},
                                    {-1,           1,            1,  1}};

        float normals_data[24][4] = {{0,  0,  -1, 0},
                                     {0,  0,  -1, 0},
                                     {0,  0,  -1, 0},
                                     {0,  0,  -1, 0},
                                     {0,  -0, 1,  0},
                                     {0,  -0, 1,  0},
                                     {0,  -0, 1,  0},
                                     {0,  -0, 1,  0},
                                     {1,  -0, 0,  0},
                                     {1,  -0, 0,  0},
                                     {1,  -0, 0,  0},
                                     {1,  -0, 0,  0},
                                     {-0, -1, -0, 0},
                                     {-0, -1, -0, 0},
                                     {-0, -1, -0, 0},
                                     {-0, -1, -0, 0},
                                     {-1, 0,  -0, 0},
                                     {-1, 0,  -0, 0},
                                     {-1, 0,  -0, 0},
                                     {-1, 0,  -0, 0},
                                     {0,  1,  0,  0},
                                     {0,  1,  0,  0},
                                     {0,  1,  0,  0},
                                     {0,  1,  0,  0}};

        float colors_data[24][4] = {{1,             1,             1,            1},
                                    {1,             1,             1,            1},
                                    {1,             1,             1,            1},
                                    {1,             0.0941176564f, 0.831372619f, 1},
                                    {1,             1,             1,            1},
                                    {1,             0.890196145f,  0.152941182f, 1},
                                    {0,             1,             0.10980393f,  1},
                                    {0.0235294141f, 0.337254912f,  1,            1},
                                    {0.600000024f,  1,             0.788235366f, 1},
                                    {1,             0.0431372561f, 0.305882365f, 1},
                                    {0.0235294141f, 0.337254912f,  1,            1},
                                    {1,             1,             1,            1},
                                    {1,             1,             1,            1},
                                    {0.0235294141f, 0.337254912f,  1,            1},
                                    {1,             1,             1,            1},
                                    {1,             1,             1,            1},
                                    {1,             1,             1,            1},
                                    {0,             1,             0.10980393f,  1},
                                    {1,             0.890196145f,  0.152941182f, 1},
                                    {1,             0.0941176564f, 0.831372619f, 1},
                                    {1,             0.0431372561f, 0.305882365f, 1},
                                    {0.600000024f,  1,             0.788235366f, 1},
                                    {1,             0.0941176564f, 0.831372619f, 1},
                                    {1,             0.890196145f,  0.152941182f, 1}};

        int indices_data[12][3] = {{0,  1,  2},
                                   {0,  2,  3},
                                   {4,  5,  6},
                                   {4,  6,  7},
                                   {8,  9,  10},
                                   {8,  10, 11},
                                   {12, 13, 14},
                                   {12, 14, 15},
                                   {16, 17, 18},
                                   {16, 18, 19},
                                   {20, 21, 22},
                                   {20, 22, 23}};


        for (int i = 0; i < 24; i++) {
            if (vec_check(points.data[i], points_data[i], 4, 0.01f))
                return err("simple_load 4 failed, points wrong", "");
            if (vec_check(normals.data[i], normals_data[i], 4, 0.01f))
                return err("simple_load 4 failed, normals wrong", "");
            if (vec_check(colors.data[i], colors_data[i], 4, 0.01f))
                return err("simple_load 4 failed, colors wrong", "");
        }
        for (int i = 0; i < 12; i++) {
            if (veci_check(indices.indices[i], indices_data[i], 3))
                return err("simple_load 4 failed, indices wrong", "");
        }

        ret = ply_simple_save(points, &normals, &colors, &indices, NULL, "savetest_4.ply", PLY_FORMAT_ASCII);
        if (ret) return err("simple_save 4 failed", ret);
    }

}

