#include "netpbm.h"


/**
 * Allocates memory for the given dimensions of an RGB image
 * and returns a pointer to the rgb_image structure
 */
struct rgb_image* create_rgb_image(uint width, uint height) {
    // allocate for the structure
    struct rgb_image* image = (struct rgb_image*)malloc(sizeof(struct rgb_image));

    // set the dimensions
    image->width = width;
    image->height = height;

    // allocate a pointer to hold pointers to rows
    // and pointers for the rows themselves
    image->matrix = (struct rgb_color**)calloc(height, sizeof(struct rgb_color*));
    for (int i = 0; i < height; i++) {
        image->matrix[i] = (struct rgb_color*)calloc(width, sizeof(struct rgb_color));
    }

    return image;
}

/**
 * Completely frees the allocated memory for the RGB image structure
 */
void free_rgb_image(struct rgb_image *image) {
    for (int i = 0; i < image->height; i++) {
        free(image->matrix[i]); // free each row
    }

    // then free the matrix and the image struct itself
    free(image->matrix);
    free(image);
}

/**
 * Allocates memory for the given dimensions of a grayscale image
 * and returns a pointer to the grayscale_image structure
 */
struct grayscale_image* create_grayscale_image(uint width, uint height) {
    // allocate for the structure
    struct grayscale_image* image = (struct grayscale_image*)malloc(sizeof(struct grayscale_image));

    // set the dimensions
    image->width = width;
    image->height = height;

    // allocate a pointer to hold pointers to rows
    // and pointers for the rows themselves
    image->matrix = (uint**)calloc(height, sizeof(uint*));
    for (int i = 0; i < height; i++) {
        image->matrix[i] = (uint*)calloc(width, sizeof(uint));
    }

    return image;
}

/**
 * Completely frees the allocated memory for the grayscale image structure
 */
void free_grayscale_image(struct grayscale_image *image) {
    for (int i = 0; i < image->height; i++) {
        free(image->matrix[i]); // free each row
    }

    // then free the matrix and the image struct itself
    free(image->matrix);
    free(image);
}

/**
 * Creates a new grayscale image and then finds average color
 * for each pixel in the colored image, assigns it to the corresponding
 * pixel in the grayscale image, that is, converts it to grayscale
 */
struct grayscale_image* rgb_to_grayscale_image(struct rgb_image* image) {
    struct grayscale_image* result = create_grayscale_image(image->width, image->height);

    for (uint y = 0; y < image->height; y++) {
        for (uint x = 0; x < image->width; x++) {
            uint average = (image->matrix[y][x].r + image->matrix[y][x].g + image->matrix[y][x].b)/3;
            result->matrix[y][x] = average;
        }
    }

    return result;
}