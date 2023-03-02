#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 1000

// Define complex number structure
typedef struct {
    double real;
    double imag;
} complex_t;

// Calculate square of complex number
complex_t square(complex_t z) {
    complex_t z_squared;
    z_squared.real = z.real*z.real - z.imag*z.imag;
    z_squared.imag = 2*z.real*z.imag;
    return z_squared;
}

// Calculate modulus of complex number
double modulus(complex_t z) {
    return sqrt(z.real*z.real + z.imag*z.imag);
}

// Check if point is in Mandelbrot set
int in_mandelbrot(complex_t c) {
    int i;
    complex_t z = {0, 0};
    for (i = 0; i < MAX_ITER; i++) {
        z = square(z);
        z.real += c.real;
        z.imag += c.imag;
        if (modulus(z) > 2) {
            return 0;
        }
    }
    return 1;
}

// Map pixel coordinates to complex plane coordinates
complex_t map_to_complex(int x, int y) {
    double real = (double)(x - WIDTH/2) / (double)(WIDTH/4);
    double imag = (double)(y - HEIGHT/2) / (double)(HEIGHT/4);
    return (complex_t) {real, imag};
}

// Generate Mandelbrot set image
void generate_image(uint8_t *image) {
    int x, y;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            complex_t c = map_to_complex(x, y);
            if (in_mandelbrot(c)) {
                image[y*WIDTH + x] = (uint8_t) 0;
            } else {
                image[y*WIDTH + x] = (uint8_t) 255;
            }
        }
    }
}

// Write image to file
void write_image(uint8_t *image) {
    int x, y;
    FILE *fp;
    fp = fopen("mandelbrot.ppm", "wb");
    fprintf(fp, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255);
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            fwrite(&image[y*WIDTH + x], sizeof(uint8_t), 1, fp);
            fwrite(&image[y*WIDTH + x], sizeof(uint8_t), 1, fp);
            fwrite(&image[y*WIDTH + x], sizeof(uint8_t), 1, fp);
        }
    }
    fclose(fp);
}

// Measure time taken to generate image
void measure_time(void (*func)(uint8_t *image)) {
    clock_t start = clock();

    uint8_t *image = (uint8_t*) malloc(WIDTH*HEIGHT*sizeof(uint8_t));
    func(image);

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", time_spent);

    write_image(image);
    free(image);
}

int main() {
    measure_time(generate_image);
    return 0;
}
