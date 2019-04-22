#ifndef OMP_SOBEL_H
#define OMP_SOBEL_H

#include "netpbm.h" // we are going to need image structures

/* STRUCTURES */

/*
 * A helper structure for holding position
 */
struct pixel_position {
    u_int32_t x, y;
};

/*
 * Contains the task for a single thread to run, regarding
 * the multithreaded sobel operation.
 */
struct sobel_thread_task {
    struct grayscale_image *source_image, *destination_image;
    struct pixel_position from, to;
};

/* CONSTANTS */

extern const int sobel_kernel_x[3][3];
extern const int sobel_kernel_y[3][3];

/* FUNCTIONS */

/* Helpers */
void *_sobel_filter_grayscale_thread_job(void *data);
u_int32_t calculate_sobel_at(struct grayscale_image *image, int x, int y);

/* Sobel operation */
struct grayscale_image *sobel_filter_grayscale(struct grayscale_image *image, int threads);
struct grayscale_image *sobel_filter_rgb(struct rgb_image *image, int threads);

#endif // OMP_SOBEL_H
