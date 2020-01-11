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
    // - num is the amount of the points
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
