#include "../src/netpbm.h"
#include <stdio.h>

void test_conversion() {
    struct rgb_image* image = create_rgb_image(5, 5);
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

int main() {
    test_file_parsing();

    return 0;
}
