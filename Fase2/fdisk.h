#ifndef FDISK_H
#define FDISK_H

#include <iostream>
#include <string.h>
#include "estructuras.h"
#include "raid1.h"

using namespace std;

/*====================================CREACION DE PARTICIONES==============================================================*/
void imprimir_info_particion(partition particion)
{
    printf("*******************************************************\n");
    printf("part_status:%c\n",particion.part_status);
    printf("part_type:%c\n",particion.part_type);
    printf("part_fit:%c\n",particion.part_fit);
    printf("part_start:%d\n",particion.part_start);
    printf("part_size:%d\n",particion.part_size);
    printf("part_name:%s\n",particion.part_name);

    printf("**********************fin******************************\n");

}



int buscar_particion_libre(mbr m) // Busca adentro del mbr si hay alguna particion libre y retorna cual es
{
    for (int i = 0; i < 4; i++) // para iterar sobre las particiones del mbr
    {
        if(m.particiones[i].part_status == '0') // implica que la particion no esta activa
            return i;
    }
 return -1;
}




bool existen_particiones_creadas(mbr m) // Determina si existe alguna particion creada en el mbr
{
    for (int i = 0; i < 4; i++) // para iterar sobre las particiones del mbr
    {
        if(m.particiones[i].part_status != '0') // implica que la particion no esta activa
            return true;
    }
 return false;
}



