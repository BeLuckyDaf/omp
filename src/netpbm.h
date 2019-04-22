#ifndef OMP_NETPBM_H
#define OMP_NETPBM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* DEFINES */

#define NETPBM_BLACKWHITE_ASCII 1
#define NETPBM_GRAYSCALE_ASCII 2
#define NETPBM_RGB_ASCII 3
#define NETPBM_BLACKWHITE_BINARY 4
#define NETPBM_GRAYSCALE_BINARY 5
#define NETPBM_RGB_BINARY 6

#define NETPBM_ASCII 1
#define NETPBM_BINARY 2

/* TYPES */

/* STRUCTURES */

/*
 * Contains RGB data for a single pixel
 */
struct rgb_color {
    u_int32_t r, g, b;
};

/*
 * Contains data about an RGB image in a 2D matrix, where an element
 * of the matrix is an array, which consists of 3 elements: R, G and B
 * color channel values
 */
struct rgb_image {
    u_int32_t width;
    u_int32_t height;
    u_int32_t scale;
    struct rgb_color **matrix;
};

/*
 * Contains data about a grayscale image in a 2D matrix, where an element
 * of the matrix is a grayscale value
 */
struct grayscale_image {
    u_int32_t width;
    u_int32_t height;
    u_int32_t scale;
    u_int32_t **matrix; // same as rgb, but no need for third level array, just a number
};

/*
 * Contains data about a black and white image in a 2D matrix, where an element
 * of the matrix is a 1 or a 0
 */
struct blackwhite_image {
    u_int32_t width;
    u_int32_t height;
    u_int8_t **matrix; // it is just 0 or 1, so one byte is enough
};

/*
 * A helper structure for File IO
 */
struct image_file {
    FILE *stream;
    int version;
    u_int32_t width;
    u_int32_t height;
    u_int32_t scale;
};

/* FUNCTIONS */

/* Creating and memory allocation */
struct rgb_image *create_rgb_image(u_int32_t width, u_int32_t height, u_int32_t scale);
struct grayscale_image *create_grayscale_image(u_int32_t width, u_int32_t height, u_int32_t scale);
struct blackwhite_image *create_blackwhite_image(u_int32_t width, u_int32_t height);

/* Image processing */
struct grayscale_image *rgb_to_grayscale_image(struct rgb_image *image);

/* File IO */
struct rgb_image *open_rgb_image(char *file_path);
struct grayscale_image *open_grayscale_image(char *file_path);
struct blackwhite_image *open_blackwhite_image(char *file_path);

struct image_file *open_image_file(char *file_path);

int write_rgb_image(char *file_path, struct rgb_image *image, int format);
int write_grayscale_image(char *file_path, struct grayscale_image *image, int format);
int write_blackwhite_image(char *file_path, struct blackwhite_image *image, int format);

static int _parse_rgb_body_ascii(FILE *stream, struct rgb_image *image);
static int _parse_rgb_body_binary(FILE *stream, struct rgb_image *image);
static int _parse_grayscale_body_ascii(FILE *stream, struct grayscale_image *image);
static int _parse_grayscale_body_binary(FILE *stream, struct grayscale_image *image);
static int _parse_blackwhite_body_ascii(FILE *stream, struct blackwhite_image *image);
static int _parse_blackwhite_body_binary(FILE *stream, struct blackwhite_image *image);

static int _read_header(FILE *stream, int *version, u_int32_t *width, u_int32_t *height, u_int32_t *scale);
static int _skip_comment(FILE *stream);

/* Miscellaneous */
static int _get_netpbm_version(char *image_version);

/* Releasing memory */
void free_rgb_image(struct rgb_image *image);
void free_grayscale_image(struct grayscale_image *image);
void free_blackwhite_image(struct blackwhite_image *image);

#endif // OMP_NETPBM_H
