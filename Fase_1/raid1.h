#ifndef RAID1_H
#define RAID1_H

#include "estructuras.h"
#include <iostream>
using namespace std;


string crear_nombre_discoRA1(string path) // crea el nombre que tendra el disco espejo
{
    string nuevo_nombre = "";

    for(int i =0; i < (int) path.length(); i++)
    {
        if(path[i] != '.')
        {
            nuevo_nombre += path[i]; // ir concatenando los caracteres
        }else {
           break; // salir del for
        }
    }
  return nuevo_nombre;
}

void MKDISK_RAID1(mbr m, string path, int size)
{
    string nombre_raid = crear_nombre_discoRA1(path) + "_ra1.disk";
    //cout<<nombre_raid;

    FILE * disco_raid = fopen(nombre_raid.c_str(), "w+b");
    if(disco_raid != NULL)
    {
        char buffer[1024];
        for (int i = 0; i < 1024; i++) {
            buffer[i] = '\0';
        }

        for(int i = 0; i < (size/1024); i++) // Para insertar \0 en el disco (raid) y darle tamaño
            fwrite(buffer,sizeof(buffer), 1, disco_raid);
        rewind(disco_raid);

        fwrite(&m, sizeof(m), 1, disco_raid); // Escribimos el mbr recien creado en el raid

        fclose(disco_raid);
    }else{
        cout<<"Raid1:Error al crear el disco Raid 1\n";
    }

}



void FDISK_RAID1(mbr m, string path) // Solo es de copiar el mbr en el disco( usado en crear,Borrar y redimencionar partiicon)
{
    string ruta_raid = crear_nombre_discoRA1(path) + "_ra1.disk";

    FILE * disco_raid = fopen(ruta_raid.c_str(), "r+b");

    if(disco_raid != NULL)
    {
        rewind(disco_raid);
        fwrite(&m, sizeof(m), 1, disco_raid); // Escribimos el mbr recien creado en el raid

        fclose(disco_raid);
    }else{
        cout<<"Raid1: Error->al usar Fdisk el disco Raid 1\n";
    }
}
#endif // RAID1_H