int calcular_limites_para_particion(mbr m, partition particiones_ordenadas[4], int contador_ordenar, int size) // Determina donde debe iniciar la nueva particion, aplicando su fit(WF,BF,FF)
{

/*para detectar el espacio libre*/
 int lim_inf=0;
 int lim_sup=0;
 int hay_espacio=0;
 /*para saber en que posicion del disco estoy actualmente*/
  int indice = 136 ; /*inicia despues del mbr*/

    for(int i=0; i < contador_ordenar; i++) // para recorrer las particiones_ordenadas
    {
    /*debemos extraer cada particion para ver en donde hay espacios vacios entre cada particion*/

      if( (indice+1) ==  (particiones_ordenadas[i].part_start))
      {/*Esto significa que no hay espacio disponible,entonces hay que seguir buscando y saltar al final de esa particion,*/
            indice = indice + (particiones_ordenadas[i].part_size) ;

      }
      else /*existe un posible espacio libre entre bloques*/
      { /*Esto significa que SI hay un espacio disponible entre indice y un bloque(partition) cualquierda*/

          int fin_espacio_libre = (particiones_ordenadas[i].part_start - 1) ; /*Se coloca un byte antes del inicio de la siguiente posicion*/

          int  espacio_libre = fin_espacio_libre  - indice;
          printf("Cual sera aqui el espacio libre?:%i\n",espacio_libre);

              if(espacio_libre >= size)
              {
                  printf("Espacio libre en medio de 2 particiones \n");

                if(m.disk_fit=='f') // first fit
                {
                    lim_inf = indice;
                    lim_sup = fin_espacio_libre;
                    hay_espacio=1;
                   break; // Salir del For

                }else if(m.disk_fit=='w')// worst fit
                {
                    int espacio_actual = lim_sup - lim_inf ;
                    int nuevo_espacio = fin_espacio_libre - indice;

                    if(nuevo_espacio>espacio_actual)
                    {   printf("Cambio el espacio a uno mas grande ya que tiene el worst fit\n");
                        lim_inf = indice;
                        lim_sup = fin_espacio_libre;
                    }

                }else if(m.disk_fit=='b') // best fit
                {

                    int espacio_actual = lim_sup - lim_inf ;
                    int nuevo_espacio = fin_espacio_libre - indice;
                   // printf("Espacio actual: %i\n",espacio_actual);
                   // printf("Espacio nuevo donde debo cambiar:%i\n",nuevo_espacio);

                    if(espacio_actual==0) /*significa que no ha encontrado ningun espacio para la particion y que el que encontro ahorita, es el que debe agarrar*/
                    {
                        lim_inf = indice;
                        lim_sup = fin_espacio_libre;
                    }
                     else if(nuevo_espacio<espacio_actual)
                    {   printf("Cambio el espacio a uno mas pequeño ya que tiene el best fit\n");
                        lim_inf = indice;
                        lim_sup = fin_espacio_libre;
                    }
                }
              }
        indice = indice+espacio_libre + (particiones_ordenadas[i].part_size);
      }
    }

  /*Al salir de buscar espacio hay 2 posibilidades
   1 -> si indice es == al total del disco, significa que si encontro todos los espacios libres del disco
   2 -> si indice != total de diso, significa que olvido la ultima parte del disco
  */
    if(indice == (m.mbr_tamano)) /*Considero todos los posibles espacios libres existentes..*/
    {
          printf("SI ENCONTRE PARTICION y llegue al final ya que indice == tamaño total del disco\n");
    }
     else/*NO considero todos los espacios libres le falta el ultimo*/
    {
      printf("indice:%d\n",indice);
      printf("tamano_disco:%d\n",m.mbr_tamano);
      printf("FALTO ANALIZAR EL ULTIMO ESPACIO LIBRE \n");

      int espacio_previo =lim_sup - lim_inf ;
      int ultimo_espacio = m.mbr_tamano - indice;

      if(ultimo_espacio >= size) /*implica que puede sustituir al actual espacio_previo seleccionado*/
      {
          switch(m.disk_fit)
          {
              case 'f': /*primer ajuste*/
              {
                  if(!hay_espacio) /*solo puede sustituir al primer ajuste, si este no existe*/
                  {   printf("Ya que no tenia espacio escogio de ultimo el first fit\n");
                      lim_inf = indice;
                      lim_sup = m.mbr_tamano;
                  }
              }break;

              case 'w': /*worst fit*/
              {
                  if(espacio_previo==0)
                  {
                      lim_inf = indice;
                      lim_sup = m.mbr_tamano;

                  }
                   else if(ultimo_espacio>espacio_previo)/*Segun la teoria, debe escoger la de mayor tamaño*/
                  {
                       printf("El ultimo espacio fue escogido por worst fit por tener mayor capacidad \n");
                      lim_inf = indice;
                      lim_sup = m.mbr_tamano;
                  }

              }break;

              case 'b': /*best fit*/
              {
                  if(ultimo_espacio<espacio_previo)/*Segun la teoria debe escoger la de menor tamaño*/
                  {
                      printf("El ultimo espacio fue escogido por best fit por tener menor capacidad\n");
                      lim_inf = indice;
                      lim_sup = m.mbr_tamano;

                  }else if(espacio_previo==0){
                      lim_inf = indice;
                      lim_sup = m.mbr_tamano;
                  }
              }break;

          }
      }

    }
 return lim_inf; /*Este valor indica en donde debe empezar la nueva particion*/
}



