#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "bmp.h"
#include "filter.h"

#define NUM_THREADS 4

/* crea directorio en donde estara el archivo resultado */
void createDirectoryIfNotExists(const char* path) {
	struct stat st = {0};
	if (stat(path, &st) == -1) {
		mkdir(path, 0700);
	}
}


int main(int argc, char **argv) {
	
	if (argc != 3) {
        	printf("Error: Uso incorrecto. Debe ser: %s <archivo_entrada.bmp> <archivo_salida.bmp>\n", argv[0]);
		printError(ARGUMENT_ERROR);
        	exit(EXIT_FAILURE);
    	}

    	char* source_name = argv[1];
    	char* dest_name = argv[2];
    	BMP_Image* image_in = NULL;
    	BMP_Image* image_out = NULL;

    	FILE* source_fp = fopen(source_name, "rb");
    	if (!source_fp) {
        	printf("Error al abrir el archivo de entrada '%s'\n", source_name);
        	printError(FILE_ERROR);
        	exit(EXIT_FAILURE);
    	}

	// creacion de directorio para archivo de salida 
	char dest_path[1024];
	snprintf(dest_path, sizeof(dest_path), "%s", dest_name);
	char *dir = dirname(dest_path);
	createDirectoryIfNotExists(dir);

    	FILE* dest_fp = fopen(dest_name, "wb");
    	if (!dest_fp) {
        	printf("Error al abrir el archivo de salida '%s'\n", dest_name);
        	printError(FILE_ERROR);
        	fclose(source_fp);
        	exit(EXIT_FAILURE);
    	}
    
    	// Asignar memoria para image_in antes de leer la imagen
    	image_in = (BMP_Image *)malloc(sizeof(BMP_Image));
    	if (image_in == NULL) {
        	printf("Error al asignar memoria para image_in\n");
        	free(image_in);
		fclose(source_fp);
        	fclose(dest_fp);
        	exit(EXIT_FAILURE);
    	}

    	// Leer la imagen de entrada
    	readImage(source_fp, image_in);
    	if (!image_in) {
        	printf("Error al leer la imagen de entrada '%s'\n", source_name);
        	printError(VALID_ERROR);
		free(image_in);
        	fclose(source_fp);
        	fclose(dest_fp);
        	exit(EXIT_FAILURE);
    	}
    
    	fseek(source_fp, 0, SEEK_SET);

    	// Aplicar filtro a la imagen de entrada y escribir la imagen resultante
    	int boxFilter[3][3] = {
        	{1, 1, 1},
        	{1, 1, 1},
        	{1, 1, 1}
    	};

    	image_out = createBMPImage(source_fp);  // Crear image_out NO es necesario aquí
    	if (!image_out) {
        	printf("Error al crear la imagen de salida '%s'\n", dest_name);
        	printError(FILE_ERROR);
        	freeImage(image_in);
        	fclose(source_fp);
        	fclose(dest_fp);
        	exit(EXIT_FAILURE);
    	}

    	applyParallel(image_in, image_out, boxFilter, NUM_THREADS);
    	
    	writeImage(dest_name, image_out);

    	// Mostrar información de la imagen de salida
    	printBMPHeader(&image_out->header);
    	printf("\n");
    	printBMPImage(image_out);

    	// Liberar memoria y cerrar archivos
    	freeImage(image_in);
    	freeImage(image_out);
    	fclose(source_fp);
    	fclose(dest_fp);

    	exit(EXIT_SUCCESS);
}
