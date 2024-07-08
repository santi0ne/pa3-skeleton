#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "bmp.h"

void createDirectoryIfNotExists(const char* path) {
	struct stat st = {0};
	if (stat(path, &st) == -1) {
		mkdir(path, 0700);
	}
}

void compareFiles(char* file1, char* file2) {
	FILE* fp1 = fopen(file1, "rb");
	FILE *fp2 = fopen(file2, "rb");

	if (!fp1 || !fp2) {
        	printf("error al abrir los archivos para comparacion\n");
        	return;
    	}

    	fseek(fp1, 0, SEEK_END);
    	fseek(fp2, 0, SEEK_END);
    	long size1 = ftell(fp1);
    	long size2 = ftell(fp2);
    	rewind(fp1);
    	rewind(fp2);

    	if (size1 != size2) {
        	printf("Files have different sizes: %ld vs %ld bytes.\n", size1, size2);
        	fclose(fp1);
        	fclose(fp2);
		return;
    	}

    	int equal = 1;
    	for (long i = 0; i < size1; i++) {
        	if (fgetc(fp1) != fgetc(fp2)) {
            		equal = 0;
            		break;
        	}
    	}

    	fclose(fp1);
    	fclose(fp2);

    	if (equal) {
        	printf("Files are identical.\n");
    	} else {
        	printf("Files are different.\n");
    	}

}


int main(int argc, char **argv) {
	char* source_name = argv[1];
  	char* dest_name = argv[2];
  	BMP_Image* image = NULL;

  	if (argc != 3) {
    		printError(ARGUMENT_ERROR);
    		exit(EXIT_FAILURE);
  	}
  
  	FILE* source_fp = fopen(source_name, "rb");
  	if (!source_fp) {  
  		printf("aqui en source\n");
    		printError(FILE_ERROR);
    		exit(EXIT_FAILURE);
  	}

	/*
  	if((dest = fopen(argv[2], "wb")) == NULL) {
    		printf("aqui en dest\n");
    		printError(FILE_ERROR);
    		exit(EXIT_FAILURE);
  	} 
	*/

  	image = createBMPImage(source_fp);

  	int row_size = ((image->header.width_px * image->bytes_per_pixel + 3) / 4) * 4;
  	int data_size = row_size * image->norm_height;

  	readImageData(source_fp, image, data_size);

  	fclose(source_fp);

	char dest_path[1024];
	strcpy(dest_path, dest_name);
	char* dir = dirname(dest_path);
	createDirectoryIfNotExists(dir);

  	writeImage(dest_name, image);

  	compareFiles(source_name, dest_name);


  	if(!checkBMPValid(&image->header)) {
    		printf("Error al check bmp\n");
    		printError(VALID_ERROR);
    		//exit(EXIT_FAILURE);
  	}  

 	printBMPHeader(&image->header);
  	printf("\n");
  	printBMPImage(image);

  	freeImage(image);
  
 	exit(EXIT_SUCCESS);
}
