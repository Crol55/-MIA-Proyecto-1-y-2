#ifndef MKFS_H
#define MKFS_H

#include <iostream>
#include "lista_enlazada.h"
#include "getterysetter_globales.h" // adentro de este ya esta 'estructuras.h'
#include "manejo_particiones/journaling.h"
#include "math.h"
#include "op_inodos.h"


using namespace std;


super_bloque Crear_Super_Bloque(double num_estructuras, int part_start, char fs)
{
    printf("El num de estructuras sera:%f\n",num_estructuras);
    int filesystem = 2; // por defecto sera el ext2
    if(fs == '3') // es el sistema ext3
    {
        filesystem = 3;
    }

    super_bloque sb ;
    int indice = part_start; /*indice nos ayudara para saber en donde se colocara el inicio del bitmap inodos,bloques, e inodos y bloques*/

    sb.s_filesystem_type = filesystem;
    sb.s_inodes_count = num_estructuras;
    sb.s_block_count  = 3*num_estructuras;
    sb.s_free_blocks_count = 3*num_estructuras; /*ESTO SOLO SE CUMPLE cuando el sistema de archivos esta recien creado luego varia*/
    sb.s_free_inodes_count = num_estructuras;   /*ESTO SOLO SE CUMPLE cuando el sistema de archivos esta recien creado luego varia*/
    sb.s_mtime = get_tiempo(); /*puede variar*/
    sb.s_umtime = get_tiempo(); /*puede variar*/
    sb.s_mnt_count = 1; /*cantidad de veces que se ha montado el sistema y puede variar*/
    sb.s_magic = 201314172;
    sb.s_inode_size = sizeof(Inodo);
    sb.s_block_size = sizeof(bloque_archivos);
    sb.s_first_ino = 0; /*el primer inodo libre se encuentra en la primera posicion ESTO IRA CAMBIANDO*/
    sb.s_first_blo = 0; /*el primer bloque libre se encuentra en la posicion 0 ESTO CAMBIARA DESPUES*/


        indice = indice + sizeof(super_bloque); /*Se calcula en donde iniciara el (bit_map de inodos si es EXT2) o (journaling si es EXT3)*/

        if(fs == '3')
            indice = indice + (num_estructuras * sizeof(journaling)) ; // si es ext3 indica donde iniciara el bit_map de inodos

        sb.s_bm_inode_start = indice ;

        indice = indice + num_estructuras; /*Inicio de el bit_map de bloques*/
        sb.s_bm_block_start = indice;

        indice = indice + (3*num_estructuras); /*Inicio de los INODOS */
        sb.s_inode_start = indice;

        indice = indice +(num_estructuras * sizeof(Inodo) ); /*Inicio de los bloques*/
        sb.s_block_start = indice;

 return sb;
}



void Formatear_Particion(FILE *disco, super_bloque sb, char type)
{
    if(type == 'r' || type == 'c')
    {
        cout<<"Realizando formateo de particion\n";
        // solo coloca 0's en bm_bloques y bm_inodos
        int inicio_bm_inodos = sb.s_bm_inode_start;
        int inicio_bm_bloques = sb.s_bm_block_start;
       // Borramos todo del bitmap inodos
        fseek(disco, inicio_bm_inodos-1, SEEK_SET); /*posicionar el punter0 en el byte inicial del bm_inodos*/
            for(int i=0; i< sb.s_inodes_count; i++)
            {
                fputc('\0',disco);
            }
      // borramos todo del bitmap bloques
        fseek(disco, inicio_bm_bloques-1, SEEK_SET); /*posicionar el punter0 en el byte inicial del bm_bloques*/
            for(int i=0; i< sb.s_block_count; i++)
            {
                fputc('\0',disco);
            }

        if(type=='c') // esto solo ocurre cuando se quiere realizar un formateo completo
        {

            int inicio_inodos = sb.s_inode_start;
            int inicio_bloques = sb.s_block_start;

          // borramos todo el contenido de los inodos
          fseek(disco, inicio_inodos-1, SEEK_SET); //posicionar el puntero en el inicio de inodos
              for(int i=0; i< (int)(sb.s_inodes_count * sizeof(Inodo)); i++)
              {
                  fputc('\0',disco);
              }

            // borraramos todo el contenido de los bloques
            fseek(disco, inicio_bloques-1, SEEK_SET); // posicionar el puntero al inicio de los bloques
            for(int i =0; i< (int)(sb.s_block_count * sizeof(bloque_archivos)); i++)
              {
                  fputc('\0',disco);
              }
        }
    }
    else cout<<"MKFS: El parametro 'type' tiene un valor erroeneo.\n\n";
}



