# Plyc
A simple library to load and save a .ply file with any desired data, written in C.


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
```

### Saving a point cloud
Example [easysave.c](examples/helloplyc.c) shows how to save points and a comment to a ply file:

```c
#include <stdio.h>
#include <string.h>
#include <plyc/simple.h>

int main() {

    // ply file to save
    const char *file = "easysave.ply";

    // 3 data points to save (x y z w=1)
    float data_points[3][4] = {{1, 0, 0, 1},
                               {0, 1, 0, 1},
                               {0, 0, 1, 1}};

    // The SimpleCloud struct consists of two fields:
    // - data is a pointer to an xyzw point matrix
    // - num is the number of the points
    ply_SimpleCloud points;
    points.data = data_points;
    points.num = 3;

    // Plyc can also optionally load and save comments into the ply file
    // the comments struct can hold 8 comments with each 127 chars long comment lines
    ply_comments comments;
    comments.comments_size = 1;
    strcpy(comments.comments[0], "Created with easysave.c");

    ply_err err = ply_simple_save(points,  // cloud for x y z
                                  NULL,    // optional cloud for nx ny nz
                                  NULL,    // optional cloud for red green blue
                                  NULL,    // optional mesh indices
                                  &comments, // optional comments for the ply file
                                  file,
                                  PLY_FORMAT_ASCII);

    // ply_err is a typedef of const char *
    // if an error occurs, the return value is not NULL and points to the error string
    if(err) {
        fprintf(stderr, "Error saving the ply file: %s\n", err);
        exit(EXIT_FAILURE);
    }

    // SimpleCloud should only be freed (killed) if its loaded by Plyc.
    // In this case it only holds a pointer to a stack array.
}
```