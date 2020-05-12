
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

	/*PRUEBA GENERAL*/
	printf("PRUEBA GENERAL\n");
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


	return 0;
}
