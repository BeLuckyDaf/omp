#include "sobel.h"
#include <math.h> // for the square root
#include <pthread.h>

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
 * Calculates horizontal and vertical magnitudes for the given pixel by
 * multiplying the corresponding matrices and then gets the square root of
 * the sum of squares to get the value.
 */
uint calculate_sobel_at(struct grayscale_image* image, int x, int y) {
    // initialize variables
    uint mag_x = 0;
    uint mag_y = 0;

    // matrix multiplication here
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            int yn = y + a - 1;
            int xn = x + b - 1;

            // if xn or yn is not in the borders of the image, ignore it
            if (yn < 0 || yn >= image->height || xn < 0 || xn >= image->width) continue;

            // add horizontal and vertical parts to their corresponding magnitudes
            mag_x += image->matrix[yn][xn] * sobel_kernel_x[a][b];
            mag_y += image->matrix[yn][xn] * sobel_kernel_y[a][b];
        }
    }

    // get the result
    return (uint) fmin(sqrt(mag_x * mag_x + mag_y * mag_y), image->scale);
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
            // get the sobel value for this pixel
            result->matrix[y][x] = calculate_sobel_at(image, x, y);
        }
    }

    return result;
}


struct grayscale_image* sobel_filter_rgb_multithreaded(struct rgb_image* image, int threads) {
    struct grayscale_image *gray = rgb_to_grayscale_image(image);
    struct grayscale_image *result = sobel_filter_grayscale_multithreaded(gray, threads);

    free_grayscale_image(gray);

    return result;
}


struct grayscale_image* sobel_filter_grayscale_multithreaded(struct grayscale_image* image, int threads) {
    if (threads < 1) {
        printf("<sobel>: number of threads cannot be less than one.\n");
        return NULL;
    }

    if (image == NULL) {
        printf("<sobel>: met NULL instead of an existing image.\n");
        return NULL;
    }

    struct grayscale_image *result = create_grayscale_image(image->width, image->height, image->scale);

    int image_size = image->width * image->height;
    int step = image_size/threads;

    int thread_count = image_size / step;
    pthread_t *thread_ids = (pthread_t*)calloc(thread_count, sizeof(pthread_t));

    for (int s = 0; s < image_size; s += step) {
        struct sobel_thread_task *task = (struct sobel_thread_task*)malloc(sizeof(struct sobel_thread_task));

        task->source_image = image;
        task->destination_image = result;
        task->from.y = s / image->width;
        task->from.x = s % image->width;
        task->to.y = (s + step) / image->width;
        task->to.x = (s + step) % image->width;

        if (task->to.y >= image->height) {
            task->to.y = image->height - 1;
            task->to.x = image->width - 1;
        }

        pthread_create(&thread_ids[s / step], NULL, sobel_filter_grayscale_thread_job, (void*)task);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    return result;
}

void* sobel_filter_grayscale_thread_job(void *data) {
    struct sobel_thread_task task = *((struct sobel_thread_task*) data);

    for (int y = task.from.y; y < task.to.y; y++) {
        int x_from = y == task.from.y ? task.from.x : 0;
        int x_to = y == task.to.y ? task.to.x : task.source_image->width;
        for (int x = x_from; x < x_to; x++) {
            // get the sobel value for this pixel
            task.destination_image->matrix[y][x] = calculate_sobel_at(task.source_image, x, y);
        }
    }

    free(data);

    return NULL;
}