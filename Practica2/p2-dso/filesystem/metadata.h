
/*
 *
 * Operating System Design / Diseño de Sistemas Operativos
 * (c) ARCOS.INF.UC3M.ES
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	Last revision 01/04/2020
 *
 */

#define NUM_MAGICO 12345678     //numero magico elegido para el sistema de ficheros
#define MAX_FICHEROS 48         //numero de ficheros maximo definidos en el requisito no funcional 1
#define LONG_NOMBRES 32         //Longitud maxima del nombre de un fichero o enlace simbólico segun NF2
#define MIN_DISCO 460*1024      //Menor tamano disco segun NF6
#define MAX_DISCO 600*1024      //Mayor tamano disco segun NF6

#define bitmap_getbit(bitmap_, i_) (bitmap_[i_ >> 3] & (1 << (i_ & 0x07)))
static inline void bitmap_setbit(char *bitmap_, int i_, int val_) {
  if (val_)
    bitmap_[(i_ >> 3)] |= (1 << (i_ & 0x07));
  else
    bitmap_[(i_ >> 3)] &= ~(1 << (i_ & 0x07));
}

typedef struct{
  unsigned int numMagico;               /* Numero magico del superbloque*/
  unsigned int numBloquesMapaInodos;    /* Numero de bloques del mapa inodos*/
  unsigned int numBloquesMapaDatos;     /* Numero de bloques del mapa  datos */
  unsigned int numInodos;               /* Numero de inodos total */
  unsigned int primerInodo;             /* Numero correspondiente al bloque del inodo raiz */
  unsigned int numBloquesDatos;         /* Numero total de bloques de datos */
  unsigned int primerBloqueDatos;       /* Numero correspondiente al primer bloque de datos */
  unsigned int tamDispositivo;          /* Tamano del dispositivo en bytes*/
  char relleno[PADDING_SB];             /* Relleno para completar un bloque */
} TipoSuperbloque;

typedef struct{
  unsigned int tipo;                    /*  SOLO PUEDEN SER FICHEROS */
  char nombre[32];                     /* Nombre del fichero asociado al inodo */
  unsigned int inodosContenidos[200];   /* tipo==dir: lista de los inodosdel directorio HACE FALTAAAAAAAAAAA????????????????*/
  unsigned int tamanyo;                 /* Tamanoo actual del fichero en bytes */
  unsigned int bloqueDirecto;           /* Numero del bloque directo */
  char relleno[PADDING_INODO];          /* Relleno para completar un bloque */
} inodo;

typedef struct{
  int posicion;                         /* Informacion extra de los inodos */
  int abierto;                          /* Informacion extra de los inodos */
} inodo_x;