partition buscar_espacio_para_particion(mbr m, char type, int size, char fit, string name) /*Busca un espacio para la particion */
{ // SI SE EJECUTA ESTA FUNCION ES PORQUE SI EXISTE UNA PARTICION DISPONIBLE EN EL MBR
    // inicializar una particion en nulo
    partition nuevo = {'0','p','w',0,0,"nulo"};

    partition ordenar[4]; // para poder ver donde inicia la nueva particion debemos de ordenar las particiones del mbr para que sea sencillo.
    int contador_ordenar = 0;

    for (int i = 0; i < 4; i++) // recorrer las particiones internas del mbr
    {
        if(m.particiones[i].part_status == '1') // para solo agregar al vector de ordenar particiones activas
        {
            ordenar[contador_ordenar] = m.particiones[i]; // Aqui se inserta sin ordenar
            contador_ordenar++;
        }
    }

    printf("particiones existentes y ordenadas (4 unicamente)!!\n");

    for(int i =0; i<contador_ordenar; i++) // Aca procederemos a Ordenar el vector 'ordenar'
    {
        for(int j=0; j<(contador_ordenar-1); j++)
        {
            if(ordenar[j].part_start > ordenar[j+1].part_start) /*se debe intercambiar las casillas*/
            {
                 partition aux = ordenar[j];
                 ordenar[j] = ordenar[j+1];
                 ordenar[j+1] = aux;
            }
        }
        //imprimir_info_particion(ordenar[i]);
    }

    switch(type) // el type puede ser: primaria(p), extendida(e) y logica(l)
    {
        case 'p': // particion primaria
        {
        /*Aqui buscamos el inicio de donde debe colocarse la nueva particion, si da 0 entonces no hay espacio*/
           int inicio_nueva_particion = calcular_limites_para_particion(m, ordenar, contador_ordenar, size);

           if(inicio_nueva_particion > 0)
           {/*Aqui llenamos Partition p para luego retornarla en la funcion*/
               nuevo.part_status = '1';
               nuevo.part_type = type;
               nuevo.part_fit = fit;
               nuevo.part_start = inicio_nueva_particion +1 ;
               nuevo.part_size = size;
               strcpy(nuevo.part_name, name.c_str());

           }else {
                puts("Error: No hay espacio en el Disco para crear una nueva particion");
           }

        }break;

        case 'e': // particion extendida
        { /*Buscar que solo exista una extendida y que haya espacio en el mbr para colocarla*/
            bool existe_extendida = false;

            for (int i = 0; i < 4; i++) // recorrer las particiones internas del mbr
            {
                if(m.particiones[i].part_status == '1') // Solo particion que este activo
                {
                    if(m.particiones[i].part_type == 'e') // verificar si existe alguna extendida.
                        existe_extendida = true;
                }
            }

            if(!existe_extendida)
            { // procedemos a buscar los limites para crear la particion
                int inicio_nueva_particion = calcular_limites_para_particion(m, ordenar, contador_ordenar, size);

                if(inicio_nueva_particion > 0)
                {/*Aqui llenamos Partition p para luego retornarla en la funcion*/
                    nuevo.part_status = '1';
                    nuevo.part_type = type;
                    nuevo.part_fit = fit;
                    nuevo.part_start = inicio_nueva_particion +1 ;
                    nuevo.part_size = size;
                    strcpy(nuevo.part_name, name.c_str());

                }else {
                     puts("Error: No hay espacio en el Disco para crear una nueva particion(extendida)");
                }
            }else
            {
                puts("Error: La particion (extendida) no pudo ser creada, porque ya existe una");
            }

        }break;
    }

 return nuevo;
}




