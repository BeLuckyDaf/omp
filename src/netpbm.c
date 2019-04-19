#include "netpbm.h"

/**
 * Allocates memory for the given dimensions of an RGB image.
 *
 * Returns a pointer to the rgb_image structure.
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
 * Allocates memory for the given dimensions of a grayscale image.
 *
 * Returns a pointer to the grayscale_image structure.
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
 *
 * Returns a pointer to the resulting grayscale_image structure.
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
 * Skips comments if they are present on the way to a number
 *
 * Returns EOF if end-of-file is reached, otherwise returns 0.
 */
int skip_comment(FILE *stream) {
    int c;
    while ((((char)(c = fgetc(stream))) > '9' || c < '0') && c != EOF) {
        if (c == '#') while(((char)(c = fgetc(stream))) != '\n' && c != EOF);
        if (c == EOF) return EOF;
    }
    fseek(stream, -1, SEEK_CUR);
    return 0;
}

/**
 * Reads the first 2 characters and 3 other unsigned integers, considering it the header
 * of an image file. Those are believed to be image version, width, height and scale.
 *
 * Returns -1 if error occurred, otherwise returns 0.
 */
int read_header(FILE *stream, char* image_version, uint *width, uint *height, uint *scale) {
    int items_read = fscanf(stream, "%2s", image_version);

    skip_comment(stream);
    if (fscanf(stream, "%u", width) < 1) {
        printf("<netpbm>: could not read width.\n");
        return -1;
    }

    skip_comment(stream);
    if (fscanf(stream, "%u", height) < 1) {
        printf("<netpbm>: could not read height.\n");
        return -1;
    }

    skip_comment(stream);
    if (fscanf(stream, "%u", scale) < 1) {
        printf("<netpbm>: could not read scale.\n");
        return -1;
    }

    return 0;
}

/**
 * Checks the type of the image in a very straight-forward way
 *
 * Returns -1 if format is incorrect, 0 if invalid version is specified,
 * otherwise returns the version number 1..6.
 */
int get_netpbm_version(char* image_version) {
    if (strlen(image_version) < 2 || image_version[0] != 'P') return -1;

    if (image_version[1] > '0' && image_version[1] < '7') return image_version[1] - '0';
    else return 0;
}

/**
 * Creates a file stream and reads the header of the image.
 * Used not to repeat the same code in the image opening functions.
 *
 * Returns NULL if error occurred, otherwise a pointer to the image_file
 * structure is returned.
 */
struct image_file* open_image_file(char* file_path) {
    // defining variables we'll need
    char image_version[3];
    uint width, height, scale;

    // opening the file
    FILE *stream = fopen(file_path, "r");
    if (stream == NULL) {
        printf("<netpbm>: could not open image file.\n");
        return NULL;
    }

    // reading the header
    if (read_header(stream, image_version, &width, &height, &scale) != 0) {
        fclose(stream);
        return NULL; // failed reading header
    }

    // check for the specified format
    int version = get_netpbm_version(image_version);
    if (version == -1) {
        printf("<netpbm>: there was an error reading the version.\n");
        return NULL;
    }

    // fill the struct with the acquired data and send it back in
    printf("<netpbm>: opened an image of format \"P%d\".\n", version);
    struct image_file *result = (struct image_file*)malloc(sizeof(struct image_file));
    *result = (struct image_file) { .width = width,
                                    .height = height,
                                    .scale = scale,
                                    .stream = stream,
                                    .version = version };

    // return the structure
    return result;
}

/**
 * Tries to open the file that contains the image,
 * then reads the header, which should consist of the image type (we expect P3 or P6),
 * width, height, both in pixels, and the color scale.
 *
 * Creates the rgb_image where it's going to put all the data.
 * After that tries to read width*height pixels, each containing 3 color channels and
 * saves it into the matrix of the rgb_image that was created earlier.
 *
 * Returns NULL in case of an error or a pointer to struct rgb_image.
 */
struct rgb_image* open_rgb_image(char *file_path) {
    printf("<netpbm>: opening the image at \"%s\".\n", file_path);

    // opening the file and reading the header
    struct image_file *image = open_image_file(file_path);
    if (image == NULL) return NULL;

    // the resulting image is going to be stored here
    struct rgb_image* result = create_rgb_image(image->width, image->height, image->scale);

    printf("<netpbm>: parsing the image...\n");

    // parsing the image according to the specified type
    int parse_result;
    switch(image->version) {
        case 3:
            parse_result = parse_rgb_body_ascii(image->stream, result);
            break;
        case 6:
            parse_result = parse_rgb_body_binary(image->stream, result);
            break;
        default:
            printf("<netpbm>: incorrect version of the image.\n");
            return NULL;
    }

    if (parse_result == 0) {
        printf("<netpbm>: successfully parsed the image.\n");
    }

