
/*
 *
 * Operating System Design / Diseño de Sistemas Operativos
 * (c) ARCOS.INF.UC3M.ES
 *
 * @file 	test.c
 * @brief 	Implementation of the client test routines.
 * @date	01/03/2017
 *
 */


#include <stdio.h>
#include <string.h>
#include "filesystem/filesystem.h"


// Color definitions for asserts
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"

#define N_BLOCKS 300			  // Number of blocks in the device
#define DEV_SIZE N_BLOCKS *BLOCK_SIZE // Device size, in bytes

int main()
{
	int ret;
	long tamano;

	/*PRUEBA GENERAL*/
	printf("\nPRUEBA GENERAL\n");
	ret = mkFS(DEV_SIZE);
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = mountFS();
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = createFile("prueba1");
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = openFile("prueba1");
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	/////////

	char *buffer1 = "12345";
	ret = writeFile(0, buffer1, strlen(buffer1));
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////
	
	ret = lseekFile(0, 0, FS_SEEK_BEGIN);
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	char c [10] ;
	ret = readFile(0, c, 5);
	if (strcmp(c,"12345") != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	////////

	ret = lseekFile(0, -3, FS_SEEK_CUR);
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////////

	char d [10];
	ret = readFile(0, d, 3);
	if (strcmp(d,"345") != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	/////////
	
	ret = lseekFile(0, 0, FS_SEEK_END);
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////////

	char e [10];
	ret = readFile(0, e, 3);
	if (strcmp(e,"") != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	/////////////

	char *buffer2 = "Esto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturass";
	ret = writeFile(0, buffer2, 5800);
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = lseekFile(0, 0, FS_SEEK_BEGIN);
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	char f [6000];
	char *comparar = "12345Esto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturassEsto es un texto de prueba que ocupa exactamente 100 huecos para ver que tal funcionan las lecturass";
	ret = readFile(0, f, 5805);
	if (strcmp(f, comparar) != 0)
	{
		printf("he leido la cadena %s\n", f);
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	ret = closeFile(0);
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	ret = createFile("prueba1_2");
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	////

	ret = includeIntegrity("prueba1_2");
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST includeIntegrity ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST includeIntegrity ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	////

	ret = checkFile("prueba1_2");
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	////

	ret = openFileIntegrity("prueba1_2");
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileIntegrity ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileIntegrity ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	////

	ret = closeFileIntegrity(1);
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileIntegrity ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileIntegrity ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	////

	ret = createLn("prueba1", "enlace1");
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLn ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLn ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	////

	ret = openFile("enlace1");
	int descriptor = ret;
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileEnlace ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileEnlace ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	/////

	char g [10];
	ret = readFile(descriptor, g, 2);
	if (strcmp(g,"12") != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFileEnlace ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFileEnlace ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	/////

	ret = closeFile(descriptor);
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileEnlace ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileEnlace ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//////


	ret = removeLn("enlace1");
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeLn ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeLn ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	////////

	ret = openFile("prueba1");
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileEnlace ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileEnlace ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);


	///////

	ret = removeFile("prueba1");
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	ret = unmountFS();
	if (ret < 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);






		/*PRUEBA mkFS mayor y mkFS menor*/
	printf("\nPRUEBA mkFS mayor y mkFS menor\n");
	tamano = 2000;
	ret = mkFS(tamano);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFSErrorMenor ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFSErrorMenor ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	tamano = 6000000;
	ret = mkFS(tamano);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFSErrorMayor ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFSErrorMayor ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);




	
	/*PRUEBA unmountFS y mountFS*/
	printf("\nPRUEBA unmountFS y mountFS\n");
	mkFS(DEV_SIZE);
	mountFS();
	ret = mountFS();
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFSErrorMontado ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFSErrorMontado ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	unmountFS();
	ret = unmountFS();
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFSErrorDesmontado ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFSErrorDesmontado ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);





	/*PRUEBA createFileError y removeFileError*/
	printf("\nPRUEBA createFileError y removeFileError\n");
	mkFS(DEV_SIZE);

	///////

	ret = createFile("createFileErrorSinMontar");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFileErrorSinMontar ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFileErrorSinMontar ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);


	///////

	ret = removeFile("removeFileSinMontar");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFileSinMontar ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFileSinMontar ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	mountFS();
	ret = createFile("pruebaConUnNombreDeMasDeTreintaydosCaracteres");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFileErrorNombre+32 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFileErrorNombre+32 ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	createFile("createFileError+48Fich1");
	createFile("createFileError+48Fich2");
	createFile("createFileError+48Fich3");
	createFile("createFileError+48Fich4");
	createFile("createFileError+48Fich5");
	createFile("createFileError+48Fich6");
	createFile("createFileError+48Fich7");
	createFile("createFileError+48Fich8");
	createFile("createFileError+48Fich9");
	createFile("createFileError+48Fich10");
	createFile("createFileError+48Fich11");
	createFile("createFileError+48Fich12");
	createFile("createFileError+48Fich13");
	createFile("createFileError+48Fich14");
	createFile("createFileError+48Fich15");
	createFile("createFileError+48Fich16");
	createFile("createFileError+48Fich17");
	createFile("createFileError+48Fich18");
	createFile("createFileError+48Fich19");
	createFile("createFileError+48Fich20");
	createFile("createFileError+48Fich21");
	createFile("createFileError+48Fich22");
	createFile("createFileError+48Fich23");
	createFile("createFileError+48Fich24");
	createFile("createFileError+48Fich25");
	createFile("createFileError+48Fich26");
	createFile("createFileError+48Fich27");
	createFile("createFileError+48Fich28");
	createFile("createFileError+48Fich29");
	createFile("createFileError+48Fich30");
	createFile("createFileError+48Fich31");
	createFile("createFileError+48Fich32");
	createFile("createFileError+48Fich33");
	createFile("createFileError+48Fich34");
	createFile("createFileError+48Fich35");
	createFile("createFileError+48Fich36");
	createFile("createFileError+48Fich37");
	createFile("createFileError+48Fich38");
	createFile("createFileError+48Fich39");
	createFile("createFileError+48Fich40");
	createFile("createFileError+48Fich41");
	createFile("createFileError+48Fich42");
	createFile("createFileError+48Fich43");
	createFile("createFileError+48Fich44");
	createFile("createFileError+48Fich45");
	createFile("createFileError+48Fich46");
	createFile("createFileError+48Fich47");
	createFile("createFileError+48Fich48");
	ret = createFile("createFileError+48Fich49");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFileError+48Fich ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFileError+48Fich ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	removeFile("createFileError+48Fich1");
	removeFile("createFileError+48Fich2");
	removeFile("createFileError+48Fich3");
	removeFile("createFileError+48Fich4");
	removeFile("createFileError+48Fich5");
	removeFile("createFileError+48Fich6");
	removeFile("createFileError+48Fich7");
	removeFile("createFileError+48Fich8");
	removeFile("createFileError+48Fich9");
	removeFile("createFileError+48Fich10");
	removeFile("createFileError+48Fich11");
	removeFile("createFileError+48Fich12");
	removeFile("createFileError+48Fich13");
	removeFile("createFileError+48Fich14");
	removeFile("createFileError+48Fich15");
	removeFile("createFileError+48Fich16");
	removeFile("createFileError+48Fich17");
	removeFile("createFileError+48Fich18");
	removeFile("createFileError+48Fich19");
	removeFile("createFileError+48Fich20");
	removeFile("createFileError+48Fich21");
	removeFile("createFileError+48Fich22");
	removeFile("createFileError+48Fich23");
	removeFile("createFileError+48Fich24");
	removeFile("createFileError+48Fich25");
	removeFile("createFileError+48Fich26");
	removeFile("createFileError+48Fich27");
	removeFile("createFileError+48Fich28");
	removeFile("createFileError+48Fich29");
	removeFile("createFileError+48Fich30");
	removeFile("createFileError+48Fich31");
	removeFile("createFileError+48Fich32");
	removeFile("createFileError+48Fich33");
	removeFile("createFileError+48Fich34");
	removeFile("createFileError+48Fich35");
	removeFile("createFileError+48Fich36");
	removeFile("createFileError+48Fich37");
	removeFile("createFileError+48Fich38");
	removeFile("createFileError+48Fich39");
	removeFile("createFileError+48Fich40");
	removeFile("createFileError+48Fich41");
	removeFile("createFileError+48Fich42");
	removeFile("createFileError+48Fich43");
	removeFile("createFileError+48Fich44");
	removeFile("createFileError+48Fich45");
	removeFile("createFileError+48Fich46");
	removeFile("createFileError+48Fich47");
	removeFile("createFileError+48Fich48");
	removeFile("createFileError+48Fich49");

	///////

	createFile("createFileErrorRepetido");
	ret = createFile("createFileErrorRepetido");
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFileErrorRepetido ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFileErrorRepetido ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);


	///////

	ret = removeFile("removeFileNoExiste");
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFileNoExiste ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFileNoExiste ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	unmountFS();




	/*PRUEBA openFileError y closeFileError*/
	printf("\nPRUEBA openFileError y closeFileError\n");
	mkFS(DEV_SIZE);
	mountFS();
	ret = openFile("openFileErrorInexistente");
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileErrorInexistente ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileErrorInexistente ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	createFile("openFileErrorAbierto");
	openFile("openFileErrorAbierto");
	ret = openFile("openFileErrorAbierto");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileErrorAbierto ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileErrorAbierto ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	createFile("openFileErrorFicheroIntegridad");
	includeIntegrity("openFileErrorFicheroIntegridad");
	ret = openFile("openFileErrorFicheroIntegridad");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileErrorFicheroIntegridad ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileErrorFicheroIntegridad ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	createFile("openFileErrorFich");
	createLn("openFileErrorFich", "openFileErrorEnlaceSinFich");
	removeFile("openFileErrorFich");
	ret = openFile("openFileErrorEnlaceSinFich");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileErrorEnlaceSinFich ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileErrorEnlaceSinFich ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	removeLn("openFileErrorEnlaceSinFich");

	//////

	ret = closeFile(5000);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileErrorInexistente ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileErrorInexistente ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	createFile("closeFileError");
	descriptor = openFile("closeFileError");
	closeFile(descriptor);
	ret = closeFile(descriptor);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileErrorCerrado ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileErrorCerrado ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	includeIntegrity("closeFileError");
	descriptor = openFileIntegrity("closeFileError");
	ret = closeFile(descriptor);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileErrorFicheroIntegridad ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileErrorFicheroIntegridad ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	removeFile("closeFileError");
	unmountFS();






	/*PRUEBA readFileError y writeFileError*/
	printf("\nPRUEBA readFileError y writeFileError\n");
	mkFS(DEV_SIZE);
	mountFS();

	ret = readFile(5000, c, 5);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFileErrorInexistente ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFileErrorInexistente ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	/////////

	createFile("readFileErrorCerrado");
	descriptor = openFile("readFileErrorCerrado");
	char *buffer3 = "12345";
	writeFile(descriptor, buffer3, strlen(buffer3));
	closeFile(descriptor);
	ret = readFile(descriptor, c, 5);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFileErrorCerrado ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFileErrorCerrado ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	/////////

	char *buffer4 = "12345";
	ret = writeFile(descriptor, buffer4, strlen(buffer4));
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFileErrorCerrado ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFileErrorCerrado ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	unmountFS();




	/*PRUEBA lseekFileError*/
	printf("\nPRUEBA lseekFileError\n");
	mkFS(DEV_SIZE);
	mountFS();

	createFile("lseekFileErrorAtras");
	descriptor = openFile("lseekFileErrorAtras");
	lseekFile(descriptor, 0, FS_SEEK_BEGIN);
	ret = lseekFile(descriptor, -2, FS_SEEK_CUR);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFileErrorAtras ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFileErrorAtras ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	lseekFile(descriptor, 0, FS_SEEK_END);
	ret = lseekFile(descriptor, 4, FS_SEEK_CUR);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFileErrorDelante ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFileErrorDelante ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	closeFile(descriptor);

	///////

	ret = lseekFile(5000, 0, FS_SEEK_END);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFileErrorCerrado ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFileErrorCerrado ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	unmountFS();




	/*PRUEBA checkFileError e includeIntegrityError*/
	printf("\nPRUEBA checkFileError e includeIntegrityError\n");
	mkFS(DEV_SIZE);
	mountFS();

	ret = checkFile("checkFileErrorInexistente");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFileErrorInexistente ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFileErrorInexistente ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	createFile("checkFileErrorFichAbierto");
	includeIntegrity("checkFileErrorFichAbierto");
	openFileIntegrity("checkFileErrorFichAbierto");
	ret = checkFile("checkFileErrorFichAbierto");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFileErrorFichAbierto ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFileErrorFichAbierto ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	createFile("checkFileErrorFichSinIntegridad");
	ret = checkFile("checkFileErrorFichSinIntegridad");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFileErrorFichSinIntegridad ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFileErrorFichSinIntegridad ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	removeFile("checkFileErrorFichSinIntegridad");

	///////

	ret = includeIntegrity("includeIntegrityErrorInexistente");
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST includeIntegrityErrorInexistente ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST includeIntegrityErrorInexistente ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	createFile("includeIntegrityErrorIntegridad");
	includeIntegrity("includeIntegrityErrorIntegridad");
	ret = includeIntegrity("includeIntegrityErrorIntegridad");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST includeIntegrityErrorIntegridad ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST includeIntegrityErrorIntegridad ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);







	/*PRUEBA openFileIntegrityError y closeFileIntegrityError*/
	printf("\nPRUEBA openFileIntegrityError y closeFileIntegrityError\n");
	mkFS(DEV_SIZE);
	mountFS();
	ret = openFileIntegrity("openFileIntegrityErrorInexistente");
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileIntegrityErrorInexistente ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileIntegrityErrorInexistente ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	createFile("openFileIntegrityErrorAbierto");
	openFileIntegrity("openFileIntegrityErrorAbierto");
	ret = openFileIntegrity("openFileIntegrityErrorAbierto");
	if (ret != -3)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileIntegrityErrorAbierto ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileIntegrityErrorAbierto ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	createFile("openFileIntegrityErrorFichNoInt");
	ret = openFileIntegrity("openFileIntegrityErrorFichNoInt");
	if (ret != -3)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileIntegrityErrorFichNoInt ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileIntegrityErrorFichNoInt ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	createFile("openFileIntegrityErrorFich");
	createLn("openFileIntegrityErrorFich", "openFileIntegrityErrorLnNoFich");
	removeFile("openFileIntegrityErrorFich");
	ret = openFileIntegrity("openFileIntegrityErrorLnNoFich");
	if (ret != -3)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileIntegrityErrorLnNoFich ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFileIntegrityErrorLnNoFich ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	removeLn("openFileIntegrityErrorLnNoFich");

	//////

	ret = closeFileIntegrity(5000);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileIntegrityErrorInexistente ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileIntegrityErrorInexistente ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	createFile("closeFileIntegrityError");
	includeIntegrity("closeFileIntegrityError");
	descriptor = openFileIntegrity("closeFileIntegrityError");
	closeFileIntegrity(descriptor);
	ret = closeFileIntegrity(descriptor);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileIntegrityErrorCerrado ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileIntegrityErrorCerrado ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	createFile("closeFileIntegrityErrorNoInte");
	descriptor = openFile("closeFileIntegrityErrorNoInte");
	ret = closeFileIntegrity(descriptor);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileIntegrityErrorFichNoIntegridad ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFileIntegrityErrorFichNoIntegridad ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	removeFile("closeFileIntegrityError");
	unmountFS();





	/*PRUEBA createLnError y removeLnError*/
	printf("\nPRUEBA createLnError y removeLnError\n");
	mkFS(DEV_SIZE);
	mountFS();

	createFile("createLnErrorNombre+32");
	ret = createLn("createLnErrorNombre", "pruebaConUnNombreDeMasDeTreintaydosCaracteres");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLnErrorNombre+32 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLnErrorNombre+32 ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	ret = createLn("createLFileErrorFichNoExiste", "createLnErrorFichNoExiste");
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLnErrorFichNoExiste ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLnErrorFichNoExiste ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	createFile("createLnErrorLinkYaExiste");
	createLn("createLnErrorNombre", "createLnErrorLinkYaExiste");
	ret = createLn("createLnErrorLinkYaExiste", "createLnErrorLinkYaExiste");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLnErrorLinkYaExiste ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLnErrorLinkYaExiste ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	createFile("createFileErrorLinkToLink");
	createLn("createFileErrorLinkToLink", "createLnErrorLinkToLink");
	ret = createLn("createLnErrorLinkToLink", "createFileErrorLinkToLink2");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLnErrorLinkToLink ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLnErrorLinkToLink ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//////

	ret = removeLn("createFileErrorLinkToLink2");
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLnErrorLinkToLink ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createLnErrorLinkToLink ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	unmountFS();

	return 0;
}