void Crear_Primaria_O_extendida(int size, string path, string name, char unit, char type, char fit)
{
 /*paso 1: de *ruta ver si existe el disco y extraer su mbr para verificar que haya espacio para crear la particion*/
    FILE *disco;
          disco = fopen(path.c_str(), "r+b"); // abre para lectura y escritura al inicio del fichero
    if(disco != NULL)
    {
        mbr aux;
        int mbr_leido = fread(&aux,sizeof(aux),1,disco);

        if(mbr_leido==1)
        {
            // Determinar el tamaño total de la particion
            if(unit == 'k')
                size = size*1024;
            else if(unit=='m')
                size = size*(1024*1024);

    /*ahora debemos verificar que el tamaño que se le quiere colocar a la particion no sobrepase el tamaño del disco*/
            int tamano_disco = aux.mbr_tamano-sizeof(aux);
            printf("Comparando tamaños en crear particion tamaño requerido:%d , tamaño del disco:%i\n",size,tamano_disco);
            if(size <= tamano_disco )
            {
                if(existen_particiones_creadas(aux)) // si existen particiones entra aqui
                {
                    int num_particion_libre = buscar_particion_libre(aux);

                    if(num_particion_libre != -1) // implica que si hay una particion libre en el mbr
                    {
                        cout<<"La particion libre es ->"<<num_particion_libre<<endl;
                        partition p = buscar_espacio_para_particion(aux,type,size,fit,name); // Crea y busca el espacio de una nueva particion

                        if(p.part_status == '1') // implica que si se pudo crear la particion
                        { //ya solo se debe insertar la nueva particion en el espacio para particiones del MBR
                            puts("fdisk: Insersion correcta de nueva particion");
                            aux.particiones[num_particion_libre] = p;
                            rewind(disco);
                            fwrite(&aux, sizeof(mbr), 1 , disco);

                            FDISK_RAID1(aux, path); // grabar los cambios en el RAID1
                        }

                    }else {
                        puts("Error en fisk:El disco ya no tiene espacio libre para crear Particiones");
                    }

                }else{ // es la primera particion la cual se creara

                        partition p;
                                  p.part_status = '1';
                                  p.part_type = type;
                                  p.part_fit = fit;
                                  p.part_start = sizeof(mbr)+1; /*donde inicia este disco en el byte -> 137*/
                                  p.part_size = size;
                            strcpy(p.part_name,name.c_str());

                        aux.particiones[0] = p; // insertamos la nueva particion al mbr
                        rewind(disco); // rebobinamos el disco
                        fwrite(&aux, sizeof(mbr), 1 , disco);

                        FDISK_RAID1(aux, path); // grabar los cambios en el RAID1
                }
            }
            else
              puts("Error, el tamaño de la particion que desea crear, supera el tamaño del disco");


        }else
            printf("Error en la lectura del mbr: %i\n",mbr_leido);

      fclose(disco);
    }else {
        printf("Error, no se pudo abrir el Disco de la ruta proporcionada pueda que no exista \n");
    }
}


int buscar_extendida(mbr m) // busca adentro del mbr si existe una particion extendida Y RETORNA SU POSICION sino (-1)
{
    for (int i = 0; i < 4; i++)  // BUSCAR SI EXISTE UNA PARTICION EXTENDIDA
    {
        if(m.particiones[i].part_status == '1') // solo particiones activas
        {
            if(m.particiones[i].part_type == 'e') // busca si es extendida
                return i; // RETORNA LA POSICION DONDE ENCONTRO LA EXTENDIDA
        }
    }

 return -1;
}

/*
void recorrer_ebr(FILE *disco, partition extendida,string name, char type, char fit )// recorre el ebr de una particion extendida para saber si se puede colocar la particion logica
{
// EL EBR ES COMO UNA LISTA ENLAZADA ENTONCES SOLO SE DEBE RECORRER

    int inicio_extendida = extendida.part_start;


    fseek(disco, inicio_extendida-1, SEEK_SET);//mover puntero del disco a la posicion donde inicia la particion extendida

    ebr e;
    fread(&e, sizeof(ebr),1, disco );


    if(e.part_status == '1') // implica que la primera particion logica ya fue creada
    {

    }else{ // implica que la primera particion logica NO ha sido creada



    }



}


void Crear_logica(int size, string path, string name, char unit, char type, char fit)
{
    FILE *disco = fopen(path.c_str(), "r+b"); // ABRIR EL DISCO

    if(disco != NULL){

        mbr mbr_aux;
        int mbr_leido = fread(&mbr_aux, sizeof(mbr_aux), 1, disco);


        if(mbr_leido==1){

            int pos = buscar_extendida(mbr_aux); // devuelve la posicion donde esta la particion extendida
            if(pos != -1)
            {
                partition extendida = mbr_aux.particiones[pos]; // sacamos la particion extendida del mbr
                int tamano_total_extendida = extendida.part_size - sizeof(ebr); // Determinar el tamaño que se puede utilizar de la extendida

                // Determinar el tamaño total de la particion logica que se desea crear
                if(unit == 'k')
                    size = size*1024;
                else if(unit=='m')
                    size = size*(1024*1024);

                if(size <= tamano_total_extendida) // Logica no debe superar el tamaño de la extendida
                {
                    //recorer ebr de extendida para colocar la particion
                   //  recorrer_ebr(disco, extendida, name, type, fit); QUEDO Inconcluso

                }else{
                    cout<<"fdisk: Error, El tamaño de la particion (logica) supera el tamaño de la (extendida)\n";
                }

            }else{
                cout<<"fdisk: Error, NO existe particion (extendida) por lo tanto no se puede crear la particion logica\n";
            }

        }else{
            cout<<"fdisk: Error al leer el mbr en 'crear_logica()'\n";
        }
      fclose(disco);
    }else {
        cout<<"fdisk: Error al abrir el Disco puede ser que no exista..\n";
    }
}*/


