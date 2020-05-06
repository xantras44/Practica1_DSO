
/*
 *
 * Operating System Design / Diseño de Sistemas Operativos
 * (c) ARCOS.INF.UC3M.ES
 *
 * @file 	filesystem.c
 * @brief 	Implementation of the core file system funcionalities and auxiliary functions.
 * @date	Last revision 01/04/2020
 *
 */


#include "filesystem/filesystem.h" // Headers for the core functionality
#include "filesystem/auxiliary.h"  // Headers for auxiliary functions
#include "filesystem/metadata.h"   // Type and structure declaration of the file system



/********************************************************************************************/
// En las diapositivas (Tema 4: 155 - 162) tambien estan estas 2 funciones para leer metadatos
/********************************************************************************************/

/*int metadata_fromDiskToMemory ( void ) {
 // leer bloque 0 de disco en sbloques[0]
 bread(DISK, 1, &(sbloques[0]) );
 // leer los bloques para el mapa de i-nodos
 for (int=0; i<sbloques[0].numBloquesMapaInodos; i++)
 	bread(DISK, 2+i, ((char *)imap + i*BLOCK_SIZE) ;
 // leer los bloques para el mapa de bloques de datos
 for (int=0; i<sbloques[0].numBloquesMapaDatos; i++)
 	bread(DISK, 2+i+sbloques[0].numBloquesMapaInodos, ((char *)dbmap + i*BLOCK_SIZE);
 // leer los i-nodos a memoria (en este diseño 1 inodo -> 1 bloque)
 for (int=0; i<(sbloques[0].numInodos*sizeof(TipoInodoDisco)/BLOCK_SIZE); i++)
 	bread(DISK, i+sbloques[0].primerInodo, ((char *)inodos + i*BLOCK_SIZE);
 return 1;
}*/

/*int metadata_fromMemoryToDisk ( void ) {
 // escribir bloque 0 de sbloques[0] a disco
 bwrite(DISK, 1, &(sbloques[0]) );
 // escribir los bloques para el mapa de i-nodos
 for (int=0; i<sbloques[0].numBloquesMapaInodos; i++)
 	bwrite(DISK, 2+i, ((char *)imap + i*BLOCK_SIZE) ;
 // escribir los bloques para el mapa de bloques de datos
 for (int=0; i<sbloques[0].numBloquesMapaDatos; i++)
 	bwrite(DISK, 2+i+sbloques[0].numBloquesMapaInodos, ((char *)dbmap + i*BLOCK_SIZE);
 // escribir los i-nodos a disco (en este diseño 1 inodo -> 1 bloque)
 for (int=0; i<(sbloques[0].numInodos*sizeof(TipoInodoDisco)/BLOCK_SIZE); i++)
 	bwrite(DISK, i+sbloques[0].primerInodo, ((char *)inodos + i*BLOCK_SIZE);
 return 1;
}*/

/***********************/
// Aqui empieza el codigo
/***********************/

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */

