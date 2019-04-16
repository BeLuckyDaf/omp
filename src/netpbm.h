#include <stdlib.h>


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
    struct rgb_color **matrix;
};

/**
 * Contains data about an RGB image in a 2D matrix, where an element
 * of the matrix is a grayscale value
 */
struct grayscale_image {
    uint width;
    uint height;    
    uint **matrix; // same as rgb, but no need for third level array, just a number
};


/** FUNCTIONS */

/* Creating and memory allocation */
struct rgb_image* create_rgb_image(uint width, uint height);
struct grayscale_image* create_grayscale_image(uint width, uint height);

/* Modifying data */
struct grayscale_image* rgb_to_grayscale_image(struct rgb_image* image);

/* Releasing memory */
void free_rgb_image(struct rgb_image *image);
void free_grayscale_image(struct grayscale_image *image);
