
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

TipoInodo_x inodos_x;  		//Declaramos los inodos_x con informacion adicional que no deben guardarse en disco

/*INICIO FUNCIONES AUXILIARES*/

//Funcion encargada de devolver un inodo libre
int ialloc ( void )
{
 // cogemos el primer i-nodo libre
 for (int i = 0; i < superbloque[0].numInodos; i++) {
    if (superbloque[0].mapaInodos[i] == 0) {
        superbloque[0].mapaInodos[i] = 1; // cambiamos el estado del inodo a ocupado


        // le ponemos los valores por defecto al inodo 
        memset(&(inodos[i]),0, sizeof(TipoInodo));


        return i; // devolvemos el identificador del inodo.
    }
 }
 return -1;
}

//Funcion encargada de devolver un bloque libre
int alloc ( void )
{
 char b[BLOCK_SIZE];
 // cogemos el primer bloque libre
 for (int i =0; i<superbloque[0].numBloquesDatos; i++) {
    if (superbloque[0].mapaBloques[i] == 0) {
        superbloque[0].mapaBloques[i] = 1 ; // cambiamos el estado del bloque a ocupado.
        memset(b, 0, BLOCK_SIZE) ;
        bwrite(DEVICE_IMAGE, superbloque[0].primerBloqueDatos + i, b); //ponemos los valores a 0
        return i ; // devolvemos el identificador del bloque.
    }
 }
 return -1;
}

//Funcion encargada de liberar un inodo
int ifree ( int inodo_id )
{
    // comprobamos si el identificador del inodo es valido.
    if (inodo_id >= superbloque[0].numInodos)
    return -1;
    // liberamos el inodo
    superbloque[0].mapaInodos[inodo_id] = 0;
    return 0;
}

//Funcion encargada de liberar un bloque
int bfree ( int block_id )
{
    // comprobamos si el identificador del bloque es valido.
    if (block_id >= superbloque[0].numBloquesDatos)
    return -1;
    // liberamos el bloque
    superbloque[0].mapaBloques[block_id] = 0;
    return 0;
}

//Funcion encargada de devolver el bloque asociado al id y offset recibido
int bmap ( int inodo_id, int offset )
{
	int bloqueLog = offset / BLOCK_SIZE;	//Calculo el bloque asociado
	if (bloqueLog > 4){
		return -1;						//Solo tenemos 5 bloques por fichero, de 0 a 4
	}

	return inodos[inodo_id].bloqueDirecto[bloqueLog];  //devuelvo el bloque obtenido
}

//Función encargada de comprobar si existe un inodo con nombre fname en el sistema
int namei ( char *fname )
{
 // buscamos el inodo cuyo nombre sea <fname>
 for (int i =0; i<superbloque[0].numInodos; i++) {
    if (! strcmp(inodos[i].nombre, fname))   //Si existe tal inodo devuelve su id
        return i;
 }
 return -1;
}


/*FIN FUNCIONES AUXILIARES*/


/********************************************************************************************/
// En las diapositivas (Tema 4: 155 - 162) tambien estan estas 2 funciones para leer metadatos
/********************************************************************************************/

//Funcion encargada de pasar la informacion de disco a memoria
int metadata_fromDiskToMemory ( void ) {
	char buffer[BLOCK_SIZE] ;

	//actualiza el superbloque[0], el cual contiene mapas de inodos y datos
	bread(DEVICE_IMAGE, 0, buffer);
	memmove(&(superbloque[0]), buffer, sizeof(TipoSuperbloque));

	//actualizo todos los inodos en memoria, empiezan desde el bloque 1 (siendo el 0 el superbloque[0])
	int inodosBloque = MAX_FICHEROS / superbloque[0].numBloquesInodos;
	for (int i = 0; i < superbloque[0].numBloquesInodos; i++){  //para cada bloque de inodos (hay 2)

		bread(DEVICE_IMAGE,superbloque[0].primerInodo + i, buffer);
		memmove(&(inodos[i*inodosBloque]), buffer, inodosBloque*sizeof(TipoInodo));   //actualiza el valor de los 24 inodos del bloque

	}
	
 return 0;
}