int mkFS(long deviceSize)
{
	
	sbloques[0].numMagico = 12345; // ayuda a comprobar que se haya creado por nuestro mkfs
	sbloques[0].numInodos = numInodo;
	…
	for (int=0; i<sbloques[0].numInodos; i++)
		imap[i] = 0; // free
	for (int=0; i<sbloques[0].numBloquesDatos; i++)
		bmap[i] = 0; // free
	for (int=0; i<sbloques[0].numInodos; i++)
		memset(&(inodos[i]), 0, sizeof(TipoInodoDisco) );
	
	// to write the default file system into disk
	metadata_fromMemoryToDisk();
	return 1;*/

	return -1;
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{
	/*// si ya montado -> error
 	if (1 == montado)
 		return -1 ;

 	// sincronizar metadatos
 	metadata_fromDiskToMemory() ;

 	// montado
 	montado = 1 ;

 	return 1 ;*/

	return -1;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	/*// si hay algún fichero abierto -> error
	for (int i=0; i<sbloques[0].numInodos; i++) {
		if (1 == inodos_x[i].abierto)
			return -1 ;
	}

	// sincronizar metadatos
	metadata_fromMemoryToDisk() ;

	// desmontado
	montado = 0 ;

	return 1 ;*/

	return -1;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName)
{
	/*int b_id, inodo_id ;
	inodo_id = ialloc() ;
	if (inodo_id < 0) { return inodo_id ; }

	// para este diseño: preallocation
	b_id = alloc();
	if (b_id < 0) { ifree(inodo_id); return b_id ; }

	inodos[inodo_id].tipo = 1 ; // FICHERO
	strcpy(inodos[inodo_id].nombre, nombre);
	inodos[inodo_id].bloqueDirecto = b_id ;
	inodos_x[inodo_id].posicion = 0;
	inodos_x[inodo_id].abierto = 1;
	return inodo_id;*/

	return -2;
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *fileName)
{
	/*int inodo_id ;
	inodo_id = namei(nombre) ;
	if (inodo_id < 0)
		return inodo_id ;
	free(inodos[inodo_id].bloqueDirecto);
	memset( &(inodos[inodo_id]), 0, sizeof(TipoInodoDisco) ) ;
	ifree(inodo_id) ;
	return 1; */

	return -2;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName)
{
	/*int inodo_id ;
	inodo_id = namei(nombre) ;
	if (inodo_id < 0)
		return inodo_id ;
	inodos_x[inodo_id].posicion = 0;
	inodos_x[inodo_id].abierto = 1;
	return inodo_id; */

	return -2;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{
	/*if (fd < 0)
		return fd ;
	inodos_x[fd].posicion = 0;
	inodos_x[fd].abierto = 0;
	return 1;*/

	return -1;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{
	/*char b[BLOCK_SIZE] ;
	int b_id ;
	if (inodos_x[fd].posicion+size > inodos[fd].size)
		size = inodos[fd].size - inodos_x[fd].posicion;
	if (size <= 0)
		return 0;
	b_id = bmap(fd, inodos_x[fd].posicion);
	bread(DISK, sbloques[0].primerBloqueDatos+b_id, b);

	// si (1 bloque) entonces [..].posicion => desplazamiento en bloque
	memmove(buffer, b+inodos_x[fd].posicion, size);

	inodos_x[fd].posicion += size;
	return size;*/

	return -1;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	/*char b[BLOCK_SIZE] ;
	int b_id ;
	if (inodos_x[fd].posicion+size > BLOCK_SIZE)
		size = BLOCK_SIZE - inodos_x[fd].posicion;
	if (size =< 0)
		return 0;
	b_id = bmap(fd, inodos_x[fd].posicion);
	bread(DISK, sbloques[0].primerBloqueDatos+b_id, b);
	memmove(b+inodos_x[fd].posicion, buffer, size);
	bwrite(DISK, sbloques[0].primerBloqueDatos+b_id, b);
	inodos_x[fd].posicion += size;
	inodos[fd].size += size;
	return size;*/

	return -1;
}

/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
	return -1;
}

/*
 * @brief	Checks the integrity of the file.
 * @return	0 if success, -1 if the file is corrupted, -2 in case of error.
 */

int checkFile (char * fileName)
{
    return -2;
}

/*
 * @brief	Include integrity on a file.
 * @return	0 if success, -1 if the file does not exists, -2 in case of error.
 */

int includeIntegrity (char * fileName)
{
    return -2;
}

/*
 * @brief	Opens an existing file and checks its integrity
 * @return	The file descriptor if possible, -1 if file does not exist, -2 if the file is corrupted, -3 in case of error
 */
int openFileIntegrity(char *fileName)
{

    return -2;
}

/*
 * @brief	Closes a file and updates its integrity.
 * @return	0 if success, -1 otherwise.
 */
int closeFileIntegrity(int fileDescriptor)
{
    return -1;
}

/*
 * @brief	Creates a symbolic link to an existing file in the file system.
 * @return	0 if success, -1 if file does not exist, -2 in case of error.
 */
int createLn(char *fileName, char *linkName)
{
    return -1;
}

/*
 * @brief 	Deletes an existing symbolic link
 * @return 	0 if the file is correct, -1 if the symbolic link does not exist, -2 in case of error.
 */
int removeLn(char *linkName)
{
    return -2;
}
