#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s width height output.ppm\n", argv[0]);
        return 1;
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    if (width < 2 || height < 2) {
        fprintf(stderr, "Width and height must be at least 2.\n");
        return 1;
    }

    FILE *fp = fopen(argv[3], "wb");
    if (!fp) {
        perror("Could not create file");
        return 1;
    }

    fprintf(fp, "P6\n%d %d\n255\n", width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char rgb[3];

            rgb[0] = (unsigned char)(x * 255 / (width - 1));
            rgb[1] = (unsigned char)(y * 255 / (height - 1));
            rgb[2] = (unsigned char)((x + y) % 256);

            fwrite(rgb, 1, 3, fp);
        }
    }

    fclose(fp);
    return 0;
}
