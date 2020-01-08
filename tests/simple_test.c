#include <stdio.h>
#include <math.h>
#include <string.h>


#include "plyc/simple.h"


static bool vec_check(float *a, float *b, int n, float eps) {
    for(int i=0; i<n; i++) {
        if ( isnan(b[i]) != isnan(a[i]) || fabsf(a[i] - b[i]) > eps)
            return true;
    }
    return false;
}

static int err(char *msg, const char *ret) {
    fprintf(stderr, "%s : <%s>\n", msg, ret);
    return 1;
}


int main() {
    ply_err ret;

    // simple test 1
    {
        ply_SimpleCloud points;
        ret = ply_simple_load(&points, NULL, NULL, NULL, NULL, "data_1.txt");
        if(ret) return err("simple_load 1 failed", ret);
        if(points.num != 2)
            return err("simple_load 1 failed, wrong loaded data fields", "");
        if(vec_check(points.data[0], (float[]) {1.1, 2.2, 3.3, 1}, 4, 0.001))
            return err("simple_load 1 failed, wrong loaded point 1", "");
        if(vec_check(points.data[1], (float[]) {4.4, 5.5, 6.6, 1}, 4, 0.001))
            return err("simple_load 1 failed, wrong loaded point 2", "");

        ret = ply_simple_save(points, NULL, NULL, NULL, NULL, "savetest_1.txt", PLY_FORMAT_ASCII);
        if(ret) return err("simple_save 1 failed", ret);

        ply_comments comments;
        comments.comments_size = 1;
        strcpy(comments.comments[0], "testi test");
        ret = ply_simple_save(points, NULL, NULL, NULL, &comments, "savetest_2.txt", PLY_FORMAT_BINARY_BE);
        if(ret) return err("simple_save 2 failed", ret);

        ret = ply_simple_load(&points, NULL, NULL, NULL, &comments, "savetest_2.txt");
        if(ret) return err("simple_load 2 failed", ret);
        if(comments.comments_size != 1 && strcmp(comments.comments[0], "testi test") != 0)
            return err("simple_load 2 failed, wrong comments", "");
        if(points.num != 2)
            return err("simple_load 2 failed, wrong loaded data fields", "");
        if(vec_check(points.data[0], (float[]) {1.1, 2.2, 3.3, 1}, 4, 0.001))
            return err("simple_load 2 failed, wrong loaded point 1", "");
        if(vec_check(points.data[1], (float[]) {4.4, 5.5, 6.6, 1}, 4, 0.001))
            return err("simple_load 2 failed, wrong loaded point 2", "");
    }


}

