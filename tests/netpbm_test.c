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
    gettimeofday(&start_time, NULL);
    struct rgb_image* image = open_rgb_image("fullhd.ppm");
    struct grayscale_image* sobel = sobel_filter_rgb_multithreaded(image, 6);
    gettimeofday(&stop_time, NULL);

    printf("Elapsed time: %f\n", (stop_time.tv_usec - start_time.tv_usec)/1000000.0);

    write_grayscale_image("fullhd_sobel.ppm", sobel);
}

int main() {
    test_sobel();

    return 0;
}
