#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "filter.h"

void apply(BMP_Image * imageIn, BMP_Image * imageOut)
{
	for (int i=0; i<imageIn->norm_height; i++) {
		for (int j=0; j<imageIn->header.width_px; j++) {
			imageOut->pixels[i][j] = imageIn->pixels[i][j];
		}
	}
}

void applyParallel(BMP_Image * imageIn, BMP_Image * imageOut, int boxFilter[3][3], int numThreads)
{
	pthread_t threads[numThreads];
	ThreadArgs args[numThreads];

	int rows_per_thread = imageIn->norm_height/numThreads;
	for (int i=0; i<numThreads; i++) {
		args[i].image_in = imageIn;
		args[i].image_out = imageOut;
		args[i].startRow = i*rows_per_thread;
		args[i].endRow = (i==numThreads-1) ? imageIn->norm_height : args[i].startRow + rows_per_thread;
		for (int j = 0; j < 3; j++) {
            		for (int k = 0; k < 3; k++) {
                		args[i].boxFilter[j][k] = boxFilter[j][k];
            		}
        	}

        	if (pthread_create(&threads[i], NULL, filterThreadWorker, &args[i])) {
            		fprintf(stderr, "Error creating thread\n");
            		exit(EXIT_FAILURE);
        	}
    	}

    	for (int i = 0; i < numThreads; i++) {
        	pthread_join(threads[i], NULL);
    	}
}

void *filterThreadWorker(void * args)
{
	ThreadArgs *threadArgs = (ThreadArgs *)args;
	BMP_Image *image_in = threadArgs->image_in;
	BMP_Image *image_out = threadArgs->image_out;
	int (*boxFilter)[3] = threadArgs->boxFilter;
	int startRow = threadArgs->startRow;
	int endRow = threadArgs->endRow;

	int width = image_in->header.width_px;
	int height = image_in->norm_height;

	for (int y=startRow; y<endRow; y++) {
		for (int x=0; x<width; x++) {
			int red = 0, green = 0, blue = 0;
			for (int fy=0; fy<3; fy++) {
				for (int fx=0; fx<3; fx++) {
					int ix = x + fx - 1;
					int iy = y + fy - 1;
					if (ix>=0 && ix<width && iy>=0 && iy<height) {
						Pixel pixel = image_in->pixels[iy][ix];
						red += pixel.red * boxFilter[fy][fx];
						green += pixel.green * boxFilter[fy][fx];
						blue += pixel.blue * boxFilter[fy][fx];
					}
				}
			}
			image_out->pixels[y][x].red = red / 9;
			image_out->pixels[y][x].green = green / 9;
			image_out->pixels[y][x].blue = blue / 9;
		}
	}

	pthread_exit(NULL);
}
