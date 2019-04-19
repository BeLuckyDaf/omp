#include "netpbm.h"

/**
 * Allocates memory for the given dimensions of an RGB image.
 *
 * Returns a pointer to the rgb_image structure.
 */
struct rgb_image* create_rgb_image(u_int32_t width, u_int32_t height, u_int32_t scale) {
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
struct grayscale_image* create_grayscale_image(u_int32_t width, u_int32_t height, u_int32_t scale) {
    // allocate for the structure
    struct grayscale_image* image = (struct grayscale_image*)malloc(sizeof(struct grayscale_image));

    // set the dimensions
    image->width = width;
    image->height = height;
    image->scale = scale;

    // allocate a pointer to hold pointers to rows
    // and pointers for the rows themselves
    image->matrix = (u_int32_t**)calloc(height, sizeof(u_int32_t*));
    for (int i = 0; i < height; i++) {
        image->matrix[i] = (u_int32_t*)calloc(width, sizeof(u_int32_t));
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
* Allocates memory for the given dimensions of a grayscale image.
*
* Returns a pointer to the blackwhite_image structure.
*/
struct blackwhite_image* create_blackwhite_image(u_int32_t width, u_int32_t height) {
    // allocate for the structure
    struct blackwhite_image* image = (struct blackwhite_image*)malloc(sizeof(struct blackwhite_image));

    // set the dimensions
    image->width = width;
    image->height = height;

    // allocate a pointer to hold pointers to rows
    // and pointers for the rows themselves
    image->matrix = (u_int8_t**)calloc(height, sizeof(u_int8_t*));
    for (int i = 0; i < height; i++) {
        image->matrix[i] = (u_int8_t*)calloc(width, sizeof(u_int8_t));
    }

    return image;
}

/**
 * Completely frees the allocated memory for the black and white image structure
 */
void free_blackwhite_image(struct blackwhite_image *image) {
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

    for (u_int32_t y = 0; y < image->height; y++) {
        for (u_int32_t x = 0; x < image->width; x++) {
            u_int32_t average = (image->matrix[y][x].r + image->matrix[y][x].g + image->matrix[y][x].b)/3;
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
int read_header(FILE *stream, int *version, u_int32_t *width, u_int32_t *height, u_int32_t *scale) {
    char image_version_str[3];
    int items_read = fscanf(stream, "%2s", image_version_str);

    // check for the specified format
    *version = get_netpbm_version(image_version_str);
    if (*version == -1) {
        printf("<netpbm>: there was an error reading the version.\n");
        return -1;
    }

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

    if (*version == NETPBM_BLACKWHITE_ASCII || *version == NETPBM_BLACKWHITE_BINARY) {
        *scale = 1;
        return 0;
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
    u_int32_t width, height, scale;
    int version;

    // opening the file
    FILE *stream = fopen(file_path, "r");
    if (stream == NULL) {
        printf("<netpbm>: could not open image file.\n");
        return NULL;
    }

    // reading the header
    if (read_header(stream, &version, &width, &height, &scale) != 0) {
        fclose(stream);
        return NULL; // failed reading header
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
        case NETPBM_RGB_ASCII:
            parse_result = parse_rgb_body_ascii(image->stream, result);
            break;
        case NETPBM_RGB_BINARY:
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
    u_int8_t rgb[3];

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fread(rgb, sizeof(u_int8_t), 3, stream);
            if (items_read < 3) {
                printf("<netpbm>: binary parsing error, incorrect format.\n");
                return -1;
            }

            // assign the color
            image->matrix[y][x] = (struct rgb_color) { .r = (u_int32_t)rgb[0], .g = (u_int32_t)rgb[1], .b = (u_int32_t)rgb[2] };
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
        case NETPBM_GRAYSCALE_ASCII:
            parse_result = parse_grayscale_body_ascii(image->stream, result);
            break;
        case NETPBM_GRAYSCALE_BINARY:
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
    u_int32_t pixel;

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fscanf(stream, "%u", &pixel);
            if (items_read < 1) {
                printf("<netpbm>: ASCII parsing error, incorrect format.\n");
                return -1;
            }

            // assign the color
            image->matrix[y][x] = pixel;
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
    u_int8_t pixel;

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fread(&pixel, sizeof(u_int8_t), 1, stream);
            if (items_read < 1) {
                printf("<netpbm>: binary parsing error, incorrect format.\n");
                return -1;
            }

            // assign the color
            image->matrix[y][x] = pixel;
        }
    }

    return 0;
}

/**
 * Tries to open the file that contains the image,
 * then reads the header, which should consist of the image type (we expect P1 or P4),
 * width, height, both in pixels.
 *
 * Creates the blackwhite_image where it's going to put all the data.
 * After that tries to read width*height pixels, each containing just one
 * unsigned integer or byte (for P4) and then saves it into the matrix of the blackwhite_image
 * that was created earlier.
 *
 * Returns NULL in case of an error or a pointer to struct blackwhite_image.
 */
struct blackwhite_image* open_blackwhite_image(char *file_path) {
    printf("<netpbm>: opening the image at \"%s\".\n", file_path);

    // opening the file and getting the header of it
    struct image_file *image = open_image_file(file_path);
    if (image == NULL) return NULL;

    // the resulting image is going to be stored here
    struct blackwhite_image* result = create_blackwhite_image(image->width, image->height);

    printf("<netpbm>: parsing the image...\n");

    // parsing the image according to the specified type
    int parse_result;
    switch(image->version) {
        case NETPBM_BLACKWHITE_ASCII:
            parse_result = parse_blackwhite_body_ascii(image->stream, result);
            break;
        case NETPBM_BLACKWHITE_BINARY:
            parse_result = parse_blackwhite_body_binary(image->stream, result);
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
int parse_blackwhite_body_ascii(FILE *stream, struct blackwhite_image *image) {
    // an unsigned integer for the grayscale value
    char pixel;

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fread(&pixel, sizeof(char), 1, stream);
            if (items_read < 1) {
                printf("<netpbm>: ASCII parsing error, incorrect format.\n");
                return -1;
            } else if (pixel == '\n' || pixel == ' ') {
                x--;
                continue;
            }

            // assign the color
            image->matrix[y][x] = pixel == '0' ? 0 : 1;
        }
    }

    return 0;
}

/**
 * Reading black and white image pixels represented as bytes from the image file.
 *
 * Returns -1 if error occurred, otherwise returns 0.
 */
int parse_blackwhite_body_binary(FILE *stream, struct blackwhite_image *image) {
    // an unsigned integer for the grayscale value
    u_int8_t pixel;

    // parsing width * height pixels
    int items_read;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            items_read = fread(&pixel, sizeof(u_int8_t), 1, stream);
            if (items_read < 1) {
                printf("<netpbm>: binary parsing error, incorrect format.\n");
                return -1;
            }

            image->matrix[y][x] = pixel;
        }
    }

    return 0;
}

/**
 * Uses existing rgb_image structure to save it to disk in the P3 format
 *
 * Returns -1 if could not open file, otherwise returns 0.
 */
int write_rgb_image(char *file_path, struct rgb_image *image, int format) {
    printf("<netpbm>: writing the image to disk...\n");

    if (format != NETPBM_ASCII && format != NETPBM_BINARY) {
        printf("<netpbm>: could not write, incorrect format specified.\n");
        return -1;
    }

    // open or create the file
    FILE *stream = fopen(file_path, "w");
    if (stream == NULL) {
        printf("<netpbm>: could not open file for writing.\n");
        return -1;
    }

    // write the header to file
    int version = format == NETPBM_ASCII ? NETPBM_RGB_ASCII : NETPBM_RGB_BINARY;
    fprintf(stream, "P%d\n%u %u\n%u\n", version, image->width, image->height, image->scale);

    // write all pixels down line by line
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            if (format == NETPBM_ASCII) {
                fprintf(stream, "%u %u %u ", image->matrix[y][x].r, image->matrix[y][x].g, image->matrix[y][x].b);
            }
            else {
                fwrite(&image->matrix[y][x], sizeof(struct rgb_color), 1, stream);
            }
        }
        if (format == NETPBM_ASCII) fprintf(stream, "\n");
    }

    printf("<netpbm>: image written in P%d RGB format in \"%s\"\n", version, file_path);

    fclose(stream);

    return 0;
}

/**
 * Uses existing grayscale_image structure to save it to disk in the P2 grayscale format
 *
 * Returns -1 if could not open file, otherwise returns 0.
 */
int write_grayscale_image(char *file_path, struct grayscale_image *image, int format) {
    printf("<netpbm>: writing the image to disk...\n");

    if (format != NETPBM_ASCII && format != NETPBM_BINARY) {
        printf("<netpbm>: could not write, incorrect format specified.\n");
        return -1;
    }

    // open or create the file
    FILE *stream = fopen(file_path, "w");
    if (stream == NULL) {
        printf("<netpbm>: could not open file for writing.\n");
        return -1;
    }

    // write the header to file
    int version = format == NETPBM_ASCII ? NETPBM_GRAYSCALE_ASCII : NETPBM_GRAYSCALE_BINARY;
    fprintf(stream, "P%d\n%u %u\n%u\n", version, image->width, image->height, image->scale);

    // write all pixels down line by line
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            if (format == NETPBM_ASCII) fprintf(stream, "%u ", image->matrix[y][x]);
            else fwrite(&image->matrix[y][x], sizeof(u_int8_t), 1, stream);
        }
        if (format == NETPBM_ASCII) fprintf(stream, "\n");
    }

