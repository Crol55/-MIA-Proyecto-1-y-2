#ifndef MOUNT_H
#define MOUNT_H

#include <iostream>
#include <string.h> //strcmp
#include "estructuras.h"
#include "lista_enlazada.h"

using namespace std;

void Instruccion_MOUNT(string path, string name)
{

puts("Mount: Intentando montar particion....");

    FILE *disco;
          disco = fopen(path.c_str(),"r+b"); // abre para lectura y escritura al inicio del fichero

     if(disco != NULL)
     {
        mbr aux; /*para sacar el mbr del disco*/

        int mbr_leido = fread(&aux,sizeof(aux), 1, disco);
            //fseek(disco, 0, SEEK_SET);

            if(mbr_leido==1)
            {
                /*Ahora debemos buscar entre todas las particiones la que coincida con el part_name*/
                bool existe_particion = false;

                for(int i=0; i<4; i++)
                {
                    if(aux.particiones[i].part_status=='1') /*primero verificar que si este la particion activa*/
                    {
                        if(strcmp(aux.particiones[i].part_name , name.c_str()) == 0) /*Significa que si encontro la particion que se quiere montar*/
                        {
                            existe_particion = true;
                            break; /*ya que la encontro se puede salir del for*/
                        }
                    }

                }

                if(existe_particion) /*Entonces si puedo montar la particion*/
                {
                    agregar(path, name); /*metodo de lista_enlazada.h*/
                    mostrar_particiones_montadas(); /*metodo de lista_enlazada.h*/
                }else{
                    cout<<"Mount: ERROR, La particion con '-name="<<name<<"',no existe en el Disco\n";
                }

            }
        fclose(disco);
     }else{
        printf("Mount: Error al abrir el disco, puede ser que este no exista\n");
     }

}



#endif // MOUNT_H
