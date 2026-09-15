#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

typedef struct {
    int width, height, channels;
    unsigned char *data;
} Image;

static void die(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

static void skip_ws_and_comments(FILE *fp) {
    int c;
    while (1) {
        c = fgetc(fp);
        if (c == '#') {
            while (c != '\n' && c != EOF) c = fgetc(fp);
        } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            continue;
        } else {
            if (c != EOF) ungetc(c, fp);
            return;
        }
    }
}

static Image read_ppm(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) die("Could not open input image.");

    char magic[3] = {0};
    if (fread(magic, 1, 2, fp) != 2 || strcmp(magic, "P6") != 0)
        die("Only binary PPM (P6) is supported.");

    skip_ws_and_comments(fp);
    int width = 0;
    if (fscanf(fp, "%d", &width) != 1) die("Invalid width.");

    skip_ws_and_comments(fp);
    int height = 0;
    if (fscanf(fp, "%d", &height) != 1) die("Invalid height.");

    skip_ws_and_comments(fp);
    int maxval = 0;
    if (fscanf(fp, "%d", &maxval) != 1 || maxval != 255)
        die("Only max value 255 is supported.");

    fgetc(fp);

    Image img = {width, height, 3, NULL};
    size_t bytes = (size_t)width * height * 3;
    img.data = (unsigned char *)malloc(bytes);
    if (!img.data) die("Memory allocation failed.");

    if (fread(img.data, 1, bytes, fp) != bytes)
        die("Unexpected end of image.");

    fclose(fp);
    return img;
}

static void write_ppm(const char *filename, const Image *img) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) die("Could not create output image.");

    fprintf(fp, "P6\n%d %d\n255\n", img->width, img->height);
    size_t bytes = (size_t)img->width * img->height * 3;

    if (fwrite(img->data, 1, bytes, fp) != bytes)
        die("Could not write output image.");

    fclose(fp);
}

static omp_sched_t parse_schedule(const char *name) {
    if (strcmp(name, "static") == 0) return omp_sched_static;
    if (strcmp(name, "dynamic") == 0) return omp_sched_dynamic;
    if (strcmp(name, "guided") == 0) return omp_sched_guided;

    fprintf(stderr,
            "Invalid schedule '%s'. Use static, dynamic, or guided.\n",
            name);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr,
                "Usage: %s input.ppm output.ppm threads schedule chunk\n",
                argv[0]);
        fprintf(stderr,
                "Example: %s input.ppm output.ppm 4 static 1\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    int threads = atoi(argv[3]);
    const char *schedule_name = argv[4];
    int chunk = atoi(argv[5]);

    if (threads <= 0 || chunk <= 0)
        die("threads and chunk must be positive.");

    Image in = read_ppm(argv[1]);

    Image out = {
        in.width,
        in.height,
        3,
        (unsigned char *)calloc((size_t)in.width * in.height * 3, 1)
    };
    if (!out.data) die("Memory allocation failed.");

    const int K[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };
    const int KSUM = 16;

    omp_set_num_threads(threads);
    omp_set_schedule(parse_schedule(schedule_name), chunk);

    double start = omp_get_wtime();

    #pragma omp parallel for schedule(runtime)
    for (int y = 0; y < in.height; ++y) {
        for (int x = 0; x < in.width; ++x) {
            for (int c = 0; c < 3; ++c) {
                int sum = 0;

                for (int ky = -1; ky <= 1; ++ky) {
                    int yy = y + ky;
                    if (yy < 0) yy = 0;
                    if (yy >= in.height) yy = in.height - 1;

                    for (int kx = -1; kx <= 1; ++kx) {
                        int xx = x + kx;
                        if (xx < 0) xx = 0;
                        if (xx >= in.width) xx = in.width - 1;

                        sum += in.data[(yy * in.width + xx) * 3 + c]
                              * K[ky + 1][kx + 1];
                    }
                }

                int value = sum / KSUM;
                if (value < 0) value = 0;
                if (value > 255) value = 255;

                out.data[(y * out.width + x) * 3 + c] =
                    (unsigned char)value;
            }
        }
    }

    double elapsed = omp_get_wtime() - start;

    write_ppm(argv[2], &out);

    printf("Parallel Gaussian Blur\n");
    printf("Image: %dx%d\n", in.width, in.height);
    printf("Threads: %d\n", threads);
    printf("Schedule: %s, chunk=%d\n", schedule_name, chunk);
    printf("Execution Time: %.6f seconds\n", elapsed);

    free(in.data);
    free(out.data);
    return 0;
}
