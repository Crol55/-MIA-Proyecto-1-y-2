#ifndef JOURNALING_H
#define JOURNALING_H


#include <iostream>
#include "getterysetter_globales.h" // incluye 'estructuras.h'
#include <string.h> // para strcpy

using namespace std;


int primer_libre_journaling(int inicio_journaling, FILE *disco, int fin_journaling) // recorre el journaling y determina en donde hay un espacio vacio
{
 // Recorrer el journaling para buscar una posicion vacia
    journaling j_aux;// para ver que parte del journal esta vacio
    //Mover el puntero del disco al inicio del journaling
    fseek(disco, inicio_journaling-1, SEEK_SET);
    int contador=0;
    for (int i = 0; i < fin_journaling; i++)
    {
        fread(&j_aux, sizeof(journaling),1, disco); //la lectura se hara secuencial por medio del for
        //cout<<"Que tiene operacion adentro->"<<j_aux.operacion<<".\n";
        if(strcmp(j_aux.operacion, "")!=0) // Significa que ese espacio de journaling esta ocupado
        {
            contador++; //
          //  cout<<"Encontro un espacio ocupado_j\n";

        }else{ // el journaling esta vacio en cierto punto
           // cout<<"Encontro un espacio libre_j\n";
            break; // rompe el for para que retorne contador

        }
    }
 return contador; // retorna donde encontro un espacio vacio en el journaling
}



void Journaling(string tipo_operacion, string ruta, int propietario, string contenido)
{
    super_bloque sb = get_super_bloque(); // extraemos el super bloque

    if(sb.s_filesystem_type == 3) // entonces si podemos usar el journaling que es propio de ext3
    {
    //1) Crear el objeto journaling
        journaling journal;
        strcpy(journal.operacion, tipo_operacion.c_str());
        strcpy(journal.nombre, ruta.c_str());
        strcpy(journal.contenido, contenido.c_str());
            journal.propietario = propietario;


       //2) Insertar el objeto journaling en el disco
       int inicio_jornaling = particion_global.part_start + sizeof(super_bloque);
       cout<<"El inicio del journaling esta:"<<inicio_jornaling<<endl;

       FILE *disco;
             disco = fopen(ruta_disco_global.c_str(), "r+b");


       if(disco != NULL)
       {
           rewind(disco); //rebobinar

           int fin_journaling = sb.s_bm_inode_start-1; // el fin de journaling, es una posicion anterior al inicio del bm_inodos

           int primer_journaling_libre =  primer_libre_journaling(inicio_jornaling, disco, fin_journaling);

           if(primer_journaling_libre != fin_journaling-1) // verificar que no se haya llegado al final del journaling
           {
               int mov = inicio_jornaling + (primer_journaling_libre* sizeof(journaling)); // para moverse donde debe colocar el objeto journaling
                cout<<"Escribiendo journaling en la posicion:"<<mov<<endl;
               fseek(disco, mov -1, SEEK_SET); // mueve el puntero hacia donde hay que colocar el journaling
               fwrite(&journal, sizeof(journal), 1, disco);

           }else{
                cout<<"Journaling: Error, ya no hay mas espacio en el journaling\n";
           }

           fclose(disco);

       }else{
            cout<<"Journaling: Error al abrir el disco\n";
       }
    }
}

#endif // JOURNALING_H
