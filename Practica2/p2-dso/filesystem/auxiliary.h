
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
#include "filesystem/metadata.h"   // Type and structure declaration of the file system

int ialloc ( void )
{
 // buscar un i-nodo libre
 for (int = 0; i < sbloques[0].numInodos; i++) {
    if (imap[i] == 0) {
        imap[i] = 1; // inodo ocupado ahora
        // valores por defecto para i-nodo
        memset(&(inodos[i]),0, sizeof(TipoInodoDisco));
        return i; // devolver id. de i-nodo
    }
 }
 return -1;
}

int alloc ( void )
{
 char b[BLOCK_SIZE];
 for (int=0; i<sbloques[0].numBloquesDatos; i++) {
    if (bmap[i] == 0) {
        bmap[i] = 1 ; // bloque ocupado ahora
        // valores por defecto en el bloque (opcional)
        memset(b, 0, BLOCK_SIZE) ;
        bwrite(DISK, sbloques[0].primerBloqueDatos + i, b);
        return i ; // devolver id. de bloque
    }
 }
 return -1;
}

int ifree ( int inodo_id )
{
 // comprobar validez de inodo_id
 if (inodo_id > sbloques[0].numInodos)
    return -1;
 // liberar i-nodo
 imap[inodo_id] = 0;
 return -1;
}

int free ( int block_id )
{
 // comprobar validez de block_id
 if (block_id > sbloques[0].numBloquesDatos)
 return -1;
 // liberar bloque
 bmap[block_id] = 0;
 return -1;
}

int bmap ( int inodo_id, int offset )
{
 int b[BLOCK_SIZE/4] ;
 int bloque_logico ;
 bloque_logico = offset / BLOCK_SIZE;
 if (bloque_logico > (1+BLOCK_SIZE/4))
    return -1;
 // bloque de datos físico asociado: directo
 if (0 == bloque_logico)
    return inodos[inodo_id].bloqueDirecto;
 // bloque de datos físico asociado: indirecto
 bread(DISK, sbloques[0].primerBloqueDatos + inodos[inodo_id].bloqueIndirecto, b);
 return b[bloque_logico - 1] ; // hay 1 bloque directo -> x-1
}

int namei ( char *fname )
{
 // buscar i-nodo con nombre <fname>
 for (int=0; i<sbloques[0].numInodos; i++) {
    if (! strcmp(inodos[i].nombre, fname))
        return i;
 }
 return -1;
}

