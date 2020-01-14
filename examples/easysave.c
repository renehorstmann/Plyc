#include <stdio.h>
#include <string.h>
#include <plyc/simple.h>

int main() {

    // ply file to save
    const char *filename = "easysave.ply";

    // 3 data points to save (x y z w=1)
    // the forth data field (w) is not used and only for padding
    // (in homogeneous coordinates, w is 1 for points and 0 for vectors)
    float data_points[3][4] = {{1, 0, 0, 1},
                               {0, 1, 0, 1},
                               {0, 0, 1, 1}};

    // The Simple struct consists of multiple fields:
    ply_Simple cloud;
    cloud.points = data_points; // pointer to an xyzw point matrix
    cloud.normals = NULL;       // optional normals as nxnynzw matrix
    cloud.colors = NULL;        // optional colors as rgbw matrix
    cloud.num = 5;              // number of points in the cloud
    cloud.indices = NULL;       // optional mesh indices as abc int matrix
    cloud.indices_size = 0;     // the number of triangles for the optional mesh

    // the Simple struct has also optional comments, in the form of char[128][8]
    // so its possible to save up to 8 lines of comments, each 127 chars wide.
    cloud.comments_size = 1;    // the number of optional comments
    strcpy(cloud.comments[0], "Created with easysave.c");   // comments vector
    cloud.holds_heap_memory_ = false;   // for internal use

    // Another - easier - way to build up the struct, is to set all other values to 0 at beginning:
    ply_Simple cloud_easy_creation = {0};
    cloud_easy_creation.points = data_points;
    cloud_easy_creation.num = 5;


    // save the Simple cloud to a file.
    // format must be one of PLY_FORMAT_ASCII, PLY_FORMAT_BINARY_LE, PLY_FORMAT_BINARY_BE
    ply_err err = ply_simple_save(cloud, filename, PLY_FORMAT_ASCII);

    // ply_err is a typedef of const char *
    // if an error occurs, the return value is not NULL and points to the error string
    if (err) {
        fprintf(stderr, "Error saving the ply file: %s\n", err);
        exit(EXIT_FAILURE);
    }

    // killing a custom cloud has no effect, only if holds_heap_memory_ is true, it will free each pointer
    ply_Simple_kill(&cloud);
}
