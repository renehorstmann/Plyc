# Plyc
A simple C library to load and save a .ply file with any desired data, written in C.

## Getting Started
Copy the library to your system, compile and install it.
The library is written in C (tested on GCC 7.4.0).
If cloned:
```shell script
mkdir build
cd build
cmake ..
make
make install  # as root or with sudo make install

```

## Overview
The following public header are available in plyc/
+ [plyc/simple.h](include/plyc/simple.h) Simple way to load and save a .ply file as a point cloud or a mesh.
+ [plyc/ply.h](include/plyc/ply.h) Advanced parse and write a .ply file with any desired data.
+ [plyc/header.h](include/plyc/header.h) Parse the header text of a .ply file.
+ [plyc/data.h](include/plyc/data.h) Parse the data of a .ply file.
+ [plyc/types.h](include/plyc/types.h) All structs, types and constants needed for the functions.

## Examples

### Loading a point cloud
Example [helloplyc.c](examples/helloplyc.c) shows how to load the points of a ply file:
```c
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
```

### Saving a point cloud
Example [easysave.c](examples/easysave.c) shows how to save points and a comment to a ply file:

```c
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
```

### Loading a mesh
Example [mesh.c](examples/mesh.c) shows how to load a mesh ply file:
A mesh consists of a point cloud and mesh indices, which describe a triangle by 3 indices for the point cloud.
```c
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
```

### Loading a ply file (advanced)
Example [parsedadvanced.c](examples/parsedadvanced.c) shows how to load any ply file:
After loading, the user can get access to all informations of the ply file
and use some pointer arithmetic to iterate over its data.
```c
#include <stdio.h>
#include <string.h>
#include <plyc/ply.h>

int main() {

    // ply file to load
    const char *filename = "helloplyc.ply";

    // holds all information and memory, once loaded
    ply_File plyfile;

    // load a .ply file into memory
    // the third parameter (max_list_size) defines the maximum size for each property list
    ply_err err = ply_load_file(&plyfile, filename, 4);

    // ply_err is a typedef of const char *
    // if an error occurs, the return value is not NULL and points to the error string
    if (err) {
        fprintf(stderr, "Error loading the ply file: %s\n", err);
        exit(EXIT_FAILURE);
    }

    // format of the ply file
    printf("Format: ");
    if (plyfile.format == PLY_FORMAT_ASCII)
        puts("ascii");
    if (plyfile.format == PLY_FORMAT_BINARY_LE)
        puts("binary little endian");
    if (plyfile.format == PLY_FORMAT_BINARY_BE)
        puts("binary big endian");


    // list all elements by name
    printf("Elements:");
    for (int e = 0; e < plyfile.elements_size; e++) {
        printf(" %s", plyfile.elements[e].name);
    }
    puts("");

    // get access to an element
    plyelement *vertex;

    // option 1:
    vertex = &plyfile.elements[0];
    printf("First element has the name: %s and %zu points\n", vertex->name, vertex->num);

    // option 2:
    vertex = ply_File_get_element(&plyfile, "vertex");
    if (!vertex) {
        fprintf(stderr, "Error finding element vertex");
        exit(EXIT_FAILURE);
    }

    // list all properties of the vertex element by name
    for (int p = 0; p < vertex->properties_size; p++) {
        printf("Property name: %s: is_float: %d\n", vertex->properties[p].name,
               vertex->properties[p].type == PLY_TYPE_FLOAT);
    }


    // get access to some properties
    plyproperty *x, *y, *z;

    // option 1:
    x = &vertex->properties[0];
    printf("First property has the name: %s\n", x->name);

    // option 2:
    y = plyelement_get_property(vertex, "y");
    z = plyelement_get_property(vertex, "z");
    if (!y || !z) {
        fprintf(stderr, "Error finding properties y and z");
        exit(EXIT_FAILURE);
    }

    // check if xyz all have floats as type
    if(x->type != PLY_TYPE_FLOAT || y->type != PLY_TYPE_FLOAT || z->type != PLY_TYPE_FLOAT) {
        fprintf(stderr, "The property type of xyz is not float");
        exit(EXIT_FAILURE);
    }

    // check if xyz are not lists (marked with list_type == first element in the list describes its length)
    if(x->list_type != PLY_TYPE_NONE || y->list_type != PLY_TYPE_NONE || z->list_type != PLY_TYPE_NONE) {
        fprintf(stderr, "The property xyz is a list");
        exit(EXIT_FAILURE);
    }

    // have fun with the point cloud:
    for (int i = 0; i < vertex->num; i++) {
        float *x_val = (float *) (x->data + x->offset + x->stride * i);
        float *y_val = (float *) (y->data + y->offset + y->stride * i);
        float *z_val = (float *) (z->data + z->offset + z->stride * i);
        printf("%03d : %5.2f |%5.2f |%5.2f\n",
               i, *x_val, *y_val, *z_val);
    }
    puts(" i  :    x  |   y  |   z");


    // will print:
    //Format: ascii
    //Elements: vertex
    //First element has the name: vertex and 2 points
    //Property name: x: is_float: 1
    //Property name: y: is_float: 1
    //Property name: z: is_float: 1
    //First property has the name: x
    //000 :  1.10 | 2.20 | 3.30
    //001 :  4.40 | 5.50 | 6.60
    //i  :    x  |   y  |   z

    // free the loaded ply file
    ply_File_kill(&plyfile);
}
```
## Author

* **René Horstmann**

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