//Funcion encargada de escribir la informacion de memoria al disco
int metadata_fromMemoryToDisk ( void ) {
	char buffer[BLOCK_SIZE] ;
	//guardamos en disco la informacion del superbloque[0] (incluye mapas de inodos y bloques de datos)
	memset(buffer, 0, BLOCK_SIZE) ;
    memmove(buffer, &(superbloque[0]), sizeof(TipoSuperbloque)) ;
    bwrite(DEVICE_IMAGE, 0, buffer) ;
	


	//escribimos los inodos a disco, 24 en cada bloque
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


//Funcion que inicializa las estructuras que van a ser necesarias
int mkFS(long deviceSize)
{
	//Si el tamano del disco es menor o mayor que el rango proporcionado devuelve error
	if(deviceSize < MIN_DISCO){
		printf("Tamaño del disco demasiado pequeño, el tamaño mínimo es %d", MIN_DISCO);
		return -1;
	}
	if(deviceSize > MAX_DISCO){
		printf("Tamaño del disco demasiado grande, el tamaño máximo es %d", MAX_DISCO);
		return -1;
	}

	//Inicializo las variables y estructuras necesarias
	char buffer[BLOCK_SIZE];
	superbloque[0].numMagico = 12345678; 		   	   // se utiliza para comprobar que se ha creado por nuestro mkfs
	superbloque[0].numInodos = MAX_FICHEROS;          // el numero de inodos equivale al maximo de ficheros
	superbloque[0].primerInodo = 1;
	superbloque[0].numBloquesInodos = 2;

	/*El numero de bloques de datos dependera del tamano del dispositivo, siendo el total
	de bloques deviceSize / BLOCK_SIZE:
	  - Si tiene 243 bloques o mas habra 240 bloques de datos (para los 48 ficheros)
	  - Si tiene 242 bloques o menos, por ejemplo, un total de 230, habra 230 -3 = 227 bloques 
	  para datos, pues los 3 primeros son de la cabecera*/

	if(deviceSize / BLOCK_SIZE < MAX_FICHEROS * MAX_FILE_SIZE / BLOCK_SIZE + 3){
		superbloque[0].numBloquesDatos = deviceSize / BLOCK_SIZE - 3;
	}
	else{
		superbloque[0].numBloquesDatos = MAX_FICHEROS * MAX_FILE_SIZE / BLOCK_SIZE;
	}

	superbloque[0].primerBloqueDatos = 1 + superbloque[0].numBloquesInodos;
	superbloque[0].tamDispositivo = deviceSize;
	
	/*PRUEBA DE TAMANO
	esta prueba la realizamos para ver el tamaño que imprimia con sizeof, puesto que nos ocurre el 
	problema comentado en la memoria

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
	printf("%ld\n", sizeof(inodos[0].integridad));
	printf("%ld\n", sizeof(inodos[0]));
	*/


	//Pone a 0 todo el mapa de inodos
	for (int i=0; i<superbloque[0].numInodos; i++){
		memset(&(superbloque[0].mapaInodos[i]), 0, sizeof(superbloque[0].mapaInodos[i]) );
	}

	//Pone a 0 todo el mapa de bloques
	for (int i=0; i<superbloque[0].numBloquesDatos; i++){
		memset(&(superbloque[0].mapaBloques[i]), 0, sizeof(superbloque[0].mapaBloques[i]) );
	}

	//Pone a 0 la informacion de los inodos
	for (int i=0; i<superbloque[0].numInodos; i++){
		memset(&(inodos[i]), 0, sizeof(TipoInodo) );
	}
	
	// escribe los datos en disco
	metadata_fromMemoryToDisk();

	//Pone a 0 los bloques de datos
	memset(buffer, 0, BLOCK_SIZE);
	for (int i = 0; i < superbloque[0].numBloquesDatos; i++){
		bwrite(DEVICE_IMAGE, superbloque[0].primerBloqueDatos + i, buffer);
	}

	return 0;

}

//Funcion encargada de montar el dispositivo para su uso
int mountFS(void)
{
	if (montar == 1){
		return -1;			//Si ya esta montado devuelve error
	}

	//obtiene los datos desde el disco
	metadata_fromDiskToMemory();

	if(12345678 != superbloque[0].numMagico){
		return -1;			//Si el numero magico no coincide devuelve error
	}

	montar = 1;				//marca como montado

	return 0;
}

//Desmonta el dispositivo
int unmountFS(void)
{

	if (montar == 0){
		return -1;				//Si ya esta desmontado devuelve error
	}

	//No se podra desmontar si hay algun fichero abierto
	for (int i = 0; i < superbloque[0].numInodos; i++){
		if (inodos_x[i].abierto == 1){
			return -1;
		}
	}

	//Vuelca la informacion al disco
	metadata_fromMemoryToDisk() ;

	montar = 0;						//Marca como desmontado

	return 0;
}

//Funcion que permite crear un nuevo fichero en el sistema
int createFile(char *nombre)
{
	//para crear el fichero voy a pedir tanto un bloque libre (alloc) como un inodo (ialloc), por lo que declaro 2 id's
	int id_inodo, id_bloque ;

	if (strlen(nombre) == 0 || strlen(nombre) > 32){
		return -1;				// Si el nombre no concuerda con el tamano propuesto
	}

	if (montar == 0){
		return -1;				//Si el sistema no esta montado da error
	}

	id_inodo = namei(nombre);   //Compruebo que no exista ya un fichero con ese nombre usando namei()
	if(id_inodo >= 0){	
		return -1;
	}

	//Pido un inodo libre con ialloc()
	id_inodo = ialloc() ;
	if (id_inodo < 0) { 		//Si ialloc devuelve un valor menor a 0 da error
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


	for (int i = 1; i < MAX_FILE_SIZE / BLOCK_SIZE; i++){  /* Asigno el resto de bloques directos
															  a un valor no valido por el momento*/
		inodos[id_inodo].bloqueDirecto[i] = 255;

	}

	inodos_x[id_inodo].posicion = 0;					// establezco el puntero a 0
	inodos_x[id_inodo].abierto = 0;						// marco el inodo como cerrado

	return 0;

}

//Funcion que borra un fichero del sistema
int removeFile(char *nombre)
{
	int id_inodo ;

	char c [BLOCK_SIZE];

	if (montar == 0){
		return -1;					//Si el sistema no esta montado da error
	}

	id_inodo = namei(nombre);   //Compruebo que exista ya un fichero con ese nombre usando namei()
	if(id_inodo < 0){			//Dara error si namei devuelve un valor menor a 0, puesto que para borrar un fichero debe existir
		return -1;
	}

	inodos_x[id_inodo].posicion = 0;
	inodos_x[id_inodo].abierto = 0;			//Pongo los valores del inodo x a 0

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

//Funcion que abre un fichero dado su nombre
int openFile(char *nombre)
{
	int id_inodo ;
	
	if (montar == 0){
		return -2;					//Si el sistema no esta montado da error
	}

	// Obtenemos el inodo asociado al nombre propuesto.
	id_inodo = namei(nombre) ;
	if (id_inodo < 0){
		return id_inodo ;     // Si no existe el fichero devuelvo -1 (id_inodo = -1 pues namei no lo encuentra)
	}

	if (inodos_x[id_inodo].abierto == 1){
		return -2;								//Si ya esta abierto devuelvo -2 (error)
	}

	if(inodos[id_inodo].integridad != 0){
		return -2;								//Si tiene integridad debe abrirse con integridad
	}
	// Compruebo si el inodo es de tipo enlace
	if (inodos[id_inodo].tipo == T_ENLACE) {
		 
		int id_inodo2 = namei(inodos[id_inodo].nombreEnlace) ;
			if (id_inodo2 < 0){
				return -2 ;     // Si no existe el fichero al que apunta devuelve error
			}  

		if (inodos[id_inodo2].tipo == T_ENLACE) {
			return -2;					// No queremos bucles, no permitimos enlaces a enlaces
		}
		return openFile (inodos[id_inodo].nombreEnlace);		//abro el fichero asociado al enlace
	}		
	inodos_x[id_inodo].posicion = 0;       //Establezco el puntero a 0 y marco como abierto
	inodos_x[id_inodo].abierto = 1;
	
	return id_inodo;                       //Devuelve el descriptor del fichero

}

//Funcion que cierra un fichero dado su descriptir
int closeFile(int descriptor)
{	
	if (montar == 0){
		return -1;									//Si el sistema no esta montado da error
	}

	if (descriptor < 0 || (descriptor >= superbloque[0].numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}
	if (inodos_x[descriptor].abierto == 0){
		return -1;									//Si ya está cerrado no lo vuelvo a cerrar
	}

	if(inodos[descriptor].integridad != 0){
		return -1;								//Si tiene integridad debe cerrarse con integridad
	}

	if (inodos[descriptor].tipo == T_ENLACE) {
		 return -1;								// No se puede cerrar un enlace
	}

	inodos_x[descriptor].abierto = 0;			//Marco como cerrado
	metadata_fromMemoryToDisk() ;				//Vuelco la informacion a disco
	return 0;

}

//Funcion que lee la informacion de un fichero y la almacena en un buffer
int readFile(int descriptor, void *buffer, int size)
{

	int id_bloque ;

	if (montar == 0){
		return -1;									//Si el sistema no esta montado da error
	}

	if (descriptor < 0 || (descriptor >= superbloque[0].numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}

	if(inodos_x[descriptor].abierto == 0){
		return -1;									//Si no esta abierto doy error
	}

	if(inodos[descriptor].tipo == T_ENLACE){
		return -1;									//Si es de tipo enlace da error
	}

	//Si el tamano pedido es mayor que lo que queda devuelvo lo que queda por leer
	if (inodos_x[descriptor].posicion + size > inodos[descriptor].tamano){
		size = inodos[descriptor].tamano - inodos_x[descriptor].posicion;    
	}


	if (size <= 0){
		return 0;         // Devuelvo 0 si no queda nada por leer
	}

	int actual = inodos_x[descriptor].posicion / BLOCK_SIZE;		//Obtengo el bloque actual
	int leido = 0;

	//Mientras que lo que falte por leer mas la posicion actual requiere de pasar de bloque
	while((inodos_x[descriptor].posicion + size) >= (actual + 1) * BLOCK_SIZE){
		
		//LLamamos a bmap para obtener el id del bloque correspondiente
		id_bloque = bmap(descriptor, inodos_x[descriptor].posicion);          

		if(id_bloque < 0){
			return -1;         //Si el id del bloque no es valido devuelve error
		}

		char b[BLOCK_SIZE] ;
		
		// calcula la posicion con respecto al tamano del bloque 
		int posActual =	inodos_x[descriptor].posicion % BLOCK_SIZE;  
		
		//Calculo lo que lee en esta iteracion
		int leer = (actual + 1) * BLOCK_SIZE - inodos_x[descriptor].posicion;

		// lee el bloque y lo guarda en el buffer
		bread(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+id_bloque, b);     

		memmove(buffer + leido, b+posActual, leer);	/* Guarda la informacion en el buffer proporcionado usando lo almacenado en el 
													buffer b, como este es de tamano BLOCK SIZE y puede que queramos empezar a leer
													desde el puntero, uso la posActual que informa de la posicion que tendria el puntero
													en para 2048 bytes*/

		inodos_x[descriptor].posicion += leer;		// Ajusta el puntero de posicion										// guarda en el buffer a devolver la informacion que quiere

		size -= leer;							// ajusta size por si queda algo que leer de otro bloque
		actual += 1;							// ajusta el bloque actual
		leido += leer;							// encargado de mantener cuanto se ha leido

		
	}

		id_bloque = bmap(descriptor, inodos_x[descriptor].posicion); //LLamamos a bmap para obtener el id del bloque correspondiente

		if(id_bloque < 0){
			return -1;         //Si el id del bloque no es valido devuelve error
		}

		char b[BLOCK_SIZE] ;

		int posActual =	inodos_x[descriptor].posicion % BLOCK_SIZE;  // calcula la posicion con respecto al tamano del bloque 

		bread(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+id_bloque, b);

		memmove(buffer + leido, b+posActual, size);	/* Guarda la informacion en el buffer proporcionado usando lo almacenado en el 
													buffer b, como este es de tamano BLOCK SIZE y puede que queramos empezar a leer
													desde el puntero, uso la posActual que informa de la posicion que tendria el puntero
													en para 2048 bytes*/

		inodos_x[descriptor].posicion += size;						 // Ajusta el puntero de posicion

		leido += size;												// encargado de mantener cuanto se ha leido
	return leido;		//Devuelve el numero de bytes leidos

}

//Funcion que escribe la informacion almacenada en un buffer al fichero
int writeFile(int descriptor, void *buffer, int size)
{
	int id_bloque ;

	if (montar == 0){
		return -1;									//Si el dispositivo no esta montado da error
	}

	if (descriptor < 0 || (descriptor >= superbloque[0].numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}

	if(inodos_x[descriptor].abierto == 0){
		return -1;									//Si no esta abierto doy error
	}

	if(inodos[descriptor].tipo == T_ENLACE){
		return -1;									//Si es de tipo enlace da error
	}

	//Si el tamano pedido es mayor que lo que queda devuelvo el hueco que queda para escribir
	if (inodos_x[descriptor].posicion+size > MAX_FILE_SIZE-1){   
		size = MAX_FILE_SIZE-1 - inodos_x[descriptor].posicion;  
	}

	if (size <= 0){
		return 0;			// Devuelvo 0 si no se pide leer nada
	}
 
	int actual = inodos_x[descriptor].posicion / BLOCK_SIZE;    //Obtengo el bloque actual
	int escrito = 0;

	while((inodos_x[descriptor].posicion + size) >= (actual + 1) * BLOCK_SIZE){
		
		id_bloque = bmap(descriptor, inodos_x[descriptor].posicion);    // busco el bloque con bmap
		if(id_bloque == 255){
			id_bloque = alloc();          //Si el bloque tiene valor 255 pido un bloque vacio
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
			id_bloque = alloc();          //Si el bloque tiene valor 255 pido un bloque vacio
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

//Funcion que actualiza el puntero de lectura y escritura
int lseekFile(int descriptor, long offset, int whence)
{	

	if (montar == 0){
		return -1;
	}

	if (descriptor < 0 || (descriptor >= superbloque[0].numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}

	if(inodos[descriptor].tipo == T_ENLACE){
		return -1;									//Si es de tipo enlace da error
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
		if (offset <= 0){

			if(inodos_x[descriptor].posicion + offset < 0){  //Si queremos ir a una posicion menor que 0 da error
				return -1;
			}
			else{
				inodos_x[descriptor].posicion = inodos_x[descriptor].posicion + offset;
				return 0;
			}			
		}

		if (offset > 0){
			if (inodos_x[descriptor].posicion + offset > inodos[descriptor].tamano){
				return -1;			//Si queremos ir a una posicion mayor que el tamano actual del fichero
			}
			else{
				inodos_x[descriptor].posicion = inodos_x[descriptor].posicion + offset;
				return 0;
			}
			
		}
	}


	return -1;      //Si whence no vale ninguno de los valores propuestos


}

//Funcion que comprueba la integridad de un fichero
int checkFile (char * fileName)
{
	int id_inodo ;

	if (montar == 0){
		return -2;						//Comprobamos que el dispositivo este montado
	}
	
	// Obtenemos el inodo asociado al nombre propuesto.
	id_inodo = namei(fileName) ;
	if (id_inodo < 0){
		return -2 ;     // Si no existe el fichero devuelvo -1 (id_inodo = -1 pues namei no lo encuentra)
	}

	if (inodos_x[id_inodo].abierto == 1){
		return -2;									//Si el fichero esta abierto devuelve -2
	}

	if(inodos[id_inodo].tipo == T_ENLACE){
		return -2;									//Si es de tipo enlace da error
	}

	if (inodos[id_inodo].integridad == 0){
		return -2;									//Si no tiene integridad devuelve error
	}

	uint32_t hashFichParam = inodos[id_inodo].integridad;
	// Calculamos la integridad total del fichero como la suma de las integriaddes de todos sus bloques directos.
	uint32_t hashFich = 0;
	uint32_t hash = 0;
	for (int i = 0; i < 5; i++) {
		char b [BLOCK_SIZE];
		//Leemos los datos de los bloques y calculamos el CRC32
		int desplazar = inodos[id_inodo].bloqueDirecto[i];
		bread(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+desplazar,b);
		hash = CRC32 ((const unsigned char *)b, sizeof(b));

		//Vamos actualizando la suma
		hashFich += hash;

		// Si se ha dado un error al calcular la integriad del bloque: Error.
		if (hash == 0) {
			return -2;
		}
	}
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

//Funcion que da integridad a un fichero
int includeIntegrity (char * fileName)
{
	int id_inodo ;

	if (montar == 0){
		return -2;					//Si no esta montado el dispositivo devuelve -2
	}
	
	// Obtenemos el inodo asociado al nombre propuesto.
	id_inodo = namei(fileName) ;
	if (id_inodo < 0){
		return id_inodo ;     // Si no existe el fichero devuelvo -1 (id_inodo = -1 pues namei no lo encuentra)
	}
	

	if (inodos[id_inodo].integridad != 0){
		return -2;				//Si ya tiene integridad da error
	}

	if(inodos[id_inodo].tipo == T_ENLACE){
		return -1;									//Si es de tipo enlace da error
	}

	// Calculamos la integridad total del fichero como la suma de las integriaddes de todos sus bloques directos.
	uint32_t hashFich = 0;
	uint32_t hash = 0;
	for (int i = 0; i < 5; i++) {
		char b [BLOCK_SIZE];
		//Si el bloque tiene valor 255 pido un bloque con alloc y se lo asigno al fichero
		if(inodos[id_inodo].bloqueDirecto[i] == 255){
			inodos[id_inodo].bloqueDirecto[i] = alloc();          
			if (inodos[id_inodo].bloqueDirecto[i] < 0){
				return -1;
			}

		}
		int desplazar = inodos[id_inodo].bloqueDirecto[i];
		//Leemos los datos de los bloques y calculamos el CRC32
		bread(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+desplazar,b);
		hash = CRC32 ((const unsigned char *)b, sizeof(b));
	
		//Vamos actualizando la suma
		hashFich += hash;

		// Si se ha dado un error al calcular la integriad del bloque: Error.
		if (hash == 0) {
			return -2;
		}
	}

	// Le añadimos la integridad al fichero.
	inodos[id_inodo].integridad = hashFich;
    return 0;
}

//Funcion que abre un fichero con integridad
int openFileIntegrity(char *fileName)
{
	int id_inodo ;

	if (montar == 0){
		return -3;				//Si el dispositivo no esta montado devuelve -3
	}
	
	// Obtenemos el inodo asociado al nombre propuesto.
	id_inodo = namei(fileName) ;
	if (id_inodo < 0){
		return id_inodo ;     // Si no existe el fichero devuelvo -1 (id_inodo = -1 pues namei no lo encuentra)
	}


	if (inodos_x[id_inodo].abierto == 1){
		return -3;								//Si ya esta abierto devuelvo -3 (error)
	}

	
	if (inodos[id_inodo].integridad == 0) {
		return -3;								//Si el fichero no tiene integridad devuelve error
	}

	if(inodos[id_inodo].tipo == T_ENLACE){
		return -3;									//Si es de tipo enlace da error
	}
	
	// Si tiene integridad, comprobamos que no este corrupto.
	int integridad = checkFile(fileName);
	if (integridad == -1) {
		return -2;
	}

	if(integridad == -2){
		return -3;
	}

	inodos_x[id_inodo].posicion = 0;       //Establezco el puntero a 0 y marco como abierto
	inodos_x[id_inodo].abierto = 1;

	return id_inodo;						//Devuelvo el descriptor del fichero 
}

//Funcion que cierra un fichero con integridad
int closeFileIntegrity(int fileDescriptor)
{

	if (montar == 0){
		return -1;						//Si el dispositivo no esta montado
	}

	if (fileDescriptor < 0 || (fileDescriptor >= superbloque[0].numInodos)){
		return -1 ;									//Devuelve error si el descriptor no corresponde a un valor valido de inodo
	}
	
	if (inodos_x[fileDescriptor].abierto == 0){
		return -1;									//Si ya está cerrado no lo vuelvo a cerrar
	}

	if(inodos[fileDescriptor].integridad == 0){
		return -1;									//Si no tiene integridad da error
	}

	if(inodos[fileDescriptor].tipo == T_ENLACE){
		return -1;									//Si es de tipo enlace da error
	}

	// Calculamos la integridad total del fichero como la suma de las integriaddes de todos sus bloques directos.
	uint32_t hashFich = 0;
	uint32_t hash = 0;
	for (int i = 0; i < 5; i++) {
		char b [BLOCK_SIZE];
		int desplazar = inodos[fileDescriptor].bloqueDirecto[i];
		//Leemos los datos de los bloques y calculamos el CRC32
		bread(DEVICE_IMAGE, superbloque[0].primerBloqueDatos+desplazar,b);
		hash = CRC32 ((const unsigned char *)b, sizeof(b));

		//Vamos actualizando la suma
		hashFich += hash;

		// Si se ha dado un error al calcular la integriad del bloque: Error.
		if (hash == 0) {
			return -2;
		}
	}

	inodos[fileDescriptor].integridad = hashFich;	//Actualizamos la integridad y marcamos como cerrado
	inodos_x[fileDescriptor].abierto = 0;
	metadata_fromMemoryToDisk() ;                //Volvamos la informacion a disco
	return 0;
}

//Funcion que crea un enlace simbolico
int createLn(char *fileName, char *linkName)
{
	int id_inodo;
	int id_enlace;

	if (strlen(fileName) == 0 || strlen(fileName) > 32){
		return -1;				// Si el nombre no concuerda con el tamano propuesto
	}

	if (strlen(linkName) == 0 || strlen(linkName) > 32){
		return -1;				// Si el nombre no concuerda con el tamano propuesto
	}

	if (montar == 0){
		return -1;				//Si el dispositivo no esta montado
	}
	
	// Obtenemos el inodo asociado al nombre propuesto.
	id_inodo = namei(fileName);
		if (id_inodo < 0){
			return -1 ;     // Si no existe el fichero devuelvo -1 
		}

	id_enlace = namei(linkName);
		if (id_enlace >= 0){
			return -2;		// Si ya existe un enlace con ese nombre devuelvo -2
		}

	if (inodos[id_inodo].tipo == T_ENLACE){
		return -1;						//No permitimos enlaces a enlaces
	}

	id_inodo = ialloc();
	if (id_inodo < 0) { 		//Si ialloc devuelve un valor menor a 0 devuelvo error, no hay inodos libres
		return -2 ;
		 }


	inodos[id_inodo].tipo = T_ENLACE ; 					// es de tipo enlace
	strcpy(inodos[id_inodo].nombre, linkName);    		// asigno el nombre al inodo
	strcpy(inodos[id_inodo].nombreEnlace, fileName);    // asigno el nombre de lo que apunta el enlace
	inodos[id_inodo].tamano = 0;
	inodos_x[id_inodo].posicion = 0;					// establezco el puntero a 0
	inodos_x[id_inodo].abierto = 0;						// marco el inodo como abierto

	return 0;

}

//Funcion que elimina un enlace simbolico existente
int removeLn(char *linkName)
{
	int id_inodo ;

	if (montar == 0){
		return -1;					//Si el dispositivo no esta montado
	}
	
	// Obtenemos el inodo asociado al nombre propuesto.
	id_inodo = namei(linkName) ;
	if (id_inodo < 0){
		return -1 ;     // Si no existe el enlace devuelvo -1 
	}


	memset( &(inodos[id_inodo]), 0, sizeof(TipoInodo) ) ;	 	// pongo todos los valores del inodo a 0
	ifree(id_inodo) ;										 	// Libero el inodo		
	metadata_fromMemoryToDisk() ;				//guardo todas las modificaciones a disco
    return 0;
}



