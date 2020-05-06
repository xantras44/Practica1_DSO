
/*
 *
 * Operating System Design / Diseño de Sistemas Operativos
 * (c) ARCOS.INF.UC3M.ES
 *
 * @file 	auxiliary.h
 * @brief 	Headers for the auxiliary functions required by filesystem.c.
 * @date	Last revision 01/04/2020
 *
 */ 

int ialloc ( void )
{
 // cogemos el primer i-nodo libre
 for (int = 0; i < superbloque.numInodos; i++) {
    if (superbloque.mapaInodos[i] == 0) {
        superbloque.mapaInodos[i] = 1; // cambiamos el estado del inodo a ocupado


        // le ponemos los valores por defecto al inodo AAAAAAAAAAAAAAAAAAAAAAAA
        memset(&(inodos[i]),0, sizeof(TipoInodoDisco));


        return i; // devolvemos el identificador del inodo.
    }
 }
 return -1;
}

int alloc ( void )
{
 char b[BLOCK_SIZE];
 // cogemos el primer bloque libre
 for (int=0; i<superbloque.numBloquesDatos; i++) {
    if (superbloque.mapaBloques[i] == 0) {
        superbloque.mapaBloques[i] = 1 ; // cambiamos el estado del bloque a ocupado.
        // le ponemos los valores por defecto al bloque (opcional).
        memset(b, 0, BLOCK_SIZE) ;
        bwrite(DISK, superbloque.primerBloqueDatos + i, b);
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

int free ( int block_id )
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
    int b[BLOCK_SIZE/4] ;
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
        return inodos.bloqueDirecto[inodo_id];
    return b[bloque_logico - 1] ; // hay 1 bloque directo -> x-1
}

int namei ( char *fname )
{
 // buscamos el inodo cuyo nombre sea <fname>
 for (int=0; i<superbloque.numInodos; i++) {
    if (! strcmp(inodos.nombre[i], fname))
        return i;
 }
 return -1;
}