/*===================================FIN CREACION DE PARTICIONES========================================================*/



/*=====================================BORRADO DE PARTICIONES===========================================================*/
void Borrar_particion(string path, char Delete, string name)
{
    FILE *disco;
          disco = fopen(path.c_str(),"r+b"); // abre para lectura y escritura al inicio del fichero
    if(disco != NULL)
    {
        mbr mbr_aux ;
        int mbr_leido;
            mbr_leido = fread(&mbr_aux, sizeof(mbr_aux), 1, disco);

        if(mbr_leido == 1)
        {
            // BUSCAR EN LAS PARTICIONES DEL MBR, LA CUAL COINCIDA CON EL NOMBRE DE LA PARTICION QUE DESEAMOS ELIMINAR
            bool encontro_particion = false; // para verificar si la particion que se desea eliminar existe.
            for(int i=0; i<4; i++)
            {
                if(mbr_aux.particiones[i].part_status == '1') // solo verificar particiones activas.
                {
                    if( strcmp(mbr_aux.particiones[i].part_name, name.c_str()) == 0)
                    {
                        cout<<"\ncomparando:"<<mbr_aux.particiones[i].part_name<<"=="<<name<<endl;
                        encontro_particion = true;
                        printf("Esta seguro que desea eliminar la particion, S|N: ");
                        int c = getchar();

                        while(getchar() != '\n'){} // para limpiar la entrada del usuario

                        if( (c==83) || (c==115)) // c == si
                        {
                            if(Delete == 'r') /*borrado:fast*/
                            {
                                printf("La particion:%s, ha sido eliminada con fast\n",mbr_aux.particiones[i].part_name);
                                 mbr_aux.particiones[i].part_status = '0' ;
                                 rewind(disco);
                                 fwrite(&mbr_aux,sizeof(mbr),1,disco); /*Reescribimos el mbr con la nueva informacion*/
                                 imprimir_info_particion(mbr_aux.particiones[i]); // imprimimos la info de la que se acaba de borrar

                                 FDISK_RAID1(mbr_aux, path); // grabar los cambios en el RAID1
                                break;
                            }
                            else if(Delete == 'c') /*borrado:full*/
                            {
                                printf("La particion:%s, ha sido eliminada con full\n",mbr_aux.particiones[i].part_name);
                                mbr_aux.particiones[i].part_status = '0' ;
                                rewind(disco);
                                fwrite(&mbr_aux,sizeof(mbr),1,disco);
                                FDISK_RAID1(mbr_aux, path); // grabar los cambios en el RAID1
                                break;
                            }
                        }
                    }
                }
            }
            if(!encontro_particion){
              printf("fdisk: Error, La particion que desea eliminar no existe en el disco!\n");
            }
        }


        fclose(disco);
    }else
    {
        puts("Error en borrar_particion: No se puede abrir el disco");
    }


}
/*===================================FIN BORRADO DE PARTICIONES========================================================*/




