#include "src/sobel.h"
#include <stdio.h>
#include <sys/time.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: <source path> <target path> <# of threads>\n");
        return 0;
    }

    char *source = argv[1];
    char *target = argv[2];

    int threads = 0;
    if (argc < 4) printf("<note>: number of threads to use was not specified => using one thread.\n");
    else threads = atoi(argv[3]);
    if (threads == 0) threads = 1;

    struct timeval sobel_start_time, sobel_stop_time, overall_start_time, overall_stop_time;

    gettimeofday(&overall_start_time, NULL);
    struct rgb_image* image = open_rgb_image(source);

    printf("<debug>: started sobel...\n");
    gettimeofday(&sobel_start_time, NULL);

    struct grayscale_image* sobel = sobel_filter_rgb(image, threads);

    gettimeofday(&sobel_stop_time, NULL);
    printf("<debug>: finished sobel operation.\n");

    write_grayscale_image(target, sobel);
    gettimeofday(&overall_stop_time, NULL);

    double sobel_time = (sobel_stop_time.tv_sec - sobel_start_time.tv_sec);
    if (sobel_stop_time.tv_usec < sobel_start_time.tv_usec) {
        sobel_time -= (sobel_start_time.tv_usec - sobel_stop_time.tv_usec)/1000000.0;
    } else {
        sobel_time += (sobel_stop_time.tv_usec - sobel_start_time.tv_usec)/1000000.0;
    }

    double overall_time = (overall_stop_time.tv_sec - overall_start_time.tv_sec);
    if (overall_stop_time.tv_usec < overall_start_time.tv_usec) {
        overall_time -= (overall_start_time.tv_usec - overall_stop_time.tv_usec)/1000000.0;
    } else {
        overall_time += (overall_stop_time.tv_usec - overall_start_time.tv_usec)/1000000.0;
    }

    printf("\n--------------\n\n");
    printf("<note>: sobel execution time: %f seconds.\n", sobel_time);
    printf("<note>: overall program execution time: %f seconds.\n", overall_time);

    return 0;
}