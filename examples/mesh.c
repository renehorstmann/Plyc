#include <stdio.h>
#include <plyc/simple.h>

int main() {

    // ply file to load
    const char *filename = "mesh.ply";

    // a simple format to store a point cloud, but also a mesh (with an additional indices list of int[3])
    ply_Simple cloud;

    ply_err err = ply_simple_load(&cloud, filename);

    // ply_err is a typedef of const char *
    // if an error occurs, the return value is not NULL and points to the error string
    if(err) {
        fprintf(stderr, "Error loading the ply file: %s\n", err);
        exit(EXIT_FAILURE);
    }

    // check if the indices were available in the ply file (points must be available, or ply_err is set)
    if(cloud.indices_size == 0) {
        fprintf(stderr, "The ply file %s did not contain mesh indices\n", filename);
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
