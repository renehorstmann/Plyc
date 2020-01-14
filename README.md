# Plyc
A simple C library to load and save a .ply file with any desired data, written in C.


## TODO

+ bindings
  - Python3 (numpy)


## Examples

### Loading a point cloud
Example [helloplyc.c](examples/helloplyc.c) shows how to load the points of a ply file:
```c
#include <stdio.h>
#include <plyc/simple.h>

int main() {

    // ply file to load
    const char *file = "helloplyc.ply";

    // a simple format to store a point cloud (with list of float[3])
    ply_Simple cloud;

    ply_err err = ply_simple_load(&cloud, file);

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
```

### Saving a point cloud
Example [easysave.c](examples/easysave.c) shows how to save points and a comment to a ply file:

```c
#include <stdio.h>
#include <string.h>
#include <plyc/simple.h>

int main() {

    // ply file to save
    const char *file = "easysave.ply";

    // 5 data points to save (x y z)
    float data_points[5][3] = {{0, 0, 0},
                               {1, 0, 0},
                               {0, 1, 0},
                               {0, 0, 1},
                               {1, 1, 1}};

    // The SimpleCloud struct consists of two fields:
    // - data is a pointer to an xyzw point matrix
    // - num is the number of the points
    ply_Simple cloud;
    cloud.points = data_points; // pointer to an xyz point matrix
    cloud.normals = NULL;       // optional normals as nxnynz matrix
    cloud.colors = NULL;        // optional colors as rgb matrix
    cloud.num = 5;
    cloud.indices = NULL;       // optional mesh indices as abc int matrix
    cloud.indices_size = 0;     // the number of triangles for the optional mesh

    // the Simple struct has also optional comments, in the form of char[128][8]
    // so its possible to save up to 8 lines of comments, each 127 chars wide.
    cloud.comments_size = 1;    // the number of optional comments
    strcpy(cloud.comments[0], "Created with easysave.c");   // comments vector
    cloud.holds_heap_memory_ = false;   // for internal use

    // Another - easier - way to build up the struct, is to set all other values to 0 at beginning:
    ply_Simple cloud_easy = {0};
    cloud_easy.points = data_points;
    cloud_easy.num = 5;


    // save the Simple cloud to a file.
    // format must be one of PLY_FORMAT_ASCII, PLY_FORMAT_BINARY_LE, PLY_FORMAT_BINARY_BE
    ply_err err = ply_simple_save(cloud, file, PLY_FORMAT_ASCII);

    // ply_err is a typedef of const char *
    // if an error occurs, the return value is not NULL and points to the error string
    if (err) {
        fprintf(stderr, "Error saving the ply file: %s\n", err);
        exit(EXIT_FAILURE);
    }

    // killing a custom cloud has no effect, if holds_heap_memory_ is true, it will free each pointer
    ply_Simple_kill(&cloud);
}
```

### Loading a mesh
Example [mesh.c](examples/mesh.c) shows how to load a mesh ply file:
A mesh consists of a point cloud and mesh indices, which describe a triangle by 3 indices for the point cloud.
```c
#include <stdio.h>
#include <plyc/simple.h>

int main() {

    // ply file to load
    const char *file = "mesh.ply";

    // a simple format to store a point cloud, but also a mesh (with an additional indices list of int[3])
    ply_Simple cloud;

    ply_err err = ply_simple_load(&cloud, file);

    // ply_err is a typedef of const char *
    // if an error occurs, the return value is not NULL and points to the error string
    if(err) {
        fprintf(stderr, "Error loading the ply file: %s\n", err);
        exit(EXIT_FAILURE);
    }

    // check if the indices were available in the ply file (points must be available, or ply_err is set)
    if(cloud.indices_size == 0) {
        fprintf(stderr, "The ply file %s did not contain mesh indices\n", file);
        exit(EXIT_FAILURE);
    }

    // have fun with the mesh
    for(int i=0; i<cloud.indices_size; i++) {
        printf("Triangle %d:\n", i);
        for(int abc=0; abc<3; abc++) {
            int index = cloud.indices[i][abc];
            printf("%03d :%c: %5.2f |%5.2f |%5.2f\n",
                   index, 'a' + abc,
                   cloud.points[index][0], cloud.points[index][1], cloud.points[index][2]);
        }
        puts("");
    }
    puts(" i  :v:    x  |   y  |   z");

    // will print:
    //Triangle 0:
    //000 :a:  0.00 |10.00 |10.00
    //001 :b:  0.00 |10.00 | 0.00
    //002 :c:  0.00 | 0.00 |10.00
    // ...
    //Triangle 11:
    //006 :a: 10.00 |10.00 |10.00
    //002 :b:  0.00 | 0.00 |10.00
    //004 :c: 10.00 | 0.00 |10.00
    //
    // i  :v:    x  |   y  |   z

    // free the point cloud and its mesh indices
    ply_Simple_kill(&cloud);
}
```
