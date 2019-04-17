#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/** TYPES */

typedef unsigned int uint;

/** STRUCTURES */

/**
 * Contains RGB data for a single pixel
 */
struct rgb_color {
    uint r, g, b;
};

/**
 * Contains data about an RGB image in a 2D matrix, where an element
 * of the matrix is an array, which consists of 3 elements: R, G and B
 * color channel values
 */
struct rgb_image {
    uint width;
    uint height;
    uint scale;
    struct rgb_color **matrix;
};

/**
 * Contains data about an RGB image in a 2D matrix, where an element
 * of the matrix is a grayscale value
 */
struct grayscale_image {
    uint width;
    uint height;
    uint scale;
    uint **matrix; // same as rgb, but no need for third level array, just a number
};


struct image_file {
    FILE *stream;
    uint width;
    uint height;
    uint scale;
};

/** FUNCTIONS */

/* Creating and memory allocation */
struct rgb_image* create_rgb_image(uint width, uint height, uint scale);
struct grayscale_image* create_grayscale_image(uint width, uint height, uint scale);

/* Image processing */
struct grayscale_image* rgb_to_grayscale_image(struct rgb_image *image);

/* File IO */
struct rgb_image* open_rgb_image(char *file_path);
struct grayscale_image* open_grayscale_image(char *file_path);
struct image_file* open_image_file(char* file_path, int expected_version);
int write_rgb_image(char *file_path, struct rgb_image *image);
int write_grayscale_image(char *file_path, struct grayscale_image *image);
int read_header(FILE *stream, char* image_version, uint *width, uint *height, uint *scale);
int get_netpbm_version(char* image_version);


/* Releasing memory */
void free_rgb_image(struct rgb_image *image);
void free_grayscale_image(struct grayscale_image *image);
