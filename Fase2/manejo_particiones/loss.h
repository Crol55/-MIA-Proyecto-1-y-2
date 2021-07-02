#ifndef LOSS_H
#define LOSS_H


#include <iostream>
#include "../lista_enlazada.h"
using namespace std;

/*====== Prototipos =========*/
/*==Externos===*/
partition get_partition(string nombre_particion, mbr m); // compilada en "getterysetter_globales"
/*======= FIN =============*/

void LOSS(string id)
{
    string ruta_disco = get_ruta(id); // funcion de "lista_enlazada.h"

    FILE *disco = fopen(ruta_disco.c_str(),"r+b");

    if(disco != NULL)
    {
        rewind(disco); // colocarse en el byte inicial
        mbr mbr_aux;
        fread(&mbr_aux, sizeof(mbr_aux), 1, disco); /*leemos los 136 bytes iniciales donde sabemos que se encuentra el mbr*/

        string nombre_particion = get_nombre_particion(id); // funcion de "lista_enlazada.h"
        partition particion_ = get_partition(nombre_particion, mbr_aux);

        super_bloque sb;
        fseek(disco, particion_.part_start-1, SEEK_SET); // nos colocamos donde inicia la particion
        fread(&sb, sizeof(super_bloque), 1, disco);

        // FORMATEAR CON \0 toda la particion iniciando desde el bm de bloques hasta el tamaño TOTAlde la particion

        int tamano_real_disco = particion_.part_start + particion_.part_size;
        int inicio_formateo = sb.s_bm_inode_start; // sb.s_bm_block_start
        int diferencia      = tamano_real_disco - inicio_formateo; // esto nos indica cuantos bytes se deben formatear
        fseek(disco, inicio_formateo-1, SEEK_SET); // nos colocamos al inicio del bm bloques (donde se iniciara el formateo)
        //cout<<"Inicio formateo:"<<inicio_formateo<<endl;
        //cout<<"Fin de formateo:"<<tamano_real_disco<<endl;

        for (int i = 0; i <= diferencia; i++)
        {
            fputc('\0', disco); // el puntero interno se mueve solito
        }
        //2) RESTAURAR EL SUPER_BLOQUE A SUS VALORES INICIALES y volver a colocarlo en la particion
        sb.s_free_blocks_count = sb.s_block_count;
        sb.s_free_inodes_count = sb.s_inodes_count;
        sb.s_first_ino         = 0;
        sb.s_first_blo         = 0;
        fseek(disco, particion_.part_start-1, SEEK_SET); // nos colocamos donde inicia la particion
        fwrite(&sb, sizeof(super_bloque), 1, disco);     // Volvemos a colocar el super bloque restaurado

        cout<<"Loss: La particion con el id="<<id<<", ha perdido toda la informacion\n";

        fclose(disco);
    }else{
        cout<<"loss: Error, la particion con el id="<<id<<", no existe en el disco\n";
    }
}
#endif // LOSS_H
