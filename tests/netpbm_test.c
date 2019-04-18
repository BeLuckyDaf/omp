#include "../src/sobel.h"
#include <stdio.h>

#include <sys/time.h>

void test_conversion() {
    struct rgb_image* image = create_rgb_image(5, 5, 255);
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            image->matrix[y][x] = (struct rgb_color) { .r=50, .g=0, .b=250 };
        }
    }

    struct grayscale_image* result = rgb_to_grayscale_image(image);

    free_rgb_image(image);
    free_grayscale_image(result);

    printf("Done.\n");
}

void test_file_parsing() {
    open_rgb_image("test.ppm");
}

void test_writing_rgb_file() {
    struct rgb_image* image = open_rgb_image("test256.ppm");
    write_rgb_image("written.ppm", image);
}

void test_convert_rgb_to_grayscale_and_write() {
    struct rgb_image* image = open_rgb_image("test.ppm");
    struct grayscale_image* gray = rgb_to_grayscale_image(image);
    write_grayscale_image("gray.ppm", gray);
}

void test_sobel() {
    struct timeval start_time, stop_time;
    struct rgb_image* image = open_rgb_image("16k.ppm");
    printf("<debug>: started sobel...\n");
    gettimeofday(&start_time, NULL);
    struct grayscale_image* sobel = sobel_filter_rgb(image, 32);
    gettimeofday(&stop_time, NULL);
    printf("<debug>: finished sobel operation.\n");

    double elapsed = (stop_time.tv_sec - start_time.tv_sec);
    if (stop_time.tv_usec < start_time.tv_usec) {
        elapsed -= (start_time.tv_usec - stop_time.tv_usec)/1000000.0;
    } else {
        elapsed += (stop_time.tv_usec - start_time.tv_usec)/1000000.0;
    }

    printf("Elapsed time: %f\n", elapsed);

    write_grayscale_image("16k_sobel.ppm", sobel);
}

int main() {
    test_sobel();

    return 0;
}
