#ifndef CODE_SOBEL_H
#define CODE_SOBEL_H

#include "netpbm.h" // we are going to need image structures

struct pixel_position {
    uint x, y;
};

struct sobel_thread_task {
    struct grayscale_image *source_image, *destination_image;
    struct pixel_position from, to;
};

extern const int sobel_kernel_x[3][3];
extern const int sobel_kernel_y[3][3];

struct grayscale_image* sobel_filter_rgb(struct rgb_image* image);
struct grayscale_image* sobel_filter_grayscale(struct grayscale_image* image);
uint calculate_sobel_at(struct grayscale_image* image, int x, int y);

// threads
struct grayscale_image* sobel_filter_grayscale_multithreaded(struct grayscale_image* image, int threads);
struct grayscale_image* sobel_filter_rgb_multithreaded(struct rgb_image* image, int threads);
void* sobel_filter_grayscale_thread_job(void *data);

#endif // CODE_SOBEL_H
