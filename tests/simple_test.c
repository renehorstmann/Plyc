#include "plyc/simple.h"
#include "test_helper.h"


int main() {
    ply_err ret;

    // points
    {
        PlySimple simple;
        ret = ply_simple_load(&simple, "data_1.ply");
        if (ret) return err("simple_load 1 failed", ret);
        if (simple.num != 2)
            return err("simple_load 1 failed, wrong loaded data fields", "");
        if (!simple.holds_heap_memory_)
            return err("simple_load 1 failed, holds heap memory should be true", "");
        if (simple.normals || simple.colors || simple.indices || simple.indices_size != 0 || simple.comments_size != 0)
            return err("simple_load 1 failed, should only have points", "");
        if (vec_check(simple.points[0], (float[]) {1.1, 2.2, 3.3}, 3, 0.001))
            return err("simple_load 1 failed, wrong loaded point 1", "");
        if (vec_check(simple.points[1], (float[]) {4.4, 5.5, 6.6}, 3, 0.001))
            return err("simple_load 1 failed, wrong loaded point 2", "");

        ret = ply_simple_save(simple, "savetest_1.ply", PLY_FORMAT_ASCII);
        if (ret) return err("simple_save 1 failed", ret);

        simple.comments_size = 1;
        strcpy(simple.comments[0], "testi test");
        ret = ply_simple_save(simple, "savetest_2.ply", PLY_FORMAT_BINARY_BE);
        if (ret) return err("simple_save 2 failed", ret);

        ply_simple_kill(&simple);

        ret = ply_simple_load(&simple, "savetest_2.ply");
        if (ret) return err("simple_load 2 failed", ret);
        if (simple.normals || simple.colors || simple.indices || simple.indices_size != 0)
            return err("simple_load 2 failed, should only have points and a comment", "");
        if (simple.comments_size != 1 && strcmp(simple.comments[0], "testi test") != 0)
            return err("simple_load 2 failed, wrong comments", "");
        if (simple.num != 2)
            return err("simple_load 2 failed, wrong loaded data fields", "");
        if (vec_check(simple.points[0], (float[]) {1.1, 2.2, 3.3}, 3, 0.001))
            return err("simple_load 2 failed, wrong loaded point 1", "");
        if (vec_check(simple.points[1], (float[]) {4.4, 5.5, 6.6}, 3, 0.001))
            return err("simple_load 2 failed, wrong loaded point 2", "");

        ply_simple_kill(&simple);
    }

    // mesh
    {
        PlySimple simple;
        ret = ply_simple_load(&simple, "data_2.ply");
        if (ret) return err("simple_load 3 failed", ret);
        if (simple.normals || simple.colors)
            return err("simple_load 2 failed, should only have points and indices", "");
        if (!simple.points || simple.num != 8 || !simple.indices || simple.indices_size != 12)
            return err("simple_load 3 failed, wrong loaded sizes", "");

        float points_data[8][3] = {{0,  10, 10},
                                   {0,  10, 0},
                                   {0,  0,  10},
                                   {0,  0,  0},
                                   {10, 0,  10},
                                   {10, 10, 0},
                                   {10, 10, 10},
                                   {10, 0,  0}};

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
            if (vec_check(simple.points[i], points_data[i], 3, 0.01f))
                return err("simple_load 3 failed, points wrong", "");
        }
        for (int i = 0; i < 12; i++) {
            if (veci_check(simple.indices[i], indices_data[i], 3))
                return err("simple_load 3 failed, indices wrong", "");
        }

        ret = ply_simple_save(simple, "savetest_3.ply", PLY_FORMAT_ASCII);
        if (ret) return err("simple_save 3 failed", ret);

        ply_simple_kill(&simple);

        ret = ply_simple_load(&simple, "savetest_3.ply");
        if (ret) return err("simple_load 4 failed", ret);
        if (simple.normals || simple.colors)
            return err("simple_load 2 failed, should only have points and indices", "");
        if (!simple.points || simple.num != 8 || !simple.indices || simple.indices_size != 12)
            return err("simple_load 4 failed, wrong loaded sizes", "");

        for (int i = 0; i < 8; i++) {
            if (vec_check(simple.points[i], points_data[i], 3, 0.01f))
                return err("simple_load 4 failed, points wrong", "");
        }
        for (int i = 0; i < 12; i++) {
            if (veci_check(simple.indices[i], indices_data[i], 3))
                return err("simple_load 4 failed, indices wrong", "");
        }

        ply_simple_kill(&simple);
    }


    // mesh with normals and colors
    {
        PlySimple simple;
        ret = ply_simple_load(&simple, "data_3.ply");
        if (ret) return err("simple_load 4 failed", ret);
        if (simple.comments_size != 1
            || strcmp(simple.comments[0], "Created by Blender 2.79 (sub 0) - www.blender.org, source file: ''") != 0)
            return err("simple_load 4 failed, comment wrong", "");
        if (simple.num != 24 || simple.indices_size != 12
            || !simple.points || !simple.normals || !simple.colors || !simple.indices)
            return err("simple_load 4 failed, wrong loaded sizes", "");

        float points_data[24][3] = {{1,            1,            -1},
                                    {1,            -1,           -1},
                                    {-1,           -1,           -1},
                                    {-1,           1,            -1},
                                    {1,            0.999998987f, 1},
                                    {-1,           1,            1},
                                    {-1,           -1,           1},
                                    {0.999998987f, -1.00000095f, 1},
                                    {1,            1,            -1},
                                    {1,            0.999998987f, 1},
                                    {0.999998987f, -1.00000095f, 1},
                                    {1,            -1,           -1},
                                    {1,            -1,           -1},
                                    {0.999998987f, -1.00000095f, 1},
                                    {-1,           -1,           1},
                                    {-1,           -1,           -1},
                                    {-1,           -1,           -1},
                                    {-1,           -1,           1},
                                    {-1,           1,            1},
                                    {-1,           1,            -1},
                                    {1,            0.999998987f, 1},
                                    {1,            1,            -1},
                                    {-1,           1,            -1},
                                    {-1,           1,            1}};

        float normals_data[24][3] = {{0,  0,  -1},
                                     {0,  0,  -1},
                                     {0,  0,  -1},
                                     {0,  0,  -1},
                                     {0,  -0, 1},
                                     {0,  -0, 1},
                                     {0,  -0, 1},
                                     {0,  -0, 1},
                                     {1,  -0, 0},
                                     {1,  -0, 0},
                                     {1,  -0, 0},
                                     {1,  -0, 0},
                                     {-0, -1, -0},
                                     {-0, -1, -0},
                                     {-0, -1, -0},
                                     {-0, -1, -0},
                                     {-1, 0,  -0},
                                     {-1, 0,  -0},
                                     {-1, 0,  -0},
                                     {-1, 0,  -0},
                                     {0,  1,  0},
                                     {0,  1,  0},
                                     {0,  1,  0},
                                     {0,  1,  0}};

        float colors_data[24][3] = {{1,             1,             1},
                                    {1,             1,             1},
                                    {1,             1,             1},
                                    {1,             0.0941176564f, 0.831372619f},
                                    {1,             1,             1},
                                    {1,             0.890196145f,  0.152941182f},
                                    {0,             1,             0.10980393f},
                                    {0.0235294141f, 0.337254912f,  1},
                                    {0.600000024f,  1,             0.788235366f},
                                    {1,             0.0431372561f, 0.305882365f},
                                    {0.0235294141f, 0.337254912f,  1},
                                    {1,             1,             1},
                                    {1,             1,             1},
                                    {0.0235294141f, 0.337254912f,  1},
                                    {1,             1,             1},
                                    {1,             1,             1},
                                    {1,             1,             1},
                                    {0,             1,             0.10980393f},
                                    {1,             0.890196145f,  0.152941182f},
                                    {1,             0.0941176564f, 0.831372619f},
                                    {1,             0.0431372561f, 0.305882365f},
                                    {0.600000024f,  1,             0.788235366f},
                                    {1,             0.0941176564f, 0.831372619f},
                                    {1,             0.890196145f,  0.152941182f}};

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
            if (vec_check(simple.points[i], points_data[i], 3, 0.01f))
                return err("simple_load 4 failed, points wrong", "");
            if (vec_check(simple.normals[i], normals_data[i], 3, 0.01f))
                return err("simple_load 4 failed, normals wrong", "");
            if (vec_check(simple.colors[i], colors_data[i], 3, 0.01f))
                return err("simple_load 4 failed, colors wrong", "");
        }
        for (int i = 0; i < 12; i++) {
            if (veci_check(simple.indices[i], indices_data[i], 3))
                return err("simple_load 4 failed, indices wrong", "");
        }

        ret = ply_simple_save(simple, "savetest_4.ply", PLY_FORMAT_ASCII);
        if (ret) return err("simple_save 4 failed", ret);

        ply_simple_kill(&simple);
    }


    // not available
    {
        PlySimple simple;
        ret = ply_simple_load(&simple, "not_available.ply");
        if(!ret)
            return err("simple_load 5 failed, should get an error", NULL);
        if(strcmp(ret, "File not found")!=0)
            return err("simple_load 5 failed, wrong err: ", ret);
    }

}

