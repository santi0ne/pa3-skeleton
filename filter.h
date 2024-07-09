#ifndef _FILTER_H_
#define _FILTER_H_

#include "bmp.h"

typedef struct ThreadArgs {
	BMP_Image *image_in;
	BMP_Image *image_out;
	int boxFilter[3][3];
	int startRow, endRow;
} ThreadArgs;

void apply(BMP_Image * imageIn, BMP_Image * imageOut);

void applyParallel(BMP_Image * imageIn, BMP_Image * imageOut, int boxFilter[3][3], int numThreads);

void *filterThreadWorker(void * args);
#endif
