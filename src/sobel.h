#ifndef CODE_SOBEL_H
#define CODE_SOBEL_H

#include "netpbm.h" // we are going to need image structures

extern const int sobel_kernel_x[3][3];

extern const int sobel_kernel_y[3][3];

struct grayscale_image* sobel_filter_rgb(struct rgb_image* image);
struct grayscale_image* sobel_filter_grayscale(struct grayscale_image* image);

#endif // CODE_SOBEL_H
