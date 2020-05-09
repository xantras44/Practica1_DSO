
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

#include <string.h>

//DECLARACIONES NECESARIAS
TipoSuperbloque superbloque;    //Declaramos el superbloque a usar
TipoInodo inodos;				//Declaramos los inodos a usar
int8_t montar = 0;             //indicamos si el dispositivo esta montado (0) o no (1)
TipoInodo_x inodos_x;  		//Declaramos los inodos_x con informacion adicional que no deben guardarse en disco

/*INICIO FUNCIONES AUXILIARES*/

int ialloc ( void )
{
 // cogemos el primer i-nodo libre
 for (int i = 0; i < superbloque.numInodos; i++) {
    if (superbloque.mapaInodos[i] == 0) {
        superbloque.mapaInodos[i] = 1; // cambiamos el estado del inodo a ocupado


        // le ponemos los valores por defecto al inodo AAAAAAAAAAAAAAAAAAAAAAAA
        memset(&(inodos[i]),0, sizeof(TipoInodo));


        return i; // devolvemos el identificador del inodo.
    }
 }
 return -1;
}

int alloc ( void )
{
 char b[BLOCK_SIZE];
 // cogemos el primer bloque libre
 for (int i =0; i<superbloque.numBloquesDatos; i++) {
    if (superbloque.mapaBloques[i] == 0) {
        superbloque.mapaBloques[i] = 1 ; // cambiamos el estado del bloque a ocupado.
        // le ponemos los valores por defecto al bloque (opcional).
        memset(b, 0, BLOCK_SIZE) ;
        bwrite(DEVICE_IMAGE, superbloque.primerBloqueDatos + i, b);
        return i ; // devolvemos el identificador del bloque.
    }
 }
 return -1;
}

int ifree ( int inodo_id )
{
    // comprobamos si el identificador del inodo es valido.
    if (inodo_id > superbloque.numInodos)
    return -1;
    // liberamos el inodo
    superbloque.mapaInodos[inodo_id] = 0;
    return -1;
}

int bfree ( int block_id )
{
    // comprobamos si el identificador del bloque es valido.
    if (block_id > superbloque.numBloquesDatos)
    return -1;
    // liberamos el bloque
    superbloque.mapaBloques[block_id] = 0;
    return -1;
}

int bmap ( int inodo_id, int offset )
{
	return 0;
    /*int b[BLOCK_SIZE/4] ;
    int bloque_logico ;
    if (inodo_id > superbloque.numInodos) {
        return -1;
    }
    bloque_logico = offset / BLOCK_SIZE;


    // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    // Por qué es 1+BLOCK_SIZE
    if (bloque_logico > (1+BLOCK_SIZE/4))
        return -1;



    // bloque de datos físico asociado: directo
    if (0 == bloque_logico)
        return inodos[0].bloqueDirecto[inodo_id];
    return b[bloque_logico - 1] ; // hay 1 bloque directo -> x-1*/
}

int namei ( char *fname )
{
 // buscamos el inodo cuyo nombre sea <fname>
 for (int i =0; i<superbloque.numInodos; i++) {
    if (! strcmp(inodos[i].nombre, fname))
        return i;
 }
 return -1;
}