/*=====================================REDIMENSIONAMIENTO DE PARTICIONES===========================================================*/
int existe_espacio_paraAumentar(mbr m, string name_partcion_aModificar) // funcion que determina si una particion puede incrementar en tamaño
{
    // ORDENAR LAS PARTICIONES YA QUE EN EL MBR (m) ES MUY PROBABLE QUE ESTE DESORDENADO

    partition ordenar[4]; // para ordenar las particiones activas
    int contador_ordenar = 0;

    // INSERTAR PARTICIONES ACTIVAS AL VECTOR ordenar
    for (int i = 0; i < 4; i++) // recorrer las particiones internas del mbr(m)
    {
        if(m.particiones[i].part_status == '1') // para solo agregar al vector de ordenar particiones activas
        {
            ordenar[contador_ordenar] = m.particiones[i]; // Aqui se inserta sin ordenar
            contador_ordenar++;
        }
    }

    // ORDENAR LAS PARTICIONES QUE ESTEN EN EL VECTOR 'ordenar'
    for(int i =0; i<contador_ordenar; i++) // Aca procederemos a Ordenar el vector 'ordenar'
    {
      for(int j=0; j<(contador_ordenar-1); j++)
      {

        if(ordenar[j].part_start > ordenar[j+1].part_start) /*se debe intercambiar las casillas*/
        {
             partition aux = ordenar[j];
             ordenar[j] = ordenar[j+1];
             ordenar[j+1] = aux;
        }
      }
      //imprimir_info_particion(ordenar[i]);
    }

    int tamano_total_del_disco = m.mbr_tamano;

    for(int i =0; i < contador_ordenar; i++) // Recorrer el vector ordenadamente
    {
        if(ordenar[i].part_name == name_partcion_aModificar) //Buscar solo la particion que se quiere modificar
        {
            int tamano_actual = ordenar[i].part_start + ordenar[i].part_size;

            if( (i+1) == contador_ordenar ) // Ingresa si la particion ya NO tiene otra al lado derecho
            {
                return tamano_total_del_disco - tamano_actual;

            }else // ingresa SI tiene una particion al lado derecho
            {
               if( tamano_actual == ordenar[i+1].part_start) // implica que no se puede agregar mas espacio
               {
                   return 0;
               }else{ // si se puede aumentar el tamaño de la particion

                   return ordenar[i+1].part_start - tamano_actual; // lo que se puede aumentar es la diferencia entre lado derecho - izquierdo
               }
            }
        }
    }
return 0;
}


