#include "netpbm.h"


/**
 * Allocates memory for the given dimensions of an RGB image
 * and returns a pointer to the rgb_image structure
 */
struct rgb_image* create_rgb_image(uint width, uint height, uint scale) {
    // allocate for the structure
    struct rgb_image* image = (struct rgb_image*)malloc(sizeof(struct rgb_image));

    // set the dimensions
    image->width = width;
    image->height = height;
    image->scale = scale;

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
struct grayscale_image* create_grayscale_image(uint width, uint height, uint scale) {
    // allocate for the structure
    struct grayscale_image* image = (struct grayscale_image*)malloc(sizeof(struct grayscale_image));

    // set the dimensions
    image->width = width;
    image->height = height;
    image->scale = scale;

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
    struct grayscale_image* result = create_grayscale_image(image->width, image->height, image->scale);

    for (uint y = 0; y < image->height; y++) {
        for (uint x = 0; x < image->width; x++) {
            uint average = (image->matrix[y][x].r + image->matrix[y][x].g + image->matrix[y][x].b)/3;
            result->matrix[y][x] = average;
        }
    }

    return result;
}

/**
 * Reads the first 2 characters and 3 other unsigned integers, considering it the header
 * of an image file. Those are believed to be image version, width, height and scale.
 */
int read_header(FILE *stream, char* image_version, uint *width, uint *height, uint *scale) {
    int items_read = fscanf(stream, "%2s %u %u %u", image_version, width, height, scale);

    if (items_read < 4) {
        printf("<netpbm>: could not parse file header, only read %d items out of 4.\n", items_read);
        printf("<netpbm>: please, note that comments (lines starting with #) are not supported.\n");
        return -1;
    }

    return 0;
}

/**
 * Checks the type of the image in a very straight-forward way
 */
int get_netpbm_version(char* image_version) {
    if (strlen(image_version) < 2 || image_version[0] != 'P') return -1;

    if (image_version[1] > '0' && image_version[1] < '7') return image_version[1] - '0';
    else return 0;
}

/**
 * Creates a file stream and reads the header of the image.
 * Used not to repeat the same code in the image opening functions.
 */
struct image_file* open_image_file(char* file_path, int expected_version) {
    // defining variables we'll need
    char image_version[3];
    uint width, height, scale;

    // opening the file
    FILE *stream = fopen(file_path, "r");
    if (stream == NULL) return NULL;

    // reading the header
    if (read_header(stream, image_version, &width, &height, &scale) != 0) {
        return NULL; // failed reading header
    }

    // check for the specified format
    int version = get_netpbm_version(image_version);

    if (version == expected_version) {
        // fill the struct with the acquired data and send it back in
        printf("<netpbm>: opened a file of format \"P%d\".\n", expected_version);
        struct image_file *result = (struct image_file*)malloc(sizeof(struct image_file));
        *result = (struct image_file) { .width = width, .height = height, .scale = scale, .stream = stream };
        return result;
    } else if (version == 0) {
        printf("<netpbm>: format \"%s\" does not exist.\n");
    } else {
        printf("<netpbm>: incorrect file format, expected \"P%d\".\n", expected_version);
    }

    // if there was an error return NULL
    fclose(stream);
    return NULL;
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
    // opening the file and reading the header
    struct image_file *image = open_image_file(file_path, 3);
    if (image == NULL) return NULL;

    // the resulting image is going to be stored here
    struct rgb_image* result = create_rgb_image(image->width, image->height, image->scale);

    // handy struct for the parsed pixels
    struct rgb_color pixel;

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fscanf(image->stream, "%u %u %u", &pixel.r, &pixel.g, &pixel.b);
            if (items_read < 3) {
                printf("<netpbm>: parsing error, incorrect format.\n");
                printf("<netpbm>: please, note that comments (lines starting with #) are not supported.\n");
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
 * Tries to open the file that contains the image,
 * then reads the header, which should consist of the image type (we expect P3),
 * width, height, both in pixels, and the color scale.
 *
 * Creates the grayscale_image where it's going to put all the data.
 * After that tries to read width*height pixels, each containing just one
 * unsigned integer and then saves it into the matrix of the grayscale_image
 * that was created earlier.
 *
 * Returns NULL in case of an error or a pointer to struct grayscale_image.
 */
struct grayscale_image* open_grayscale_image(char *file_path) {
    // opening the file and getting the header of it
    struct image_file *image = open_image_file(file_path, 2);
    if (image == NULL) return NULL;

    // the resulting image is going to be stored here
    struct grayscale_image* result = create_grayscale_image(image->width, image->height, image->scale);

    // an unsigned integer for the grayscale value
    uint pixel;

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fscanf(image->stream, "%u", &pixel);
            if (items_read < 1) {
                printf("<netpbm>: parsing error, incorrect format.\n");
                printf("<netpbm>: please, note that comments (lines starting with #) are not supported.\n");
                return NULL;
            }
        }
    }

    printf("<netpbm>: successfully parsed the image.\n");

    return result;
}

/**
 * Uses existing rgb_image structure to save it to disk in the P3 format
 */
int write_rgb_image(char *file_path, struct rgb_image *image) {
    // open or create the file
    FILE *stream = fopen(file_path, "w");
    if (stream == NULL) {
        printf("<netpbm>: could not open file for writing.\n");
        return -1;
    }

    // write the header to file
    fprintf(stream, "P3\n%u %u\n%u\n", image->width, image->height, image->scale);

    // write all pixels down line by line
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            fprintf(stream, "%u %u %u ", image->matrix[y][x].r, image->matrix[y][x].g, image->matrix[y][x].b);
        }
        fprintf(stream, "\n");
    }

    return 0;
}

/**
 * Uses existing grayscale_image structure to save it to disk in the P2 grayscale format
 */
int write_grayscale_image(char *file_path, struct grayscale_image *image) {
    // open or create the file
    FILE *stream = fopen(file_path, "w");
    if (stream == NULL) {
        printf("<netpbm>: could not open file for writing.\n");
        return -1;
    }

    // write the header to file
    fprintf(stream, "P2\n%u %u\n%u\n", image->width, image->height, image->scale);

    // write all pixels down line by line
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            fprintf(stream, "%u ", image->matrix[y][x]);
        }
        fprintf(stream, "\n");
    }

    return 0;
}