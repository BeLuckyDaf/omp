#include "sobel.h"
#include <math.h> // for the square root

const int sobel_kernel_x[3][3] = {{-1, 0, 1},
                                  {-2, 0, 2},
                                  {-1, 0, 1}};

const int sobel_kernel_y[3][3] = {{-1, -2, -1},
                                  { 0,  0,  0},
                                  { 1,  2,  1}};

/**
 * First convert rgb to grayscale, then call filter_grayscale to
 * perform the operation.
 */
struct grayscale_image* sobel_filter_rgb(struct rgb_image* image) {
    struct grayscale_image *gray = rgb_to_grayscale_image(image);
    struct grayscale_image *result = sobel_filter_grayscale(gray);

    free_grayscale_image(gray);

    return result;
}

/**
 * Creates another grayscale image, then applies sobel operation to the
 * provided image, saving the result into the newly created one.
 */
struct grayscale_image* sobel_filter_grayscale(struct grayscale_image* image) {
    // the resulting image will be stored here
    struct grayscale_image *result = create_grayscale_image(image->width, image->height, image->scale);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // define the variables
            uint magnitude_x = 0;
            uint magnitude_y = 0;

            // matrix multiplication here
            for (int a = 0; a < 3; a++) {
                for (int b = 0; b < 3; b++) {
                    int yn = y + a - 1;
                    int xn = x + b - 1;

                    // if xn or yn is not in the borders of the image, ignore it
                    if (yn < 0 || yn >= image->height || xn < 0 || xn >= image->width)
                        continue;

                    // add horizontal and vertical parts to their corresponding magnitudes
                    magnitude_x += image->matrix[yn][xn] * sobel_kernel_x[a][b];
                    magnitude_y += image->matrix[yn][xn] * sobel_kernel_y[a][b];
                }
            }

            // put the result into the variable
            result->matrix[y][x] = (uint) sqrt(magnitude_x*magnitude_x + magnitude_y*magnitude_y);
        }
    }

    return result;
}