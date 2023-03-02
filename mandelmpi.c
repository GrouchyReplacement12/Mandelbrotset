s#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define WIDTH 1200
#define HEIGHT 1200

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
double start_time= MPI_Wtime();
    double real_min = -2.0;
    double real_max = 1.0;
    double imag_min = -1.0;
    double imag_max = 1.0;
    int max_iter = 1000;

    int rows_per_process = HEIGHT / size;
    int start_row = rows_per_process * rank;
    int end_row = start_row + rows_per_process;
    if (rank == size - 1) {
        end_row = HEIGHT;
    }

    int* row = (int*)malloc(sizeof(int) * WIDTH);
    int* data = (int*)malloc(sizeof(int) * WIDTH * rows_per_process);

    for (int y = start_row; y < end_row; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double c_real = real_min + (real_max - real_min) * x / WIDTH;
            double c_imag = imag_min + (imag_max - imag_min) * y / HEIGHT;

            double z_real = 0.0;
            double z_imag = 0.0;

            int iter = 0;
            while (z_real * z_real + z_imag * z_imag < 4.0 && iter < max_iter) {
                double next_z_real = z_real * z_real - z_imag * z_imag + c_real;
                double next_z_imag = 2.0 * z_real * z_imag + c_imag;
                z_real = next_z_real;
                z_imag = next_z_imag;
                iter++;
            }

            if (iter == max_iter) {
                row[x] = 0;
            } else {
                row[x] = iter % 256;
            }
        }
        int row_index = (y - start_row) * WIDTH;
        for (int x = 0; x < WIDTH; x++) {
            data[row_index + x] = row[x];
        }
    }

    free(row);

    int* final_data = NULL;
    if (rank == 0) {
        final_data = (int*)malloc(sizeof(int) * WIDTH * HEIGHT * 3);
    }
    
    MPI_Gather(data, WIDTH * rows_per_process, MPI_INT, final_data, WIDTH * rows_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    free(data);

    if (rank == 0) {
    double end_time= MPI_Wtime();
    printf("Elapsed time: %f seconds\n", end_time-start_time);
        FILE* fp = fopen("mandelbrot.ppm", "wb");
        fprintf(fp, "P3\n%d %d\n255\n", WIDTH, HEIGHT);
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            fprintf(fp, "%d %d %d ", final_data[i], final_data[i], final_data[i]);
        }
        fclose(fp);
        free(final_data);
    }

    MPI_Finalize();
    return 0;
}
