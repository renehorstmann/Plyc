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
