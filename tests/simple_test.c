#include <stdio.h>
#include <math.h>


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

    // simpel test 1
    {
        ply_SimpleCloud cloud;
        ret = ply_simple_load_cloud(&cloud, NULL, "data_1.txt");
        if(ret) return err("simple_load_cloud 1 failed", ret);
        if(cloud.size != 2 || cloud.normals || cloud.colors || cloud.curvatures || !cloud.points)
            return err("simple_load_cloud 1 failed, wrong loaded data fields", "");
        if(vec_check(cloud.points[0], (float[]) {1.1, 2.2, 3.3, 1}, 4, 0.001))
            return err("simple_load_cloud 1 failed, wrong loaded point 1", "");
        if(vec_check(cloud.points[1], (float[]) {4.4, 5.5, 6.6, 1}, 4, 0.001))
            return err("simple_load_cloud 1 failed, wrong loaded point 2", "");
    }


}

