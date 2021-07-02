#ifndef LISTA_ENLAZADA_H
#define LISTA_ENLAZADA_H


#include <iostream>
#include "estructuras.h"


using namespace std;


/*ESTA LISTA SE UTILIZARA PARA MONTAR PARTICIONES*/
//===== Variables GLOBALES, Y TODAS LAS CLASES DONDE SE INCLUYAN SE PUEDEN USAR =====
nodo *inicio = NULL;
nodo *ultimo = NULL;
// ========================================================================
void agregar(string path, string name)
{

    char letra = 'a'; /*Iniciamos para ver que letra le corresponde a cada disco*/
    int contador=49; /*Para saber el numero de particion que se le pondra al momento de montar inicializado en 1 == 49 */

      if(inicio!=NULL) /*la lista NO esta vacia*/
      {
          nodo *current = inicio;
          bool existe_disco = false;
          bool particion_repetida = false;

          while(current != NULL)
          {
              /*hay que buscar si una particion del mismo disco ya esta montada, si es asi, el nuevo toma el valor de su letra
               * y aumenta el contador en la unidad, si no encuentra un disco ya montado, la letra aumenta y el contador queda igual
              */
              if(current->ruta == path) /*busca si una particion del disco ya esta montada*/
              {
                  if(current->nombre_particion != name) // verificar que esa particion no este ya montada
                  {
                      existe_disco = true;
                      letra = current->id[2]; /*Extraemos la letra de ese disco*/
                      if(contador >= current->id[3]) // el contador debe ser >= para incrementar contador
                      {
                          contador = current->id[3];
                          contador ++;
                      }
                  }else { // la particion ya esta montada
                      particion_repetida = true;
                      cout<<"Mount: Error, la particion con \" name="<< name <<"\" ya fue montada.\n";
                      break;
                  }
              }else
              {
                  if(!existe_disco) /*Si el disco no existe en la lista de montaje entonces si aumenta la letra*/
                  {
                    if(letra >= current->id[2])
                    {
                        letra = current->id[2];
                        ++letra; /*y le aumentamos para que cambie ya que puede ser otro disco*/
                    }

                  }
              }
            current = current->siguiente;
          }

          if(!particion_repetida) // Puede montar si la particion con el mismo nombre no fue previamente montado.
          {
              nodo *nuevo = new nodo();
              char *identificador = (char*) malloc(5); /*5 bytes para 5 caracteres*/
                    identificador[0] = 'v';
                    identificador[1] = 'd';
                    identificador[2] = letra;
                    identificador[3] = contador;
                    identificador[4] = '\0';

                  nuevo->id = string(identificador);
                  nuevo->ruta = path;
                  nuevo->nombre_particion = name;
                  nuevo->siguiente = NULL;
                  ultimo->siguiente = nuevo;
                  ultimo = nuevo; /*movemos el ultimo de posicion*/
          }

      }else
      {
        nodo *nuevo = new nodo();

        char *identificador = (char*)malloc(5); /*5 bytes para 5 caracteres*/
                identificador[0] = 'v';
                identificador[1] = 'd';
                identificador[2] = letra;
                identificador[3] = contador;
                identificador[4] = '\0';

              nuevo->id = string(identificador); // cast char*  a string
              nuevo->ruta = path;
              nuevo->nombre_particion = name;
              nuevo->siguiente = NULL;
              inicio= nuevo;
              ultimo = inicio;

      }

}


void eliminar(string id_montaje) // elimina de la lista enlazada por medio de su id
{
    nodo *anterior = inicio;
    nodo *actual = inicio;
    bool elimino = false;

    if(actual != NULL)
    {
      while(actual != NULL)
      {
          if(actual->id == id_montaje)
          {
            elimino =true;
            anterior->siguiente = actual->siguiente;
            // verificar si el que se esta eliminando es: 'inicio' o 'ultimo' ya que sino dara segmentation fault
            if(actual == inicio)
                inicio = actual->siguiente;
            if(actual == ultimo)
                ultimo = anterior;

          }
          anterior = actual; // antes de saltar a la siguiente memoria hay que guardarlo como anterior.
          actual = actual->siguiente;
      }
      // Si al salir de aqui actual es nulo implica que no existe el id_montaje que se desea eliminar
      if(! elimino)
        cout<<"Unmount: Error, La particion que desea desmontar con el \" id="<<id_montaje<<"\", NO existe\n";
      else
        cout<<"Unmount: Exito, Se desmonto exitosamente la particion con el \" id="<<id_montaje<<"\".\n"    ;
    }else {
      std::cout<<"Unmount: Error, no hay particiones montadas";
    }
}


void mostrar_particiones_montadas()
{
    nodo *aux = inicio;
  printf("********mostrando particiones montadas*********\n");
     while(aux != NULL)
     {
        cout<<aux->id<<endl;

        aux = aux->siguiente;
     }
  printf("**********Fin particiones montadas**********\n");
}



string get_ruta(string id_disco) // funcion que busca en la lista el disco montado con el id_disco, y retorna su ruta
{
    string ruta_encontrada = "";
    nodo *aux = inicio;

       while(aux != NULL) /*recorrer todas las particiones hasta encontrar la que coincida con id_disco*/
       {
              if(aux->id == id_disco)
              {
                  return aux->ruta;
              }
          aux = aux->siguiente;
       }

 return ruta_encontrada;
}


string get_nombre_particion(string id_montaje) /*Devuele el nombre de la particion que coincida con el 'id_montaje' */
{ // id_montaje = vd+letra+num

    nodo *aux = inicio;

    while(aux != NULL)
    {
           if(aux->id == id_montaje)
           {
               return aux->nombre_particion;
           }
       aux = aux->siguiente;
    }
 return ""; /*Si llega hasta aqui implica que el id_montaje no existe*/
}

#endif // LISTA_ENLAZADA_H