/*FIN FUNCIONES AUXILIARES*/


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
    memmove(buffer, &(superbloque), sizeof(TipoSuperbloque)) ;
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


	//char buffer[BLOCK_SIZE];
	//printf("%ld", sizeof(superbloque));
	printf("%ld", sizeof(inodos[0]));
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
		memset(&(inodos[i]), 0, sizeof(TipoInodo) );
		printf("%d", inodos[i].tipo);
	}
	
	// to write the default file system into disk
	/*metadata_fromMemoryToDisk();

	memset(buffer, 0, BLOCK_SIZE);
	for (int i = 0; i < superbloque.numBloquesDatos; i++){
		bwrite(DEVICE_IMAGE, superbloque.primerBloqueDatos + i, buffer);
	}*/

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
	int id_inodo, id_bloque ;

	id_inodo = namei(nombre);   //Compruebo que no exista ya un fichero con ese nombre usando namei()
	if(id_inodo >= 0){			//Dara error si namei devuelve un valor mayor o igual a 0 (id del inodo cuyo fichero coincide en nombre)
		return -1;
	}

	//Pido un inodo libre con ialloc()
	id_inodo = ialloc() ;
	if (id_inodo < 0) { 		//Si ialloc devuelve un valor menor a 0 (ha devuelto -1 puesto que no hay libres) devuelvo -1 el id (-1)
		return id_inodo ;
		 }

	//Miramos que haya bloques libres con alloc()
	id_bloque = alloc();
	if (id_bloque < 0){           
		ifree(id_inodo);          		//Si no hay bloques libres libero el inodo que acabo de crear        
		return -2;
	}

	inodos[id_inodo].bloqueDirecto[0] = id_bloque ;


	inodos[id_inodo].tipo = T_FICHERO ; 				// es de tipo fichero
	strcpy(inodos[id_inodo].nombre, nombre);    		// asigno el nombre al inodo
	inodos[id_inodo].bloqueDirecto[0] = id_bloque ;   	// asigno el primer bloque directo

	for (int i = 1; i < MAX_FILE_SIZE / BLOCK_SIZE; i++){  /* El resto de bloques directos (habra tantos como tamano fichero entre 
															tamano bloque) los asigno a un valor no valido por el momento*/
		inodos[id_inodo].bloqueDirecto[i] = 255;
	}

	inodos_x[id_inodo].posicion = 0;					// establezco el puntero a 0
	inodos_x[id_inodo].abierto = 1;						// marco el inodo como abierto
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

	for (int i = 1; i < MAX_FILE_SIZE / BLOCK_SIZE; i++){
		bfree(inodos[id_inodo].bloqueDirecto[i]);         		// Libero los bloques directos asociados
	}

	memset( &(inodos[id_inodo]), 0, sizeof(TipoInodo) ) ;	 	// pongo todos los valores del inodo a 0
	ifree(id_inodo) ;										 	// Libero el inodo		
	
	return 0; 

}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *nombre)
{
	int id_inodo ;

	// Obtenemos el inodo asociado al nombre propuesto.
	id_inodo = namei(nombre) ;

	if (id_inodo < 0){
		return id_inodo ;     // Si no existe el fichero devuelvo -1 (id_inodo = -1 pues namei no lo encuentra)
	}

	if (inodos_x[id_inodo].abierto == 1){
		return -2;								//Si ya esta abierto devuelvo -2 (error)
	}
	// Compruebo si el inodo es de tipo enlace blando
	/*if (inodo[id_inodo].type == T_ENLACE) {
		// Para detectar bucles en enlaces simbolicos.       
		if (inodo[inodo_id2].type == T_ENLACE) {
			return -1;
		}
		return openFile (nombre del fichero apuntado);
	}*/

	inodos_x[id_inodo].posicion = 0;       //Establezco el puntero a 0 y marco como abierto
	inodos_x[id_inodo].abierto = 1;

	return id_inodo; 

}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int descriptor)
{
	if (descriptor < 0 || (descriptor >= superbloque.numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}


	inodos_x[descriptor].posicion = 0;       //Establezco el puntero a 0 y marco como cerrado
	inodos_x[descriptor].abierto = 0;

	return 0;

}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int descriptor, void *buffer, int size)
{
	char b[BLOCK_SIZE] ;
	char aux [BLOCK_SIZE];
	int id_bloque ;

	if (descriptor < 0 || (descriptor >= superbloque.numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}

	if (inodos_x[descriptor].posicion + size > inodos[descriptor].tamano){
		size = inodos[descriptor].tamano - inodos_x[descriptor].posicion;     //Si el tamano pedido es mayor que lo que queda devuelvo lo que queda por leer
	}

	if (size <= 0){
		return 0;         // Devuelvo 0 si no queda nada por leer
	}

	int actual = inodos_x[descriptor].posicion / BLOCK_SIZE;
	int leido = 0;
	while(size > 0){

		id_bloque = bmap(descriptor, inodos_x[descriptor].posicion);          //LLamamos a bmap para obtener el id del bloque correspondiente

		if(id_bloque < 0){
			return -1;         //Si el id del bloque no es valido devuelve error
		}

		if ((inodos_x[descriptor].posicion + size / BLOCK_SIZE) >= (actual + 1)){
			int leer = (actual + 1) * BLOCK_SIZE - inodos_x[descriptor].posicion;

			int posActual =	inodos_x[descriptor].posicion % BLOCK_SIZE;  // calcula la posicion con respecto al tamano del bloque 

			bread(DEVICE_IMAGE, superbloque.primerBloqueDatos+id_bloque, b);     // lee el bloque y lo guarda en el buffer

			memmove(aux, b+posActual, leer); 	 /* Guarda la informacion en el buffer proporcionado usando lo almacenado en el 
													buffer b, como este es de tamano BLOCK SIZE y el puntero va hasta el tamano
													del fichero uso posActual, que es la posicion del puntero relativa al tamano bloque*/

			inodos_x[descriptor].posicion += leer;						 // Ajusta el puntero de posicion

			strcat(buffer, aux);										// guarda en el buffer a devolver la informacion que quiere

			size -= leer;												// ajusta size por si queda algo que leer de otro bloque

			actual += 1;												// ajusta el bloque actual

			leido += leer;												// encargado de mantener cuanto se ha leido
		}

		bread(DEVICE_IMAGE, superbloque.primerBloqueDatos+id_bloque, b);

		int posActual =	inodos_x[descriptor].posicion % BLOCK_SIZE;        // calcula la posicion con respecto al tamano del bloque

		memmove(aux, b+posActual, size);			/* Guarda la informacion en el buffer proporcionado usando lo almacenado en el 
													buffer b, como este es de tamano BLOCK SIZE y el puntero va hasta el tamano
													del fichero uso posActual, que es la posicion del puntero relativa al tamano bloque*/

		inodos_x[descriptor].posicion += size;						 // Ajusta el puntero de posicion

		size -= size;												// ajusta size para acabar el bucle

		strcat(buffer, aux);										// guarda en el buffer a devolver la informacion que quiere

		leido += size;												// encargado de mantener cuanto se ha leido
	}
	
	return leido;		//Devuelve el numero de bytes leidos

}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int descriptor, void *buffer, int size)
{
	char b[BLOCK_SIZE] ;
	int id_bloque ;

	if (descriptor < 0 || (descriptor >= superbloque.numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}

	if (inodos_x[descriptor].posicion+size > BLOCK_SIZE){   /***********QUIZA ES TAMANO FICHERO NO BLOQUE************/
		size = BLOCK_SIZE - inodos_x[descriptor].posicion;  //Si el tamano pedido es mayor que lo que queda devuelvo lo que queda por leer
	}

	if (size <= 0){
		return 0;			// Devuelvo 0 si no queda hueco para escribir
	}
 
	id_bloque = bmap(descriptor, inodos_x[descriptor].posicion);    // busco el bloque con bmap
	if(id_bloque == 255){
		id_bloque = alloc();                  	//Si el bloque estaba reservado para el inodo pido un bloque vacio
		if (id_bloque < 0){
			return -1;
		}
		for (int i = 1; i < MAX_FILE_SIZE / BLOCK_SIZE; i++){ 
															
			if(inodos[descriptor].bloqueDirecto[i] == 255){
				inodos[descriptor].bloqueDirecto[i] = id_bloque;        /*Si hay un bloque disponible se lo asigno al inodo en su
																		posicion correspondiente*/
			}
	}
		
	}

	bread(DEVICE_IMAGE, superbloque.primerBloqueDatos+id_bloque, b);
	memmove(b+inodos_x[descriptor].posicion, buffer, size);           //Modifico los datos con la informacion proporcionada
	bwrite(DEVICE_IMAGE, superbloque.primerBloqueDatos+id_bloque, b);

	inodos_x[descriptor].posicion += size;							  // ajusto el puntero
	inodos[descriptor].tamano += size;								  // aumento el tamano de este fichero

	return size;						//Devuelve el numero de bytes escritos

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



