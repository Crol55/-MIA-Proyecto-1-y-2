#ifndef GETTERYSETTER_GLOBALES_H
#define GETTERYSETTER_GLOBALES_H

#include "estructuras.h"
#include <iostream>
#include <vector> // utilizado en split, para separar cadenas de texto(string)


using namespace std;

partition particion_global ;
string ruta_disco_global;      // testeado usado en(mkgrp)

string usuario_global ="";     // testeado usado en (mkgrp) - se inicializa en login
string grupo_global = "";      // Se inicializa en login
int    id_propietario_g = -1; // se inicializa en login

time_t get_tiempo()
{
   time_t current_time;

    current_time = time(NULL);


    if (current_time == ((time_t)-1))
    {
        (void) fprintf(stderr, "Fallo al obtener el tiempo.\n");
        exit(EXIT_FAILURE);
    }
  return current_time;
}



partition get_partition(string nombre_particion, mbr m)
{
   /*Busca en todas las particiones del mbr, la que contenga el "part_name == nombre_particion" */
    partition partition_ret ;

    for(int i=0; i<4; i++) /*ahora hay que buscar la particion que coincida con el nombre*/
    {
        if(m.particiones[i].part_name == nombre_particion)
        {
            partition_ret = m.particiones[i];

            break; /*break para el for*/
        }

    }

    return partition_ret;
}


super_bloque get_super_bloque() // devuelve el super_bloque actual
{
    super_bloque sb;
    FILE *disco = fopen(ruta_disco_global.c_str(), "r+b");

    if(disco != NULL)
    {
        int inicio = particion_global.part_start;// inicio de la particion en la que actualmente se esta trabajando
        fseek(disco,inicio-1,SEEK_SET); // nos colocamos donde inicia la particion
        fread(&sb, sizeof(super_bloque), 1, disco);

     fclose(disco);
    }else{
        puts("Error en 'get_super_bloque'\n");
    }
 return sb;
}



Inodo get_usersTXT(FILE *disco, partition particion_actual) // utilizado en "admin_usuario.h"
{
    Inodo inodo_fallido; // este inodo se retorna al final, pero si llega hasta el final significa que no encontro el inodo users.txt
          inodo_fallido.i_type = 'E'; // 'E' representa ->Error


     // 1) Obtener el super_bloque para saber en donde buscar el bloque que contiene 'users.txt'
    fseek(disco, (particion_actual.part_start - 1), SEEK_SET);
    super_bloque sb;
    fread(&sb, sizeof(super_bloque), 1, disco); // obtenemos el super_bloque


    if(sb.s_filesystem_type > 0) // significa que la particion a la que se quiere acceder tiene un sistema de archivos
    {
        // 2) Acceder al inodo 'users.txt'.. sabemos que ese inodo esta en la posicion 1
        int inicio_inodos = sb.s_inode_start;

        int pos = inicio_inodos + (1 * sizeof(Inodo) ); // En esta posicion del disco esta inodo 'users.txt'

        fseek(disco, (pos -1), SEEK_SET); // nos colocamos en la posicion donde esta el inodo 'users.txt'

        Inodo i;
        fread(&i, sizeof(Inodo), 1, disco); // leemos el inodo en la posicion indicada por 'pos'

      return i; /*RETORNA ti a la funcion en donde fue llamada*/

    }else{
        printf("Error: La particion a la que se le quiere hacer login no tiene un sistema de archivos('mkfs')\n");
    }

 return inodo_fallido;
}




vector<string> split(string texto, char delimitador) // funcion que separa un string por sus caracteres
{
    int tamano = texto.length();
    string nuevo_texto = ""; // para concatenar
    vector<string> separador;

    for(int i =0; i < tamano; i++)
    {
        if((texto[i] != delimitador))
        {
            nuevo_texto += texto[i]; // concatenamos
        }else{ // encontro el delimitador

            if(nuevo_texto != "")
            {
                separador.push_back(nuevo_texto);
                nuevo_texto = ""; // limpiar para seguir concatenando
            }
        }
        if(i == tamano -1)
        {
            separador.push_back(nuevo_texto);
        }
    }
 return separador;
}


string get_grupo(string registro) // utilizado en login
{ /*Funcion que devuelve el grupo de un registro de tipo usuario*/

    vector <string> reg_split = split(registro, ','); // separa todo el registro separado por
    // El grupo estara en la posicion 2 del vector
    return reg_split[2];
}



string get_fecha(time_t tiempo) // extrae la fecha de la variable tiempo
{
    string fecha;
    tm* t = localtime(&tiempo);

    fecha.append(to_string(t->tm_mday) + "/");
    fecha.append(to_string(t->tm_mon+1) + "/");
    fecha.append( to_string(t->tm_year + 1900) );

    return fecha;
}


string get_hora(time_t tiempo)
{
    string hora;
    tm* t = localtime(&tiempo);
    hora.append(to_string(t->tm_hour) + ":" + to_string(t->tm_min));

    return hora;
}
#endif // GETTERYSETTER_GLOBALES_H