void Redimencionar_particion(string path, char unit, int add, string name, char operacion)
{
    puts("Fdisk: Intentando redimencionar una particion");
    FILE *disco = fopen(path.c_str(),"r+b"); // abre para lectura y escritura al inicio del fichero

    if(disco != NULL)
    {
        mbr mbr_aux;
        rewind(disco);
        int mbr_leido = fread(&mbr_aux,sizeof(mbr_aux), 1, disco);
        int encontro_particion = false;

        if(mbr_leido == 1)
        {
            for(int i=0; i<4; i++) // recorrido de particiones internas del mbr
            {
                if(mbr_aux.particiones[i].part_status == '1') // solo particiones activas
                {
                    if(strcmp(mbr_aux.particiones[i].part_name,name.c_str())==0) /*si da 0 significa que encontro la particion*/
                    {
                        encontro_particion = true;
                        switch(operacion)
                        {
                            case '-': /*significa que se quiere reducir el tamaño de la particion*/
                            {
                                int capacidad = mbr_aux.particiones[i].part_size;
                                int espacio_aReducir = add;

                                 if(unit == 'k')
                                 {espacio_aReducir = espacio_aReducir*1024;}
                                 else if(unit == 'm')
                                 {espacio_aReducir = espacio_aReducir*(1024*1024);}

                                  if(capacidad > espacio_aReducir) // verificar que no se quiera reducir mas espacio de la que existe
                                  {
                                      printf("El espacio de la particion:%s,fue reducida\n",name.c_str());

                                      int nuevo_part_size = (mbr_aux.particiones[i].part_size) - (espacio_aReducir);
                                      mbr_aux.particiones[i].part_size = nuevo_part_size; /*colocamos su nuevo espacio reducido*/
                                      rewind(disco);
                                      fwrite(&mbr_aux,sizeof(mbr),1,disco); /*Reescribimos el mbr con la nueva informacion*/

                                      FDISK_RAID1(mbr_aux, path); // grabar los cambios en el RAID1
                                  }
                                  else
                                     printf("Error, el espacio a reducir es mayor a la capacidad de la particion\n");

                            }break;

                            case '+': /*significa que se quiere aumentar el tamaño de la particion*/
                            {
                                int espacio_aAumentar = add;

                                if(unit == 'k')
                                {espacio_aAumentar = espacio_aAumentar*1024;}
                                else if(unit == 'm')
                                {espacio_aAumentar = espacio_aAumentar*(1024*1024);}


                                int capacidad = existe_espacio_paraAumentar(mbr_aux, name); //determina cuanto espacio hay hacia la derecha
                                cout<<"val de capacidad:"<<capacidad<<endl;

                                if(capacidad >= espacio_aAumentar)
                                {
                                    cout<<"fdisk: Si se puede aumentar de tamaño la particion\n";
                                    int nuevo_part_size = mbr_aux.particiones[i].part_size + espacio_aAumentar;
                                    mbr_aux.particiones[i].part_size = nuevo_part_size; //Colocamos su nuevo espacio aumentado
                                    rewind(disco);
                                    fwrite(&mbr_aux,sizeof(mbr),1,disco); //Reescribimos el mbr con la nueva informacion

                                    FDISK_RAID1(mbr_aux, path); // grabar los cambios en el RAID1
                                }else
                                {
                                    cout<<"fdisk: No hay espacio para aumentar tamaño la partcion\n";
                                    cout<<"fdisk: Espacio libre = "<<capacidad<<"(bytes), Espacio que se desea aumentar ="<<espacio_aAumentar<<"(bytes)\n";

                                }
                            }break;

                        }

                     break; // break del for
                    }
                }
            }
            if(!encontro_particion)
            {
                cout<<"Fdisk: Error-> La Particion con el 'name="<<name<<"' NO EXISTE en el Disco\n";
            }
        }

        fclose(disco);
    }else
    {
        puts("fdisk: Error no se pudo cargar el Disco para redimensionar, puede ser que no existe");
    }
}

/*===================================FIN REDIMENSIONAMIENTO DE PARTICIONES========================================================*/





void Instruccion_FDISK(char operacion,int size, string path, char type, char fit, char Delete, string name, int add, char unit)
{
 // lo que se puede realizar con fdisk es: Crear particiones, Borrar particion, Agregar espacio, Quitar espacio
    cout<<size<<","<<path<<","<<type<<","<<fit<<","<<Delete<<","<<name<<","<<add<<","<<unit<<endl;

    if(size>0 && path!="" && name!=""){ /*los parametros obligatorios para poder formatear el disco con particiones*/

        if( (type == 'p') || (type == 'e')) // Solo primarias o extendidas
        {
            Crear_Primaria_O_extendida(size, path, name, unit, type, fit);

        }else { // Solo logicas
           // Crear_logica(size, path, name, unit, type, fit);
        }


    }else if(path!="" && Delete!='\0' && name!=""){ /*obligatorios para poder borrar una particion*/

        Borrar_particion(path, Delete, name);

    }else if(path!="" && add > 0 && name!=""){ /*obligatorios para poder agregar o quitar espacio de una particion*/

        Redimencionar_particion(path, unit, add, name, operacion);
    }
     else
         printf("Error interno en FDISK: los valores de los parametros son erroneos o faltan parametros\n");

}


#endif // FDISK_H