void Crear_usersTXT(FILE *disco)
{
    fseek(disco, (particion_global.part_start-1), SEEK_SET); // Colocarnos donde inicia la particion_global
    super_bloque sb;
      fread(&sb, sizeof(super_bloque),1,disco);

// 1) Extraer el bloque de carpetas inicial, que estara al principio de los bloques ( Es el bloque 1 de la raiz)
    int inicio_bloques = sb.s_block_start;

    fseek(disco, (inicio_bloques-1), SEEK_SET); // puntero interno lo colocamos al inicio de los bloques
    bloque_carpetas bc_lectura;
    int bc_leido = fread(&bc_lectura, sizeof(bloque_carpetas), 1, disco); // extraemos el bloque de carpetas del inodo raiz (/)

       if(bc_leido==1)
       {
        // 2) En su 3er posicion, colocar alli la posicion del inodo que contendra users.txt y su nombre
           strcpy(bc_lectura.b_content[2].b_name , "users.txt");
                  bc_lectura.b_content[2].b_inodo = 1;
           Sobrescribir_bloque_carpeta( bc_lectura, 0);

           for(int i = 0; i < 3; i++) // imprimir solo para ver lo que se almacenara
               printf("name:%s,b_inodo:%i\n",bc_lectura.b_content[i].b_name,bc_lectura.b_content[i].b_inodo);

        // 3) Crear un nuevo inodo que representara a users.txt
           Crear_INODO(disco, '1', 1, 27, 777,0); // crea el inodo users.txt y sus bloques de archivo

    // 4) llenar con la informacion de root lo que esta en el bloque recien creado colocarme en el bloque 1
           // el bloque a utilizar sera el bloque 1
             int pos = inicio_bloques + (1 * sizeof(bloque_archivos)); // moverse a 'patita' al bloque donde 'fijo' esta el users.txt
             fseek(disco, (pos -1), SEEK_SET); // nos colocamos en la posicion donde esta el bloque 'users.txt'
             bloque_archivos ba;
             int ba_leido = fread(&ba, sizeof(bloque_archivos),1,disco);

             if(ba_leido==1)
             {
                 // crear grupo root -> necesita como maximo 15 caracteres
                 char cad[64];
                     strcpy(cad, "1,G,root\n");
                     strcat(cad, "1,U,root,root,123\n");
                 // 5) colocar adentro del bloque_archivos la informacion de los usuarios y grupos

                     strcpy(ba.b_content, cad);
                 // 6) Reescrbir el bloque de archivos users.txt
                     fseek(disco, (pos -1), SEEK_SET); // nos colocamos en la posicion donde esta el bloque 'users.txt'
                     fwrite(&ba, sizeof(bloque_archivos), 1, disco);  // reescribe el bloqué de archivos modificado

             }else  printf("Advertencia: no se pudo leer el bloque archivos que contiene users.txt en  'Crear_userTXT'\n");

       }else{
           printf("Advertencia: no se pudo leer el bloque inicial en 'Crear_userTXT'\n");
       }

}



void MKFS(string id, char type, char fs)
{
 // 1) RECUPERAR LA RUTA DEL DISCO MONTADO CON EL 'id'
    string ruta = get_ruta(id); // funcion de 'lista_enlazada.h'
           ruta_disco_global = ruta;
 // 2) ABRIR EL DISCO
    FILE * disco = fopen(ruta.c_str(), "r+b");
    if(disco != NULL)
    {
        mbr aux;
        rewind(disco);
        int mbr_leido;
            mbr_leido = fread(&aux,sizeof(aux),1,disco); /*leemos los 136 bytes iniciales donde sabemos que se encuentra el mbr*/

             if(mbr_leido==1)
             {
               //3) RECUPERAMOS EL NOMBRE DE LA PARTICION del disco montado con el id
               string nombre_particion = get_nombre_particion(id); // Funcion de 'lista_enlazada.h'

               /* 4) AHORA SE DEBE EXTRAER del DISCO LA PARTICION QUE CONINCIDA CON 'nombre_particion' hay que verificar que exista'*/
                partition particion_actual = get_partition(nombre_particion, aux);
                particion_global = particion_actual;

                int tamano_particion = particion_actual.part_size ;
                double n, num_estructuras;

                if(fs == '2')
                     n = (tamano_particion-sizeof(super_bloque)) / (4 + sizeof(Inodo) +(3*sizeof(bloque_archivos)) ); // para ext2
                else
                     n = (tamano_particion-sizeof(super_bloque)) / (4 + sizeof(Inodo) +(3*sizeof(bloque_archivos)) + sizeof(journaling) ); // para ext3

                num_estructuras = floor(n);

                fseek(disco, (particion_actual.part_start -1), SEEK_SET); // posicionarse en el inicio de la particion

                cout<<"donde vas a escribir el super bloque?"<<ftell(disco)<<endl;

                super_bloque sb = Crear_Super_Bloque(num_estructuras, particion_actual.part_start, fs);// Creamos una instancia del superbloque
                fwrite(&sb, sizeof(super_bloque), 1, disco); // escribimos el super_bloque al inicio de la particion

                Formatear_Particion(disco, sb, type);

                /* Ahora se debe Crear la carpeta raiz, que contiene un inodo y users.txt */
                Crear_INODO(disco,'0', 1, -1, 777,0);   // crea el inodo de la raiz (/)
                Journaling("raiz","/",1,"");
                Crear_usersTXT(disco);             // Crea users.txt en la raiz (/)
                Journaling("mkusers","/users.txt",1,"1,G,root\n1,U,root,root,123\n");

             }

      fclose(disco);
    }else {
       cout<<"\nMKFS: Error al abrir el disco\n";
    }
}


#endif // MKFS_H
