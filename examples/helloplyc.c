#include <stdio.h>
#include <plyc/simple.h>

int main() {

    // ply file to load
    const char *filename = "helloplyc.ply";

    // a simple format to store a point cloud (with a list of float[4])
    ply_Simple cloud;

    ply_err err = ply_simple_load(&cloud, filename);

    // ply_err is a typedef of const char *
    // if an error occurs, the return value is not NULL and points to the error string
    if(err) {
        fprintf(stderr, "Error loading the ply file: %s\n", err);
        exit(EXIT_FAILURE);
    }

    // have fun with the point cloud
    for(int i=0; i < cloud.num; i++) {
        printf("%03d : %5.2f |%5.2f |%5.2f\n",
               i, cloud.points[i][0], cloud.points[i][1], cloud.points[i][2]);
    }
    puts(" i  :    x  |   y  |   z");

    // will print:
    //000 :  1.10 | 2.20 | 3.30
    //001 :  4.40 | 5.50 | 6.60
    // i  :    x  |   y  |   z

    // free the point cloud
    ply_Simple_kill(&cloud);
}
