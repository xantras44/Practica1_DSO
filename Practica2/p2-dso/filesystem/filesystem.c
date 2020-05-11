
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
/*TipoSuperbloque superbloque;    //Declaramos el superbloque a usar
TipoInodo inodos;				//Declaramos los inodos a usar
int8_t montar = 0;             //indicamos si el dispositivo esta montado (0) o no (1)*/
TipoInodo_x inodos_x;  		//Declaramos los inodos_x con informacion adicional que no deben guardarse en disco

/*INICIO FUNCIONES AUXILIARES*/

int ialloc ( void )
{
 // cogemos el primer i-nodo libre
 for (int i = 0; i < superbloque[0].numInodos; i++) {
    if (superbloque[0].mapaInodos[i] == 0) {
        superbloque[0].mapaInodos[i] = 1; // cambiamos el estado del inodo a ocupado


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
 for (int i =0; i<superbloque[0].numBloquesDatos; i++) {
    if (superbloque[0].mapaBloques[i] == 0) {
        superbloque[0].mapaBloques[i] = 1 ; // cambiamos el estado del bloque a ocupado.
        // le ponemos los valores por defecto al bloque (opcional).
        memset(b, 0, BLOCK_SIZE) ;
        bwrite(DEVICE_IMAGE, superbloque[0].primerBloqueDatos + i, b);
        return i ; // devolvemos el identificador del bloque.
    }
 }
 return -1;
}

int ifree ( int inodo_id )
{
    // comprobamos si el identificador del inodo es valido.
    if (inodo_id >= superbloque[0].numInodos)
    return -1;
    // liberamos el inodo
    superbloque[0].mapaInodos[inodo_id] = 0;
    return 0;
}

int bfree ( int block_id )
{
    // comprobamos si el identificador del bloque es valido.
    if (block_id >= superbloque[0].numBloquesDatos)
    return -1;
    // liberamos el bloque
    superbloque[0].mapaBloques[block_id] = 0;
    return 0;
}

int bmap ( int inodo_id, int offset )
{
	int bloqueLog = offset / BLOCK_SIZE;	//Calculo el bloque asociado
	if (bloqueLog > 4){
		return -1;						//Solo tenemos 5 bloques por fichero, de 0 a 4
	}

	return inodos[inodo_id].bloqueDirecto[bloqueLog];  //devuelvo el bloque obtenido
}

int namei ( char *fname )
{
 // buscamos el inodo cuyo nombre sea <fname>
 for (int i =0; i<superbloque[0].numInodos; i++) {
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

	//leo el superbloque[0], el cual contiene mapas de inodos y datos
	bread(DEVICE_IMAGE, 0, buffer);
	memmove(&(superbloque[0]), buffer, sizeof(TipoSuperbloque));

	//leo todos los inodos a memoria, empiezan desde el bloque 1 (siendo el 0 el superbloque[0])
	int inodosBloque = MAX_FICHEROS / superbloque[0].numBloquesInodos;
	for (int i = 0; i < superbloque[0].numBloquesInodos; i++){

		bread(DEVICE_IMAGE,superbloque[0].primerInodo + i, buffer);
		memmove(&(inodos[i*inodosBloque]), buffer, inodosBloque*sizeof(TipoInodo));

	}
	
 return 0;
}

int metadata_fromMemoryToDisk ( void ) {
	char buffer[BLOCK_SIZE] ;
	//guardamos en disco la informacion del superbloque[0] (incluye mapas de inodos y bloques de datos)
	memset(buffer, 0, BLOCK_SIZE) ;
    memmove(buffer, &(superbloque[0]), sizeof(TipoSuperbloque)) ;
    bwrite(DEVICE_IMAGE, 0, buffer) ;
	


	//escribimos los inodos a disco
	int inodosBloque = MAX_FICHEROS / superbloque[0].numBloquesInodos;
	for (int i=0; i < superbloque[0].numBloquesInodos; i++){
		memset(buffer, 0, BLOCK_SIZE) ;
    	memmove(buffer, &(inodos[i*inodosBloque]), sizeof(TipoInodo) * inodosBloque) ;
    	bwrite(DEVICE_IMAGE, superbloque[0].primerInodo + i, buffer) ;

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
	superbloque[0].numMagico = 12345678; 		   	   // se utiliza para comprobar que se ha creado por nuestro mkfs
	superbloque[0].numInodos = MAX_FICHEROS;          // el numero de inodos equivale al maximo de ficheros
	superbloque[0].primerInodo = 1;
	superbloque[0].numBloquesInodos = 2;
	superbloque[0].numBloquesDatos = MAX_FICHEROS * MAX_FILE_SIZE / BLOCK_SIZE;
	superbloque[0].primerBloqueDatos = 1 + superbloque[0].numBloquesInodos;
	superbloque[0].tamDispositivo = deviceSize;
	
	printf("%ld\n", sizeof(superbloque[0].numMagico));
	printf("%ld\n", sizeof(superbloque[0].numInodos));
	printf("%ld\n", sizeof(superbloque[0].primerInodo));
	printf("%ld\n", sizeof(superbloque[0].numBloquesInodos));
	printf("%ld\n", sizeof(superbloque[0].numBloquesDatos));
	printf("%ld\n", sizeof(superbloque[0].primerBloqueDatos));
	printf("%ld\n", sizeof(superbloque[0].tamDispositivo));
	printf("%ld\n", sizeof(superbloque[0].mapaBloques));
	printf("%ld\n", sizeof(superbloque[0].mapaInodos));
	printf("%ld\n", sizeof(superbloque[0].relleno));
	printf("%ld\n", sizeof(superbloque[0]));

	printf("%ld\n", sizeof(inodos[0].tipo));
	printf("%ld\n", sizeof(inodos[0].nombre));
	printf("%ld\n", sizeof(inodos[0].nombreEnlace));
	printf("%ld\n", sizeof(inodos[0].tamano));
	printf("%ld\n", sizeof(inodos[0].bloqueDirecto));
	printf("%ld\n", sizeof(inodos[0]));

	for (int i=0; i<superbloque[0].numInodos; i++){
		memset(&(superbloque[0].mapaInodos[i]), 0, sizeof(superbloque[0].mapaInodos[i]) );
		//superbloque[0].mapaInodos[i] = 0; // free
	}
	for (int i=0; i<superbloque[0].numBloquesDatos; i++){
		memset(&(superbloque[0].mapaBloques[i]), 0, sizeof(superbloque[0].mapaBloques[i]) );
		//superbloque[0].mapaBloques[i] = 0; // free
	}
	for (int i=0; i<superbloque[0].numInodos; i++){
		memset(&(inodos[i]), 0, sizeof(TipoInodo) );
	}
	
	// to write the default file system into disk
	metadata_fromMemoryToDisk();

	memset(buffer, 0, BLOCK_SIZE);
	for (int i = 0; i < superbloque[0].numBloquesDatos; i++){
		bwrite(DEVICE_IMAGE, superbloque[0].primerBloqueDatos + i, buffer);
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

	if(12345678 != superbloque[0].numMagico){
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

	for (int i = 0; i < superbloque[0].numInodos; i++){
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



	inodos[id_inodo].tipo = T_FICHERO ; 				// es de tipo fichero
	strcpy(inodos[id_inodo].nombre, nombre);    		// asigno el nombre al inodo
	inodos[id_inodo].bloqueDirecto[0] = id_bloque ;   	// asigno el primer bloque directo


	for (int i = 1; i < MAX_FILE_SIZE / BLOCK_SIZE; i++){  /* El resto de bloques directos (habra tantos como tamano fichero entre 
															tamano bloque) los asigno a un valor no valido por el momento*/
		inodos[id_inodo].bloqueDirecto[i] = 255;

	}

	inodos_x[id_inodo].posicion = 0;					// establezco el puntero a 0
	inodos_x[id_inodo].abierto = 0;						// marco el inodo como abierto

	return 0;

}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *nombre)
{
	int id_inodo ;

	char c [BLOCK_SIZE];

	id_inodo = namei(nombre);   //Compruebo que exista ya un fichero con ese nombre usando namei()
	if(id_inodo < 0){			//Dara error si namei devuelve un valor menor a 0, puesto que para borrar un fichero debe existir
		return -1;
	}

	for (int i = 0; i < MAX_FILE_SIZE / BLOCK_SIZE; i++){
		bfree(inodos[id_inodo].bloqueDirecto[i]);         		// Libero los bloques directos asociados
	}
	
	memset(c, 0, BLOCK_SIZE);
	for (int i = 0; i < MAX_FILE_SIZE / BLOCK_SIZE; i++){			// pongo los valores del bloque a 0
		int desplazar = inodos[id_inodo].bloqueDirecto[i];
		bwrite(DEVICE_IMAGE, superbloque[0].primerBloqueDatos + desplazar, c);
	}

	memset( &(inodos[id_inodo]), 0, sizeof(TipoInodo) ) ;	 	// pongo todos los valores del inodo a 0
	
	ifree(id_inodo) ;										 	// Libero el inodo		
	metadata_fromMemoryToDisk() ;				//guardo todas las modificaciones a disco
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
	if (descriptor < 0 || (descriptor >= superbloque[0].numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}

	if (inodos_x[descriptor].abierto == 0){
		return -1;									//Si ya está cerrado no lo vuelvo a cerrar
	}

	inodos_x[descriptor].abierto = 0;
	metadata_fromMemoryToDisk() ;				//Requisito F3
	return 0;

}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int descriptor, void *buffer, int size)
{

	int id_bloque ;

	if (descriptor < 0 || (descriptor >= superbloque[0].numInodos)){
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
	while((inodos_x[descriptor].posicion + size) >= (actual + 1) * BLOCK_SIZE){
		
		id_bloque = bmap(descriptor, inodos_x[descriptor].posicion);          //LLamamos a bmap para obtener el id del bloque correspondiente

		if(id_bloque < 0){
			return -1;         //Si el id del bloque no es valido devuelve error
		}

		char b[BLOCK_SIZE] ;
		
		int posActual =	inodos_x[descriptor].posicion % BLOCK_SIZE;  // calcula la posicion con respecto al tamano del bloque 
		
		int leer = (actual + 1) * BLOCK_SIZE - inodos_x[descriptor].posicion;

		bread(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+id_bloque, b);     // lee el bloque y lo guarda en el buffer

		memmove(buffer + leido, b+posActual, leer); 	 /* Guarda la informacion en el buffer proporcionado usando lo almacenado en el 
													buffer b, como este es de tamano BLOCK SIZE y el puntero va hasta el tamano
													del fichero uso posActual, que es la posicion del puntero relativa al tamano bloque*/

		inodos_x[descriptor].posicion += leer;		// Ajusta el puntero de posicion										// guarda en el buffer a devolver la informacion que quiere

		size -= leer;							// ajusta size por si queda algo que leer de otro bloque
		actual += 1;							// ajusta el bloque actual
		leido += leer;							// encargado de mantener cuanto se ha leido

		
	}

		id_bloque = bmap(descriptor, inodos_x[descriptor].posicion);          //LLamamos a bmap para obtener el id del bloque correspondiente

		if(id_bloque < 0){
			return -1;         //Si el id del bloque no es valido devuelve error
		}

		char b[BLOCK_SIZE] ;

		int posActual =	inodos_x[descriptor].posicion % BLOCK_SIZE;  // calcula la posicion con respecto al tamano del bloque 

		bread(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+id_bloque, b);

		memmove(buffer + leido, b+posActual, size);			/* Guarda la informacion en el buffer proporcionado usando lo almacenado en el 
															buffer b, como este es de tamano BLOCK SIZE y el puntero va hasta el tamano
															del fichero uso posActual, que es la posicion del puntero relativa al tamano bloque*/

		inodos_x[descriptor].posicion += size;						 // Ajusta el puntero de posicion

		leido += size;												// encargado de mantener cuanto se ha leido
	return leido;		//Devuelve el numero de bytes leidos

}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int descriptor, void *buffer, int size)
{
	int id_bloque ;
	if (descriptor < 0 || (descriptor >= superbloque[0].numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}
	if (inodos_x[descriptor].posicion+size > MAX_FILE_SIZE-1){   /***********QUIZA ES TAMANO FICHERO NO BLOQUE************/
		size = MAX_FILE_SIZE-1 - inodos_x[descriptor].posicion;  //Si el tamano pedido es mayor que lo que queda devuelvo lo que queda por leer
	}
	if (size <= 0){
		return 0;			// Devuelvo 0 si no se pide leer nada
	}
 
	int actual = inodos_x[descriptor].posicion / BLOCK_SIZE;
	int escrito = 0;

	while((inodos_x[descriptor].posicion + size) >= (actual + 1) * BLOCK_SIZE){
		
		id_bloque = bmap(descriptor, inodos_x[descriptor].posicion);    // busco el bloque con bmap
		if(id_bloque == 255){
			id_bloque = alloc();          //Si el bloque estaba reservado para el inodo pido un bloque vacio
			if (id_bloque < 0){
				return -1;
			}
			int cambiado = 0;
			int contador = 0;
			int i = 0;
			while (contador < 5 && cambiado == 0){ 
															
				if(inodos[descriptor].bloqueDirecto[i] == 255){

					inodos[descriptor].bloqueDirecto[i] = id_bloque;    /*Si hay un bloque disponible se lo asigno al inodo en su
																		posicion correspondiente*/
					cambiado = 1;
				}

				i += 1;
			}

			if (cambiado != 1){
				return -1;
			}
		
		}

		if(id_bloque < 0){
			return -1;
		}

		char b[BLOCK_SIZE] ;

		int posActual =	inodos_x[descriptor].posicion % BLOCK_SIZE;  // calcula la posicion con respecto al tamano del bloque 
		
		int escribir = (actual + 1) * BLOCK_SIZE - inodos_x[descriptor].posicion;
			
		bread(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+id_bloque, b);  //leo el bloque de disco

		memmove(b+posActual, buffer + escrito, escribir);           //Modifico los datos con la informacion proporcionada
		bwrite(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+id_bloque, b);  //vuelvo a escribir el bloque a disco

		inodos[descriptor].tamano += escribir;          //actualizo el tamano actual del fichero

		inodos_x[descriptor].posicion += escribir;		// Ajusta el puntero de posicion									

		size -= escribir;						// ajusta size por si queda algo que escribir de otro bloque
		actual += 1;							// ajusta el bloque actual
		escrito += escribir;					// encargado de mantener cuanto se ha escrito
		
	}

		id_bloque = bmap(descriptor, inodos_x[descriptor].posicion);    // busco el bloque con bmap
		if(id_bloque == 255){
			id_bloque = alloc();          //Si el bloque estaba reservado para el inodo pido un bloque vacio
			if (id_bloque < 0){
				return -1;
			}
			for (int i = 1; i < MAX_FILE_SIZE / BLOCK_SIZE; i++){ 
															
				if(inodos[descriptor].bloqueDirecto[i] == 255){
					
					inodos[descriptor].bloqueDirecto[i] = id_bloque;    /*Si hay un bloque disponible se lo asigno al inodo en su
																		posicion correspondiente*/
				}
			}
		
		}

		if(id_bloque < 0){
			return -1;
		}

		char b[BLOCK_SIZE] ;

		int posActual =	inodos_x[descriptor].posicion % BLOCK_SIZE;  // calcula la posicion con respecto al tamano del bloque 
		
		bread(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+id_bloque, b);		//leo el bloque de disco

		memmove(b+posActual, buffer + escrito, size);           //Modifico los datos con la informacion proporcionada
			
		bwrite(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+id_bloque, b);  //vuelvo a escribir el bloque a disco

		inodos[descriptor].tamano += size;		//actualizo el tamano actual del fichero

		inodos_x[descriptor].posicion += size;		// Ajusta el puntero de posicion				

		escrito += size;					// encargado de mantener cuanto se ha escrito			
	
	
	return escrito;		//Devuelve el numero de bytes escritos

	
}

/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int descriptor, long offset, int whence)
{
	if (descriptor < 0 || (descriptor >= superbloque[0].numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}

	if(whence == FS_SEEK_BEGIN){
		inodos_x[descriptor].posicion = 0;		// Ajusta el puntero de posicion al inicio
		return 0;
	}

	if(whence == FS_SEEK_END){
		inodos_x[descriptor].posicion = inodos[descriptor].tamano;		// Ajusta el puntero de posicion al final del fichero
		return 0;
	}

	if(whence == FS_SEEK_CUR){
		if ((offset <= 0) && (inodos_x[descriptor].posicion - offset < 0)){
			return -1;			//Si queremos ir a una posicion menor que 0 da error
		}

		if ((offset > 0) && (inodos_x[descriptor].posicion + offset > inodos[descriptor].tamano)){
			return -1;			//Si queremos ir a una posicion mayor que el tamano actual del fichero
		}

		if (offset <= 0){
			inodos_x[descriptor].posicion -= offset;    //Si el offset es negativo vuelvo atras el puntero
			return 0;
		}

		if (offset > 0){
			inodos_x[descriptor].posicion += offset;    //Si el offset es positivo avanzo el puntero
			return 0;
		}
	}


	return -1;      //Si whence no vale ninguno de los valores propuestos


}

/*
 * @brief	Checks the integrity of the file.
 * @return	0 if success, -1 if the file is corrupted, -2 in case of error.
 */

int checkFile (char * fileName)
{
	// Comprobamos que existe un fichero con ese nombre.
	int id_inodo ;
	
	// Obtenemos el inodo asociado al nombre propuesto.
	id_inodo = namei(fileName) ;
	if (id_inodo < 0){
		return -2 ;     // Si no existe el fichero devuelvo -1 (id_inodo = -1 pues namei no lo encuentra)
	}

	uint32_t hashFichParam = inodos[id_inodo].integridad;
	// Calculamos la integridad total del fichero como la suma de las integriaddes de todos sus bloques.
	uint32_t hashFich = 0;
	uint32_t hash = 0;
	for (int i = 0; i < 5; i++) {
		char b [BLOCK_SIZE];
		if(inodos[id_inodo].bloqueDirecto[i] == 255){
			inodos[id_inodo].bloqueDirecto[i] = alloc();          //Si el bloque estaba reservado para el inodo pido un bloque vacio
			if (inodos[id_inodo].bloqueDirecto[i] < 0){
				return -1;
			}

		}
		int desplazar = inodos[id_inodo].bloqueDirecto[i];
		printf("desplazar %d\n", desplazar);
		bread(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+desplazar,b);
		hash = CRC32 ((const unsigned char *)b, sizeof(b));
		// uint32_t hash = CRC32 (inodos[id_inodo].bloqueDirecto[0], inodos[id_inodo].tamano);
		hashFich += hash;

		printf("hashfich check %lu\n", (unsigned long)hashFich);
		// Si se ha dado un error al calcular la integriad del bloque: Error.
		if (hash == 0) {
			return -2;
		}
	}
	printf("integridad fichero %d", hashFich);
	// Si las integridades coinciden, retornamos 0.
	if (hashFichParam == hashFich) {
		return 0;
	}
	// Sino, devolvemos -1;
	else {
		return -1;
	}
    return -2;
}

/*
 * @brief	Include integrity on a file.
 * @return	0 if success, -1 if the file does not exists, -2 in case of error.
 */

int includeIntegrity (char * fileName)
{
	int id_inodo ;
	
	// Obtenemos el inodo asociado al nombre propuesto.
	id_inodo = namei(fileName) ;
	if (id_inodo < 0){
		return id_inodo ;     // Si no existe el fichero devuelvo -1 (id_inodo = -1 pues namei no lo encuentra)
	}

	if (inodos[id_inodo].integridad != 0){
		return -2;
	}

	// Calculamos la integridad total del fichero como la suma de las integriaddes de todos sus bloques.
	uint32_t hashFich = 0;
	uint32_t hash = 0;
	for (int i = 0; i < 5; i++) {
		char b [BLOCK_SIZE];
		if(inodos[id_inodo].bloqueDirecto[i] == 255){
			inodos[id_inodo].bloqueDirecto[i] = alloc();          //Si el bloque estaba reservado para el inodo pido un bloque vacio
			if (inodos[id_inodo].bloqueDirecto[i] < 0){
				return -1;
			}

		}

		int posicion = inodos[id_inodo].bloqueDirecto[i];
		memmove(b, &posicion, 2048);
		hash = CRC32 ((const unsigned char *)b, sizeof(b));
		//hash = CRC32 (&(inodos[id_inodo].bloqueDirecto[i]), sizeof(inodos[id_inodo].bloqueDirecto[i]));
		//printf("bloque %u\n", (unsigned int)(&(inodos[id_inodo].bloqueDirecto[i])));
		// uint32_t hash = CRC32 (inodos[id_inodo].bloqueDirecto[0], inodos[id_inodo].tamano);
		hashFich += hash;
		printf("hashfich include %lu\n", (unsigned long)hashFich);
		// Si se ha dado un error al calcular la integriad del bloque: Error.
		if (hash == 0) {
			return -2;
		}
	}

	// Le añadimos la integridad al fichero.
	inodos[id_inodo].integridad = hashFich;
	printf("integridad inodo %d", inodos[id_inodo].integridad);
    return 0;
}

/*
 * @brief	Opens an existing file and checks its integrity
 * @return	The file descriptor if possible, -1 if file does not exist, -2 if the file is corrupted, -3 in case of error
 */
int openFileIntegrity(char *fileName)
{
	int id_inodo ;
	
	// Obtenemos el inodo asociado al nombre propuesto.
	id_inodo = namei(fileName) ;
	if (id_inodo < 0){
		return id_inodo ;     // Si no existe el fichero devuelvo -1 (id_inodo = -1 pues namei no lo encuentra)
	}


	// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	// En openFile se devuelve -2 pero aquí se devuelve -3 segun el enunciado.
	if (inodos_x[id_inodo].abierto == 1){
		return -3;								//Si ya esta abierto devuelvo -2 (error)
	}
	

	// Comprobamos si el fichero tiene integridad.
	// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	// Comprobar si se comprueba así.
	if (inodos[id_inodo].integridad == 0) {
		return -3;
	}
	
	// Si tiene integridad, comprobamos que no este corrupto.
	int integridad = checkFile(fileName);
	if (integridad == -1 || integridad == -2) {
		return -2;
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
 * @brief	Closes a file and updates its integrity.
 * @return	0 if success, -1 otherwise.
 */
int closeFileIntegrity(int fileDescriptor)
{
	if (fileDescriptor < 0 || (fileDescriptor >= superbloque[0].numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}
	
	if (inodos_x[fileDescriptor].abierto == 0){
		return -1;									//Si ya está cerrado no lo vuelvo a cerrar
	}

	// Calculamos la integridad total del fichero como la suma de las integriaddes de todos sus bloques.
	uint32_t hashFich = 0;
	uint32_t hash = 0;
	for (int i = 0; i < 5; i++) {
		hash = CRC32 (&(inodos[fileDescriptor].bloqueDirecto[i]), sizeof(inodos[fileDescriptor].bloqueDirecto[i]));
		// uint32_t hash = CRC32 (inodos[fileDescriptor].bloqueDirecto[0], inodos[fileDescriptor].tamano);
		hashFich += hash;
		// Si se ha dado un error al calcular la integriad del bloque: Error.
		if (hash == 0) {
			return -2;
		}
	}


	inodos_x[fileDescriptor].abierto = 0;
	metadata_fromMemoryToDisk() ;                //Requisito F3
	return 0;
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



