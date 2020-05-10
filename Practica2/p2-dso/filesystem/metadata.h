
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
#define MAX_FICHEROS 48         //numero de ficheros maximo definidos en el requisito no funcional 1 (equivale al numero de inodos)
#define LONG_NOMBRES 32         //Longitud maxima del nombre de un fichero o enlace simbólico segun NF2
#define MIN_DISCO 460*1024      //Menor tamano disco segun NF6
#define MAX_DISCO 600*1024      //Mayor tamano disco segun NF6
#define PADDING_SUPERBLOQUE 1747  // Relleno necesario para el superbloque
#define PADDING_INODO 272         // Relleno para cada bloque de inodos
#define NUMBLOQUESDATOS MAX_FICHEROS * 10240 / 2048 

#define T_FICHERO 1             //inodo de tipo archivo
#define T_ENLACE 0              //inodo de tipo enlace

#define bitmap_getbit(bitmap_, i_) (bitmap_[i_ >> 3] & (1 << (i_ & 0x07)))
static inline void bitmap_setbit(char *bitmap_, int i_, int val_) {
  if (val_)
    bitmap_[(i_ >> 3)] |= (1 << (i_ & 0x07));
  else
    bitmap_[(i_ >> 3)] &= ~(1 << (i_ & 0x07));
}

typedef struct{
  uint32_t numMagico;                 /* Numero magico del superbloque*/
  uint8_t numInodos;                  /* Numero de inodos total */
  uint8_t primerInodo;                /* Numero correspondiente al bloque del inodo raiz */
  uint8_t numBloquesInodos;           /* Numero total de bloques de inodos */
  uint8_t numBloquesDatos;            /* Numero total de bloques de datos */
  uint8_t primerBloqueDatos;          /* Numero correspondiente al primer bloque de datos */
  uint32_t tamDispositivo;            /* Tamano del dispositivo en bytes*/
  char mapaBloques[NUMBLOQUESDATOS];  /*Mapa de bloques de datos 100…0 (usado: b_map[x]=1 | libre: b_map[x]=0)*/
  char mapaInodos[MAX_FICHEROS];         /*Mapa de inodos 100…0 (usado: i_map[x]=1 | libre: i_map[x]=0)*/
  char relleno[PADDING_SUPERBLOQUE];  /* Relleno para completar un bloque */
} TipoSuperbloque;

TipoSuperbloque superbloque[1];

typedef struct {
  uint8_t tipo;                        /* Fichero == 1, enlace blando == 0 */
  char nombre[LONG_NOMBRES];           /* Nombre del fichero asociado al inodo  */
  char nombreEnlace[LONG_NOMBRES];     /* Nombre del enlace asociado al inodo  */
  uint16_t tamano;                     /* Tamano actual del fichero en bytes */
  uint8_t bloqueDirecto[5];            /* Numero del bloque directo (desde bloque 0 a numBloquesDatos - 1) */
  //char relleno[PADDING_INODO];         /* Relleno para completar un bloque */
} TipoInodo;

TipoInodo inodos[MAX_FICHEROS];

typedef struct {
  uint16_t posicion;                    /* Informacion extra de los inodos, posicion de lectura o escritura */
  uint8_t abierto;                      /* Informacion extra de los inodos, 0 si no esta abierto y 1 si lo esta */
} TipoInodo_x [MAX_FICHEROS];

int montar = 0;