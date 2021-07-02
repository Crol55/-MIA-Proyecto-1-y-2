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

#endif // ESTRUCTURAS_H
