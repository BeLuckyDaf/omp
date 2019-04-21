#include "src/sobel.h"
#include <stdio.h>
#include <sys/time.h>
#include "src/colors.h"

double get_timestamp(struct timeval from, struct timeval to) {
    double timestamp = (to.tv_sec - from.tv_sec);
    if (to.tv_usec < from.tv_usec) {
        timestamp -= (from.tv_usec - to.tv_usec)/1000000.0;
    } else {
        timestamp += (to.tv_usec - from.tv_usec)/1000000.0;
    }

    return timestamp;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Usage: <source path> <target path> <# of threads>\n");
		return 0;
	}

	// file paths
	char *source = argv[1];
	char *target = argv[2];

	// find out how many threads to use
	int threads = 0;
	if (argc < 4) printf("<note>: number of threads to use was not specified => using one thread.\n");
	else threads = atoi(argv[3]);
	if (threads == 0) threads = 1;

	// timer structures
	struct timeval sobel_start_time, sobel_stop_time, overall_start_time, overall_stop_time;

	// set the overall program timer
	gettimeofday(&overall_start_time, NULL);

	// open the image
	struct rgb_image* image = open_rgb_image(source);
	if (image == NULL) return -1;

	// set the sobel operation timer
	gettimeofday(&sobel_start_time, NULL);

	// perform the sobel operation
	struct grayscale_image* sobel = sobel_filter_rgb(image, threads);
	if (sobel == NULL) return -1;

	// stop the sobel timer
	gettimeofday(&sobel_stop_time, NULL);

	// write sobel image to disk
	write_grayscale_image(target, sobel, NETPBM_ASCII);

	// stop the overall timer
	gettimeofday(&overall_stop_time, NULL);

	// calculate how much time the sobel operation has taken
	double sobel_time = get_timestamp(sobel_start_time, sobel_stop_time);

	// calculate how much time the program has taken overall
	double overall_time = get_timestamp(overall_start_time, overall_stop_time);

	free_grayscale_image(sobel);
	free_rgb_image(image);

	// print it all
	printf("-----------------------------------------\n\n");
	printf("<note>: sobel execution time: %s%f%s seconds.\n", AC_GREEN, sobel_time, AC_RESET);
	printf("<note>: overall program execution time: %s%f%s seconds.\n", AC_GREEN, overall_time, AC_RESET);

	return 0;
}
