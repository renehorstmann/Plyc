#include <stdio.h>
#include <plyc/simple.h>

int main() {

    // ply file to load
    const char *file = "helloplyc.ply";

    // a simple format to store a point cloud (list of float[4])
    ply_SimpleCloud points;

    ply_err err = ply_simple_load(&points, // cloud for x y z
                                  NULL,    // optional cloud for nx ny nz
                                  NULL,    // optional cloud for red green blue
                                  NULL,    // optional mesh indices
                                  NULL,    // optional comments from the ply file
                                  file);

    // ply_err is a typedef of const char *
    // if an error occurs, the return value is not NULL and points to the error string
    if(err) {
        fprintf(stderr, "Error loading the ply file: %s\n", err);
        exit(EXIT_FAILURE);
    }

    // have fun with the point cloud
    for(int i=0; i<points.num; i++) {
        printf("%03d : %5.2f |%5.2f |%5.2f\n",
                i, points.data[i][0], points.data[i][1], points.data[i][2]);
    }
    puts(" i  :    x  |   y  |   z");

    // will print:
    //000 :  1.10 | 2.20 | 3.30
    //001 :  4.40 | 5.50 | 6.60
    // i  :    x  |   y  |   z

    // free the point cloud
    ply_SimpleCloud_kill(&points);
}