    printf("<netpbm>: image written in P%d grayscale format in \"%s\"\n", version, file_path);

    fclose(stream);

    return 0;
}

/**
 * Uses existing blackwhite_image structure to save it to disk in the P1
 * black and white format
 *
 * Returns -1 if could not open file, otherwise returns 0.
 */
int write_blackwhite_image(char *file_path, struct blackwhite_image *image, int format) {
    printf("<netpbm>: writing the image to disk...\n");

    if (format != NETPBM_ASCII && format != NETPBM_BINARY) {
        printf("<netpbm>: could not write, incorrect format specified.\n");
        return -1;
    }

    // open or create the file
    FILE *stream = fopen(file_path, "w");
    if (stream == NULL) {
        printf("<netpbm>: could not open file for writing.\n");
        return -1;
    }

    // write the header to file
    int version = format == NETPBM_ASCII ? NETPBM_BLACKWHITE_ASCII : NETPBM_BLACKWHITE_BINARY;
    fprintf(stream, "P%d\n%u %u\n", version, image->width, image->height);

    // write all pixels down line by line
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            if (format == NETPBM_ASCII) fprintf(stream, "%hhu ", image->matrix[y][x]);
            else fwrite(&image->matrix[y][x], sizeof(u_int8_t), 1, stream);
        }
        if (format == NETPBM_ASCII) fprintf(stream, "\n");
    }

    printf("<netpbm>: image written in P%d black and white format in \"%s\"\n", version, file_path);

    fclose(stream);

    return 0;
}