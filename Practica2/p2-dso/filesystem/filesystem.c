
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





TipoSuperbloque superbloque;    //Declaramos el superbloque a usar
TipoInodo inodos;				//Declaramos los inodos a usar
int8_t montar = 0;             //indicamos si el dispositivo esta montado (0) o no (1)
TipoInodo_x inodos_x;  		//Declaramos los inodos_x con informacion adicional que no deben guardarse en disco

/********************************************************************************************/
// En las diapositivas (Tema 4: 155 - 162) tambien estan estas 2 funciones para leer metadatos
/********************************************************************************************/

int metadata_fromDiskToMemory ( void ) {
	char buffer[BLOCK_SIZE] ;

	//leo el superbloque, el cual contiene mapas de inodos y datos
	bread(DEVICE_IMAGE, 0, buffer);
	memmove(&(superbloque), buffer, sizeof(TipoSuperbloque));

	//leo todos los inodos a memoria, empiezan desde el bloque 1 (siendo el 0 el superbloque)
	int inodosBloque = MAX_FICHEROS / superbloque.numBloquesInodos;
	for (int i = 0; i < superbloque.numBloquesInodos; i++){

		bread(DEVICE_IMAGE,superbloque.primerInodo + i, buffer);
		memmove(&(inodos[i*inodosBloque]), buffer, inodosBloque*sizeof(TipoInodo));

	}
	
 return 0;
}

int metadata_fromMemoryToDisk ( void ) {
	char buffer[BLOCK_SIZE] ;

	//guardamos en disco la informacion del superbloque (incluye mapas de inodos y bloques de datos)
	memset(buffer, 0, BLOCK_SIZE) ;
    memmove(b, &(superbloque), sizeof(TipoSuperbloque)) ;
    bwrite(DEVICE_IMAGE, 0, buffer) ;

	//escribimos los inodos a disco
	int inodosBloque = MAX_FICHEROS / superbloque.numBloquesInodos;
	for (int i=0; i < superbloque.numBloquesInodos; i++){
		memset(buffer, 0, BLOCK_SIZE) ;
    	memmove(buffer, &(inodos[i*inodosBloque]), sizeof(TipoInodo) * inodosBloque) ;
    	bwrite(DEVICE_IMAGE, superbloque.primerInodo + i, buffer) ;
	}

 return 0;
}

/***********************/
// Aqui empieza el codigo
/***********************/

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */

int mkFS(long deviceSize)
{
	if(deviceSize < MIN_DISCO){
		printf("Tamaño del disco demasiado pequeño, el tamaño mínimo es %d", MIN_DISCO);
		return -1;
	}
	if(deviceSize > MAX_DISCO){
		printf("Tamaño del disco demasiado grande, el tamaño máximo es %d", MAX_DISCO);
		return -1;
	}

	char buffer[BLOCK_SIZE];

	superbloque.numMagico = 12345678; 		   	   // se utiliza para comprobar que se ha creado por nuestro mkfs
	superbloque.numInodos = MAX_FICHEROS;          // el numero de inodos equivale al maximo de ficheros
	superbloque.primerInodo = 1;
	superbloque.numBloquesInodos = 2;
	superbloque.numBloquesDatos = MAX_FICHEROS * MAX_FILE_SIZE / BLOCK_SIZE;
	superbloque.primerBloqueDatos = 1 + superbloque.numBloquesInodos;
	superbloque.tamDispositivo = deviceSize;

	for (int i=0; i<superbloque.numInodos; i++){
		superbloque.mapaInodos[i] = 0; // free
	}
	for (int i=0; i<superbloque.numBloquesDatos; i++){
		superbloque.mapaBloques[i] = 0; // free
	}
	for (int i=0; i<superbloque.numInodos; i++){
		memset(&(inodos[i]), 0, sizeof(TipoInodoDisco) );
	}
	
	// to write the default file system into disk
	metadata_fromMemoryToDisk();

	memset(buffer, 0, BLOCK_SIZE);
	for (int i = 0; i < superbloque.numBloquesDatos; i++){
		bwrite(DEVICE_IMAGE, superbloque.primerBloqueDatos + i, buffer));
	}

	return 0;

}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{
	if (montar == 1){
		return -1;
	}

	metadata_fromDiskToMemory();

	if(12345678 != superbloque.numMagico){
		return -1;
	}

	montar = 1;

	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{

	if (montar == 0){
		return -1;
	}

	for (int i = 0; i < superbloque.numInodos; i++){
		if (inodos_x[i].abierto == 1){
			return -1;
		}
	}

	metadata_fromMemoryToDisk() ;

	montar = 0;

	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *nombre)
{
	//para crear el fichero voy a pedir tanto un bloque libre (alloc) como un inodo (ialloc), por lo que declaro 2 id's
	int id_inodo ;

	id_inodo = namei(nombre);   //Compruebo que no exista ya un fichero con ese nombre usando namei()
	if(id_inodo >= 0){			//Dara error si namei devuelve un valor mayor o igual a 0 (id del inodo cuyo fichero coincide en nombre)
		return -1;
	}

	//Pido un inodo libre con ialloc()
	id_inodo = ialloc() ;
	if (id_inodo < 0) { 		//Si ialloc devuelve un valor menor a 0 (ha devuelto -1 puesto que no hay libres) devuelvo -1 el id (-1)
		return id_inodo ;
		 }

	/*En este punto podríamos haber llamado a alloc y comprobar si hay un bloque libre y asociarlo al bloque directo del inodo
	No obstante, preferimos asignar el bloque cuando se vaya a escribir por primera vez en el, por lo que le asignaremos un valor 
	mayor al total de bloques que tenemos */

/*	id_bloque = alloc();
	if (id_bloque < 0){           *****EL OTRO CASO***
		ifree(id_inodo);
		return -2;
	}

	inodos[id_inodo].bloqueDirecto[0] = id_bloque ;

*/
	inodos[id_inodo].tipo = T_FICHERO ; 		// es de tipo fichero
	strcpy(inodos[id_inodo].nombre, nombre);    // asigno el nombre al inodo
	inodos[id_inodo].bloqueDirecto[0] = 255 ;   // asigno un bloque directo no valido por el momento
	inodos_x[id_inodo].posicion = 0;			// establezco el puntero a 0
	inodos_x[id_inodo].abierto = 1;				// marco el inodo como abierto
	return id_inodo;

}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *nombre)
{
	int id_inodo ;

	id_inodo = namei(nombre);   //Compruebo que exista ya un fichero con ese nombre usando namei()
	if(id_inodo < 0){			//Dara error si namei devuelve un valor menor a 0, puesto que para borrar un fichero debe existir
		return -1;
	}

	free(inodos[id_inodo].bloqueDirecto[0]);         		 // Libero el primer bloque directo
	memset( &(inodos[id_inodo]), 0, sizeof(TipoInodo) ) ;	 // pongo todos los valores del inodo a 0
	ifree(id_inodo) ;										 // Libero el inodo		
	
	return 0; 

}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName)
{
	/*int inodo_id ;

	// Ontener inodo a partir del nombre.
	inodo_id = namei(nombre) ;

	if (inodo_id < 0)
		return inodo_id ;

	// Enlace blando, creo que va aquí, es lo que puso el profe (fotos).
	if (inodo[inodo_id].type == enlace_simbolico) {
		// obtener "enlace_bando -> nombre fichero apunto"
		inodo_id = namei (name);
		// Para detectar bucles en enlaces simbolicos.
		if (inodo[inodo_id2].type == enlace_simbolico) {
			return -1;
		}
		return openFile (nombre del fichero apuntado);
	}

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
