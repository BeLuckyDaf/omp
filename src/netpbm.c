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

/**
 * Tries to open the file that contains the image,
 * then reads the header, which should consist of the image type (we expect P3),
 * width, height, both in pixels, and the color scale.
 *
 * Creates the rgb_image where it's going to put all the data.
 * After that tries to read width*height pixels, each containing 3 color channels and
 * saves it into the matrix of the rgb_image that was created earlier.
 *
 * Returns NULL in case of an error or a pointer to struct rgb_image.
 */
struct rgb_image* open_rgb_image(char *file_path) {
    // defining variables we'll need
    char image_type[3];
    uint width, height, scale;

    int items_read;

    // opening the file
    FILE *stream = fopen(file_path, "r");
    if (stream == NULL) return NULL;

    // reading the header
    items_read = fscanf(stream, "%2s %u %u %u", image_type, &width, &height, &scale);
    if (items_read < 4) {
        printf("<netpbm>: could not parse file header, only read %d items out of 4.\n", items_read);
        return NULL;
    }

    // check for the specified format
    if (strcmp(image_type, "P3") == 0) {
        printf("<netpbm>: opened a file of format \"image/x-portable-pixmap\".\n");
    } else {
        printf("<netpbm>: incorrect file format, expected \"image/x-portable-pixmap\".\n");
        return NULL;
    }

    // the resulting image is going to be store here
    struct rgb_image* result = create_rgb_image(width, height);

    // handy struct for the parsed pixels
    struct rgb_color pixel;

    // parsing width * height pixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            items_read = fscanf(stream, "%u %u %u", &pixel.r, &pixel.g, &pixel.b);
            if (items_read < 3) {
                printf("<netpbm>: parsing error, incorrect format.\n");
                return NULL;
            }

            // TODO: remove debugging messages
            result->matrix[y][x] = (struct rgb_color) { .r = pixel.r, .g = pixel.g, .b = pixel.b };
            printf("<debug>: matrix[%d][%d] = { %u %u %u }\n", y, x, result->matrix[y][x].r, result->matrix[y][x].g, result->matrix[y][x].b);
        }
    }

    printf("<netpbm>: successfully parsed the image.\n");

    return result;
}

/**
 *
 */
struct grayscale_image* open_grayscale_image(char *file_path) {
    return NULL;
}

/**
 *
 */
int write_rgb_image(char *file_path, struct rgb_image *image) {
    return 0;
}

/**
 *
 */
int write_grayscale_image(char *file_path, struct grayscale_image *image) {
    return 0;
}