    fclose(image->stream);
    free(image);

    return result;
}

/**
 * Reading RGB pixels represented as ASCII text from the image file.
 *
 * Returns -1 if error occurred, otherwise returns 0.
 */
int parse_rgb_body_ascii(FILE *stream, struct rgb_image *image) {
    // handy struct for the parsed pixels
    struct rgb_color pixel;

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fscanf(stream, "%u %u %u", &pixel.r, &pixel.g, &pixel.b);
            if (items_read < 3) {
                printf("<netpbm>: ASCII parsing error, incorrect format.\n");
                return -1;
            }

            // assign the color
            image->matrix[y][x] = (struct rgb_color) { .r = pixel.r, .g = pixel.g, .b = pixel.b };
        }
    }

    return 0;
}

/**
 * Reading RGB pixels represented as bytes from the image file.
 *
 * Returns -1 if error occurred, otherwise returns 0.
 */
int parse_rgb_body_binary(FILE *stream, struct rgb_image *image) {
    // this time chars will do
    unsigned char rgb[3];

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fread(rgb, sizeof(unsigned char), 3, stream);
            if (items_read < 3) {
                printf("<netpbm>: binary parsing error, incorrect format.\n");
                return -1;
            }

            // assign the color
            image->matrix[y][x] = (struct rgb_color) { .r = (uint)rgb[0], .g = (uint)rgb[1], .b = (uint)rgb[2] };
        }
    }

    return 0;
}

/**
 * Tries to open the file that contains the image,
 * then reads the header, which should consist of the image type (we expect P2 or P5),
 * width, height, both in pixels, and the color scale.
 *
 * Creates the grayscale_image where it's going to put all the data.
 * After that tries to read width*height pixels, each containing just one
 * unsigned integer or byte (for P5) and then saves it into the matrix of the grayscale_image
 * that was created earlier.
 *
 * Returns NULL in case of an error or a pointer to struct grayscale_image.
 */
struct grayscale_image* open_grayscale_image(char *file_path) {
    printf("<netpbm>: opening the image at \"%s\".\n", file_path);

    // opening the file and getting the header of it
    struct image_file *image = open_image_file(file_path);
    if (image == NULL) return NULL;

    // the resulting image is going to be stored here
    struct grayscale_image* result = create_grayscale_image(image->width, image->height, image->scale);

    printf("<netpbm>: parsing the image...\n");

    // parsing the image according to the specified type
    int parse_result;
    switch(image->version) {
        case 2:
            parse_result = parse_grayscale_body_ascii(image->stream, result);
            break;
        case 5:
            parse_result = parse_grayscale_body_binary(image->stream, result);
            break;
        default:
            printf("<netpbm>: incorrect version of the image.\n");
            return NULL;
    }

    if (parse_result == 0) {
        printf("<netpbm>: successfully parsed the image.\n");
    }

    fclose(image->stream);
    free(image);

    return result;
}

/**
 * Reading grayscale pixels represented as ASCII text from the image file.
 *
 * Returns -1 if error occurred, otherwise returns 0.
 */
int parse_grayscale_body_ascii(FILE *stream, struct grayscale_image *image) {
    // an unsigned integer for the grayscale value
    uint pixel;

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fscanf(stream, "%u", &pixel);
            if (items_read < 1) {
                printf("<netpbm>: ASCII parsing error, incorrect format.\n");
                return -1;
            }
        }
    }

    return 0;
}

/**
 * Reading grayscale pixels represented as bytes from the image file.
 *
 * Returns -1 if error occurred, otherwise returns 0.
 */
int parse_grayscale_body_binary(FILE *stream, struct grayscale_image *image) {
    // an unsigned integer for the grayscale value
    unsigned char pixel;

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fread(&pixel, sizeof(unsigned char), 1, stream);
            if (items_read < 1) {
                printf("<netpbm>: binary parsing error, incorrect format.\n");
                return -1;
            }
        }
    }

    return 0;
}

/**
 * Uses existing rgb_image structure to save it to disk in the P3 format
 *
 * Returns -1 if could not open file, otherwise returns 0.
 */
int write_rgb_image(char *file_path, struct rgb_image *image) {
    printf("<netpbm>: writing the image to disk...\n");

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

    printf("<netpbm>: image written in P3 RGB format at \"%s\"\n", file_path);

    fclose(stream);

    return 0;
}

/**
 * Uses existing grayscale_image structure to save it to disk in the P2 grayscale format
 *
 * Returns -1 if could not open file, otherwise returns 0.
 */
int write_grayscale_image(char *file_path, struct grayscale_image *image) {
    printf("<netpbm>: writing the image to disk...\n");

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

    printf("<netpbm>: image written in P2 grayscale format at \"%s\"\n", file_path);

    fclose(stream);

    return 0;
}