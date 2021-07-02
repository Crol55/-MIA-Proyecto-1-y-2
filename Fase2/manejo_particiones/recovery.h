#ifndef RECOVERY_H
#define RECOVERY_H


#include <iostream>
#include "../lista_enlazada.h" // incluye estructuras.h
#include <string.h> // para strcmp
#include "manejo_particiones/mkfs.h"
/*====== Prototipos =========*/
/*==EXTERNOS===*/
partition get_partition(string nombre_particion, mbr m); // compilada en "getterysetter_globales.h"
void MKDIR_RECOVERY(string path);  // compilada en "mkdir.h"
void MKFILE(string path, char p, int size, string cont, bool recovery_mode, int propietario); // compilada en "mkfile.h"
void Crear_INODO(FILE *disco, char i_type, int id_propietario,int i_size, int i_perm, int pos_inodo_padre); // compilada en "op_inodos.h"
/*==INTERNOS*/
void ejecutar_operacion(string ruta_disco, journaling journal);
/*======= FIN =============*/


string CONCAT="";
string RUTA_ = "";
int p_recovery = -1;

void RECOVERY(std::string id)
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

        // INICIAR EL RECOVERY POR MEDIO DEL JOURNALING
        int inicio_journaling = particion_.part_start + sizeof(super_bloque);
        int fin____journaling = sb.s_inodes_count;

        journaling journal_aux;
        fseek(disco, inicio_journaling-1, SEEK_SET); //Mover el puntero del disco al inicio del journaling

        for (int i = 0; i < fin____journaling; i++) // revision de esta mieshda
        {
            fread(&journal_aux, sizeof(journaling), 1, disco); //la lectura se hara secuencial por medio del for
            //long puntero_actual = ftell(disco); // se debe almacenar donde esta el puntero actualmente ya que en 'ejecutar_operacion' ese puntero se mueve

            if(strcmp(journal_aux.operacion, "")!=0) // Significa que si existe informacion de journaling
            {
                    cout<<"1)"<<journal_aux.operacion<<endl;
                    cout<<"2)"<<journal_aux.nombre<<endl;
                    cout<<"3)"<<journal_aux.propietario<<endl;
                    ejecutar_operacion(ruta_disco, journal_aux); // ejecuta la operacion para restaurar el sistema de archivos (El puntero se mueve)


            }else{ // el journaling esta vacio en cierto punto

                if(CONCAT != "") // implica que hay un mkfile pendiente de realizar
                {
                    MKFILE(RUTA_,'1',0, CONCAT, true, p_recovery); // recupera el archivo con todo su contenido
                    CONCAT = "";
                    RUTA_  = "";
                }
                break; // salir del journaling ya que no hay mas que leer del journaling
            }

        }

      fclose(disco);
    }else{
        cout<<"Recovery: Error, la particion con el id="<<id<<", no existe en el disco\n";
    }

}

void ejecutar_operacion(string ruta_disco, journaling journal) // leera la porcion de journaling enviada y ejecutara lo que dice internamente para recuperar el sistema
{
    if(strcmp(journal.operacion, "raiz") == 0)
    {
        FILE *disco = fopen(ruta_disco.c_str(), "r+b");
        Crear_INODO(disco, '0',journal.propietario,0, 777,0); //Aqui mueve el puntero del disco por lo tanto hay que regresarle su valor
        fclose(disco);

    }else if(strcmp(journal.operacion, "mkusers") == 0){

        if(CONCAT != "") // implica que hay un mkfile pendiente de realizar
        {
            MKFILE(RUTA_,'1',0, CONCAT, true, p_recovery); // recupera el archivo con todo su contenido
            CONCAT = "";
            RUTA_  = "";
        }
        FILE *disco = fopen(ruta_disco.c_str(), "r+b");
        Crear_usersTXT(disco);
        fclose(disco);

    }else if(strcmp(journal.operacion, "mkdir") == 0){

       if(CONCAT != "") // implica que hay un mkfile pendiente de realizar
       {
           MKFILE(RUTA_,'1',0, CONCAT, true,p_recovery); // recupera el archivo con todo su contenido
           CONCAT = "";
           RUTA_  = "";
       }
       std::string path(journal.nombre);
       MKDIR_RECOVERY(path);

    }else if(strcmp(journal.operacion, "mkfile") == 0)
    {
        // al ejecutar mkfile, el siguiente journaling puede contener el contenido del actual mkfile asi que hay sacarle el contenido para restaurarlo
        CONCAT = journal.contenido;
        RUTA_  = journal.nombre;
        p_recovery = journal.propietario;

    }else if(strcmp(journal.operacion, "contenido") == 0)
    {
       cout<<"Estoy ingresandoo ea fcontenido?";
       CONCAT.append(journal.contenido);
    }
}
#endif // RECOVERY_H
