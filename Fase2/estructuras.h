#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <time.h>
#include <iostream>

typedef struct PARTITION
{
    char part_status; // indica si esta activa o no
    char part_type; // indica si es primaria o extendida, P o E
    char part_fit; // tipo de ajuste, B(best) o F(first) o W(worst)
    int part_start;// indica en que byte del disco comienza la partitcion
    int part_size; // tamaño total de la particion en bytes
    char part_name[16]; // nombre de la particion

}partition;



typedef struct MBR
{
    int mbr_tamano; // Tamaño total del disco en bytes.
    time_t mbr_fecha_creacion; // fecha y hora de creacion del disco
    int mbr_disk_signature; // numero random que identifica al disco
    char disk_fit;
    partition particiones[4];

}mbr;


typedef struct NODO // llenao en lista enlazaad
{
    std::string ruta;
    std::string nombre_particion;
    std::string id; // asignado dinamicamente en lista_enlazada
    struct NODO *siguiente;
}nodo;



typedef struct extended_boot_record{
    char part_status; // indica si la particion esta activa o no
    char part_fit; // tipo de ajuste de la particion, best, worst , first
    int part_start; // indica en que byte del disco inicia la particion
    int part_size;  // tamaño total de la particion
    int part_next; //
    char part_name[16];
}ebr;


typedef struct SUPER_BLOQUE
{
    int s_filesystem_type; /*numero que identifica el sistema de archivos utilizado*/
    int s_inodes_count; /*guarda el numero total de inodos*/
    int s_block_count; /*Guarda el numero total de bloques*/
    int s_free_blocks_count; /*contiene el numero de bloques libres*/
    int s_free_inodes_count; /*contiene el numero de inodos libres*/
    time_t s_mtime; /*ultima fecha en la que el sistema fue montado*/
    time_t s_umtime; /*ultima fecha en la que el sistema fue montado*/
    int s_mnt_count; /*cuantas veces se ha montado el sistema*/
    int s_magic; /*valor en hexadecimal*/
    int s_inode_size; /*tamaño del inodo*/
    int s_block_size; /*tamaño del bloque*/
    int s_first_ino; /*primer inodo libre*/
    int s_first_blo; /*primer bloque libre*/
    int s_bm_inode_start; /*Guardara el inicio del bitmap de inodos*/
    int s_bm_block_start; /*Guardara el inicio del bitmap de bloques*/
    int s_inode_start; /*Guardara el inicio de la tabla de inodos*/
    int s_block_start; /*Guardara el inicio de la tabla de bloques*/

}super_bloque;


typedef struct TABLA_INODOS
{
    int i_uid; /*identificador del propietario del archivo o carpeta*/
    int i_gid; /*id del grupo al que pertenece el archivo o carpeta*/
    int i_size; /*tamaño del archivo en bytes (carpeta o archivo) -1*/
    time_t i_atime; /*ultima fecha en que se leyo el inodo sin modificarlo*/
    time_t i_ctime; /*fecha en la que se creo el inodo*/
    time_t i_mtime; /*fecha en la que se modifico el inodo*/
    int i_block[15]; /*Del 0-12 son bloques directos del 13-15 son indirectos*/
    char i_type; /* 1 = archivo , 0 = carpeta */
    int i_perm; /*se manejara a nivel de bits asi que en total deberian de ser 9 bits (user,grupo,otros)*/

}Inodo;


typedef struct CONTENT
{
    char b_name[12];
    int b_inodo;
}content;

typedef struct BLOQUE_CARPETAS
{
    content b_content[4];
}bloque_carpetas;


typedef struct BLOQUE_ARCHIVOS
{
    char b_content[64];

}bloque_archivos;


typedef struct BLOQUE_APUNTADORES
{
    int b_pointers[16];

}bloque_apuntadores;


typedef struct JOURNALING
{
    char operacion[10];
    char nombre[300];
    char contenido[64];
    int propietario;

    //time_t fecha;
}journaling;



typedef struct OBJETO // utilizado en existe_carpeta
{
    bool esValido;
    Inodo padre;
    int   pos_inodo_padre;
    std::string nombre_aCrear;

}objeto_existencia;


#endif // ESTRUCTURAS_H
