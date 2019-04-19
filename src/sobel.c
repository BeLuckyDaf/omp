#include "sobel.h"
#include <math.h> // for the square root
#include <pthread.h>

/**
 * Kernel matrices, required for the sobel operation.
 */
const int sobel_kernel_x[3][3] =	{{-1, 0, 1},
					{ -2, 0, 2},
					{ -1, 0, 1}};

const int sobel_kernel_y[3][3] = 	{{-1, -2, -1},
					{  0,  0,  0},
					{  1,  2,  1}};


/**
 * Calculates horizontal and vertical magnitudes for the given pixel by
 * multiplying the corresponding matrices and then gets the square root of
 * the sum of squares to get the value.
 *
 * Returns the calculated sobel value.
 */
u_int32_t calculate_sobel_at(struct grayscale_image* image, int x, int y) {
	// initialize variables
	u_int32_t mag_x = 0;
	u_int32_t mag_y = 0;

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
	return (u_int32_t) fmin(sqrt(mag_x * mag_x + mag_y * mag_y), image->scale);
}

/**
 * Converts the given RGB image to grayscale and call sobel_filter_grayscale_multithreaded
 * with the given number of threads.
 *
 * Returns a pointer to the resulting image.
 */
struct grayscale_image* sobel_filter_rgb(struct rgb_image *image, int threads) {
	struct grayscale_image *gray = rgb_to_grayscale_image(image);
	struct grayscale_image *result = sobel_filter_grayscale(gray, threads);

	free_grayscale_image(gray);

	return result;
}

/**
 * Applies the sobel operator to the given grayscale image, dividing the
 * work between the given number of threads.
 *
 * Returns a pointer to the resulting image.
 */
struct grayscale_image* sobel_filter_grayscale(struct grayscale_image *image, int threads) {
	if (threads < 1) {
		printf("<sobel>: number of threads cannot be less than one.\n");
		return NULL;
	}

	if (image == NULL) {
		printf("<sobel>: met NULL instead of an existing image.\n");
		return NULL;
	}

	// create the resulting structure
	struct grayscale_image *result = create_grayscale_image(image->width, image->height, image->scale);

	// overall number of pixels and pixel step for the thread
	u_int32_t image_size = image->width * image->height;
	u_int32_t step = image_size/threads;

	u_int32_t thread_count = image_size / step;
	pthread_t *thread_ids = (pthread_t*)calloc(thread_count, sizeof(pthread_t));

	printf("<sobel>: launching threads...\n");

	for (u_int32_t s = 0; s < image_size; s += step) {
		struct sobel_thread_task *task = (struct sobel_thread_task*)malloc(sizeof(struct sobel_thread_task));

		// set up the data for the upcoming thread
		task->source_image = image;
		task->destination_image = result;

		// set the borders of calculation for this thread
		task->from.y = s / image->width;
		task->from.x = s % image->width;
		task->to.y = (s + step) / image->width;
		task->to.x = (s + step) % image->width;

		// make sure not to exceed the bounds of the image
		if (task->to.y >= image->height) {
			task->to.y = image->height - 1;
			task->to.x = image->width - 1;
		}

		// create the thread and add its id to the array of ids
		pthread_create(&thread_ids[s / step], NULL, sobel_filter_grayscale_thread_job, (void*)task);
	}

	printf("<sobel>: threads launched, working...\n");

	// wait for all threads to finish before continuing
	for (int i = 0; i < thread_count; i++) {
		pthread_join(thread_ids[i], NULL);
	}

	printf("<sobel>: all threads have finished.\n");

	free(thread_ids);

	return result;
}

/**
 * A helper function for the multithreaded sobel filter. Calculates sobel
 * in the specified region of the image.
 *
 * Returns NULL.
 */
void* sobel_filter_grayscale_thread_job(void *data) {
	// converting void pointer to a data structure
	struct sobel_thread_task task = *((struct sobel_thread_task*) data);

	// calculating sobel in the given region and saving the result
	// into the image provided by the calling function
	for (int y = task.from.y; y <= task.to.y; y++) {
		int x_from = y == task.from.y ? task.from.x : 0;
		int x_to = y == task.to.y ? task.to.x : task.source_image->width;
		for (int x = x_from; x < x_to; x++) {
			// get the sobel value for this pixel
			task.destination_image->matrix[y][x] = calculate_sobel_at(task.source_image, x, y);
		}
	}

	// free the memory since it was allocated specifically for this function call
	free(data);

	return NULL;
}
