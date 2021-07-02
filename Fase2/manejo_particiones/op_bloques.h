#ifndef OP_BLOQUES_H
#define OP_BLOQUES_H

#include <iostream>
#include "string.h" // strcpy etc...
#include "getterysetter_globales.h"    // Este header ya incluye -> 'estructuras.h' 'vect.h'


bloque_archivos get_bloque_archivo(int pos, string ruta_disco) /*Busca y devuelve un bloque de archivos buscandolo en la posicion inidicada por 'pos'*/
{
    FILE *disco = fopen(ruta_disco.c_str(), "r+b");
    bloque_archivos b_archivo;

    if(disco != NULL)
    {
     //1) extraer el super_bloque de la particion global(particion en la cual se esta trabajando actualmente)
        fseek(disco, (particion_global.part_start - 1), SEEK_SET);
        super_bloque sb;
        fread(&sb, sizeof(super_bloque), 1, disco); // obtenemos el super_bloque

     //2) posicionarse donde comienzan los bloques y moverse con pos
          int inicio_bloques = sb.s_block_start;
          int mov = inicio_bloques + (pos * sizeof(bloque_archivos) ) ; // aqui esta la posicion del bloque que se quiere acceder
              fseek(disco, (mov-1), SEEK_SET); // nos posiciona donde inicia el bloque buscado
              fread(&b_archivo, sizeof(bloque_archivos), 1, disco); // lee y coloca en ba, el bloque en esa posicion

      fclose(disco);
    }else{
        printf("Error: no se puede aperturar el disco en 'get_bloque_archivos'\n");
    }
 return b_archivo;
}


bloque_carpetas get_bloque_carpeta(int pos, string ruta_disco) /*Busca y devuelve un bloque de carpeta buscandolor por la posicion inidicada por 'pos'*/
{

    //1) abrir el archivo que representa al disco
    FILE *disco = fopen(ruta_disco.c_str(), "r+b");
    bloque_carpetas bc_erroneo;

        if(disco != NULL)
        {
        //2) extraer el super_bloque de la particion global(particion en la cual se esta trabajando actualmente)
          fseek(disco, (particion_global.part_start - 1), SEEK_SET); // -> aqui inicia el super_bloque
            super_bloque sb;
            fread(&sb, sizeof(super_bloque),1,disco); // obtenemos el super_bloque

        //3) posicionarse donde comienzan los bloques y moverse con pos
            int inicio_bloques = sb.s_block_start;
            int mov = inicio_bloques + (pos * sizeof(bloque_carpetas) ) ; // aqui esta la posicion del bloque que se quiere acceder
            fseek(disco, (mov-1), SEEK_SET); // nos posiciona donde inicia el bloque buscado

            bloque_carpetas bc;
            fread(&bc, sizeof(bloque_carpetas),1,disco); // lee y coloca en ba, el bloque en esa posicion
            fclose(disco);
            return bc;

        }else{
          printf("Error: no se puede aperturar el disco en 'get_bloque_carpeta'\n");
          cout<<"posibles errores:"<<errno<<endl;
          cout<<"ruta_disco:"<<ruta_disco<<endl;
       }
  return bc_erroneo; // si llega hasta aqui implica que no encontro el bloque de carpetas deaseado..
}


void Sobrescribir_bloque_carpeta( bloque_carpetas bloqueC_nuevo, int pos_bloqueC) // Sobrescribe el bloque de carpeta en el disco.
{

    FILE *disco = fopen(ruta_disco_global.c_str(), "r+b");
        if(disco != NULL)
        {
            super_bloque sb = get_super_bloque();

            int inicio_bloques;
                inicio_bloques = sb.s_block_start; // almacenamos el inicio de los bloques
            int pos_escritura;
                pos_escritura = inicio_bloques + (pos_bloqueC * (sizeof(bloque_carpetas)) );

                fseek(disco, (pos_escritura -1), SEEK_SET); // posicionamos el puntero donde se debe Sobreescribir el bloque

                fwrite(&bloqueC_nuevo, sizeof(bloque_carpetas), 1, disco);
          fclose(disco);
        }else{
            puts("Error en 'Sobreescribir_bloque_carpeta': No se pudo abrir el disco");
        }
}


void Actualizar_bm_BLOQUES(FILE *disco, int inicio_bm_bloques, int pos) // Escribe 1 en el bit_map de inodos , en la posicion que indique 'pos'
{
 // 1) Moverse hasta la posicion donde se debe colocar el 1
    int pos_escritura = inicio_bm_bloques + ( pos*sizeof(char) );
        fseek(disco, pos_escritura-1, SEEK_SET); // posiciona el puntero interno del disco en donde inicia el bm_bloque

// 2) colocar el valor 1 en la posicion indicada por fseek
        fputc('1', disco);
}


int buscar_nuevo_bloque_libre(FILE *disco, super_bloque sb)
{
// 1) Colocarse al inicio bitmap de bloques
  int inicio_bm_blo = sb.s_bm_block_start;

// 2) Crear un ciclo for desde 0 hasta su tamaño, e ir buscando caracter a caracter en donde esta el primer '\0'
    int tamano = sb.s_block_count;
    char c;
    int pos = -1;

    fseek(disco, inicio_bm_blo-1, SEEK_SET); /*posicionar el puntero en el byte inicial del bm_bloques*/

    for(int i = 0; i < tamano; i++)
    {
        c = fgetc(disco); // obtenemos cada caracter y el puntero se va moviendo solito
        if( c == '\0')
        {
            pos = i ; // guardamos en que posicion se encontro el '0'
            break;
        }
    }
 // 4) Retornamos la posicion en donde se encontro el '0'

        printf("La posicion donde se encontro un nuevo_bloque_libre es:%i\n",pos);
       // printf("Y el valor de C se quedo con:%c\n",c);
 return pos;
}


int Escribir_bloque_archivo(int pos, string contenido, string ruta_disco) // Este metodo se llama en 'Escribir_contenido_ino_Tarchivo'
{
    int num_bloque_usado = -1;

    FILE *disco = fopen(ruta_disco.c_str(),"r+b");

    if(disco != NULL)
    {
     //2) Extraer el super_bloque de la particion
        rewind(disco);
        fseek(disco, (particion_global.part_start -1),SEEK_SET);// mover el puntero hasta donde esta el inicio del super bloque
        super_bloque sb;
        fread(&sb, sizeof(super_bloque), 1, disco); // leemos el super bloque

        if(pos != -1) // implica reescribir un bloque
        {
            num_bloque_usado  = pos;
            puts("Se quiere sobrescrbir un bloque interno de ino_archivo");
            // 1)  ir hacia la posicion donde se quiere colocar el bloque
            int inicio_bloques = sb.s_block_start;
            int pos_escritura = inicio_bloques + (pos *(sizeof(bloque_archivos)) );

            fseek(disco, pos_escritura-1, SEEK_SET);

            // 2) copiar contenido al bloque_archivos
            bloque_archivos ba;
            strcpy( ba.b_content, contenido.c_str() );
            fwrite(&ba, sizeof(bloque_archivos), 1, disco); // escribe ba en la posicion que tiene fseek (Reescribe el bloque)

        }else // crear un nuevo bloque archivo (pos = -1)
        {
            puts("Se quiere crear un bloque nuevo para la escritura");
            num_bloque_usado = sb.s_first_blo ; // siempre sera el que dice el superbloque +1 -> utilizo un bloque nuevo

            if(sb.s_free_blocks_count > 0) // VERIFICAR QUE EXISTAN BLOQUES LIBRES
            {
              bloque_archivos ba;
              strcpy(ba.b_content, contenido.c_str()); // inserta contenido en el bloque de archivo
                cout<<"QUE REMIERDA DE CONTENIDO ESTA RECIBIENDO:"<<ba.b_content<<"!\n";
              int pos_bloc_libre = sb.s_first_blo;/*busca la posicion del bloque libre en el super_bloque*/
              int pos_escritura =  sb.s_block_start + (pos_bloc_libre* sizeof(bloque_archivos));

              fseek(disco, pos_escritura-1, SEEK_SET);
              fwrite(&ba, sizeof(bloque_archivos), 1, disco);

              Actualizar_bm_BLOQUES(disco, sb.s_bm_block_start, pos_bloc_libre); // Escribir un 1 en el bm_bloques en la posicion que inidica sb.s_first_blo

            //4) Buscar un nuevo bloque libre para actualizar el super bloque y restar bloques libres(-1)
                int pos_nueva = buscar_nuevo_bloque_libre(disco, sb);
                int conteo = sb.s_free_blocks_count;

                sb.s_first_blo = pos_nueva;
                sb.s_free_blocks_count = conteo-1;

            //5) Reescribir el super_bloque con la informacion modificada
                   rewind(disco);
                   fseek(disco, (particion_global.part_start-1), SEEK_SET); // Colocarnos donde inicia la particion_global
                   fwrite(&sb,sizeof(super_bloque),1,disco); // escribimos el super_bloque en la particion_global

            }else{
                cout<<"Error en 'Escribir_bloque_archivo',No hay bloques libres en el super_bloque\n";
                num_bloque_usado = -1;
            }

        }

     fclose(disco);
    }else{
        printf("Error en Escribir_bloque_archivo: NO se pudo abrir el Disco\n");
    }
  return num_bloque_usado;
}


void Crear_bloque(FILE *disco, char tipo_bloque, int pos_inodo_padre, int pos_inodo_actual)  // tipo_bloque : 1 = archivo,   0 = carpeta,   2 = bloque apuntadores
{

    fseek(disco, (particion_global.part_start-1), SEEK_SET); // Colocarnos donde inicia la particion_global
    super_bloque sb;
      fread(&sb, sizeof(super_bloque),1,disco);
    //1) verificar si hay bloques disponibles
    if(sb.s_free_blocks_count>0)
    {
    //2) Buscar de que tipo sera el bloque a utilizar (archivo | carpeta | apuntadores)
      switch(tipo_bloque)
      {
        case '0': //  bloque_carpeta
        {
            bloque_carpetas bc; // posee 4 content
            //3) inicializar todo el bloque de carpetas a -1.. para saber que estan disponibles posteriormente
            for(int i = 0; i<4; i++)
            {
                bc.b_content[i].b_inodo = -1;
            }
            //4) Crear padre e hijo de el bloque de carpeta
            if(pos_inodo_padre >= 0) // si es menor a 0 significa que esta desactivada!!
            {
                strcpy(bc.b_content[0].b_name, ".");// El '.' hace referencia al Actual
                       bc.b_content[0].b_inodo = pos_inodo_actual; // Guarda la posicion del inodo actual
                strcpy(bc.b_content[1].b_name, ".."); // El '..' hace referencia al Actual
                       bc.b_content[1].b_inodo = pos_inodo_padre; // Guarda la posicion del inodo padre
            }

            //5) Buscar la posicion en donde se colocara el bloque recien creado
            int pos_bloc_libre = sb.s_first_blo;/* busca el primer bloque libre*/
            int pos_escritura_blo = sb.s_block_start + (pos_bloc_libre* sizeof(bloque_carpetas)); // indica en que posicion en el archivo se debe escribir el bloque
                  fseek(disco, pos_escritura_blo-1, SEEK_SET);
                  fwrite(&bc, sizeof(bloque_carpetas), 1, disco); // escribe bc en la posicion que tiene fseek

            //6) Escribir un 1 en el bm_bloques en la posicion que inidica s_first_blo
            Actualizar_bm_BLOQUES(disco, sb.s_bm_block_start, pos_bloc_libre);

            // 7) Buscar un nuevo bloque libre y restar los bloques libres(-1)
            int pos_nueva = buscar_nuevo_bloque_libre(disco, sb);
            //int conteo    = sb.s_block_count;

            sb.s_first_blo = pos_nueva;
            sb.s_free_blocks_count = sb.s_free_blocks_count - 1;

            // 8) Reescribir el superbloque con la informacion modificada
            rewind(disco);
            fseek(disco, (particion_global.part_start-1), SEEK_SET); // Colocarnos donde inicia la particion_global
                fwrite(&sb, sizeof(super_bloque), 1, disco); // escribimos el super_bloque en la particion_global

        }break;

        case '1': // bloque_archivo
        {
            printf("Creando bloque de archivo");
            bloque_archivos ba_nuevo; // posee un char[64] unicamente para escribir caracteres
         // 5) Buscar la posicion en donde se colocara el bloque recien creado
            int pos_bloc_libre = sb.s_first_blo;
            int pos_escritura_blo = sb.s_block_start + (pos_bloc_libre* sizeof(bloque_archivos)); // indica en que posicion en el archivo se debe escribir el bloque
                  fseek(disco, pos_escritura_blo-1, SEEK_SET);
                  fwrite(&ba_nuevo, sizeof(bloque_archivos), 1, disco); // escribe bc en la posicion que tiene fseek
         //6) Escribir un 1 en el bm_bloques en la posicion que inidica s_first_blo
            Actualizar_bm_BLOQUES(disco,sb.s_bm_block_start,pos_bloc_libre);

         // 7) Buscar un nuevo bloque libre y restar los bloques libres(-1)
            int pos_nueva = buscar_nuevo_bloque_libre(disco,sb);
            int conteo = sb.s_free_blocks_count;

            sb.s_first_blo = pos_nueva;
            sb.s_free_blocks_count = conteo-1;
        // 8) Reescribir el superbloque con la informacion modificada
            rewind(disco);
            fseek(disco, (particion_global.part_start-1), SEEK_SET); // Colocarnos donde inicia la particion_global
              fwrite(&sb, sizeof(super_bloque), 1, disco); // escribimos el super_bloque en la particion_global

        }break;
      }

    }else{
        cout<<"Error:Al crear un nuevo bloque, NO hay bloques libres.\n";
    }
}


void Actualizar_bloque_carpeta(bloque_carpetas bloqueC_actualizado, int pos_bloqueC)
{
    FILE *disco = fopen(ruta_disco_global.c_str(), "r+b");
    if(disco != NULL)
    {
        super_bloque sb = get_super_bloque();

        int inicio_bloques;
            inicio_bloques = sb.s_block_start; // almacenamos el inicio de los bloques
        int pos_escritura;
        pos_escritura = inicio_bloques + (pos_bloqueC * (sizeof(bloque_carpetas)) );

            fseek(disco, (pos_escritura -1), SEEK_SET); // posicionamos el puntero donde se debe Sobreescribir el bloque

            fwrite(&bloqueC_actualizado, sizeof(bloque_carpetas), 1, disco);
      fclose(disco);
    }else{
        puts("Error en 'Actualizar_bloque_carpeta': No se pudo abrir el disco\n");
    }

}

/*
int Crear_bloque_archivo() // retorna la posicion en donde creo el bloque de archivo
{
    int id_nuevo_bloque = -1;
    FILE *disco = fopen(ruta_disco_global.c_str(), "r+b");
    if(disco)
    {
        fseek(disco, (particion_global.part_start-1), SEEK_SET); // Colocarnos donde inicia la particion_global
        super_bloque sb;
        fread(&sb, sizeof(super_bloque),1,disco);

        if(sb.s_free_blocks_count>0)// 1) verificar si hay bloques disponibles
        {
        //2) Buscar de que tipo sera el bloque a utilizar (archivo | carpeta | apuntadores)
                printf("Creando bloque de archivo");
                bloque_archivos ba_nuevo; // posee un char[64] unicamente para escribir caracteres
             // 5) Buscar la posicion en donde se colocara el bloque recien creado
                int pos_bloc_libre = sb.s_first_blo;
                    id_nuevo_bloque= sb.s_first_blo;
                int pos_escritura_blo = sb.s_block_start + (pos_bloc_libre* sizeof(bloque_archivos)); // indica en que posicion en el archivo se debe escribir el bloque
                      fseek(disco, pos_escritura_blo-1, SEEK_SET);
                      fwrite(&ba_nuevo, sizeof(bloque_archivos), 1, disco); // escribe bc en la posicion que tiene fseek
             //6) Escribir un 1 en el bm_bloques en la posicion que inidica s_first_blo
                Actualizar_bm_BLOQUES(disco,sb.s_bm_block_start,pos_bloc_libre);

             // 7) Buscar un nuevo bloque libre y restar los bloques libres(-1)
                int pos_nueva = buscar_nuevo_bloque_libre(disco,sb);
                int conteo = sb.s_free_blocks_count;

                sb.s_first_blo = pos_nueva;
                sb.s_free_blocks_count = conteo-1;
            // 8) Reescribir el superbloque con la informacion modificada
                rewind(disco);
                fseek(disco, (particion_global.part_start-1), SEEK_SET); // Colocarnos donde inicia la particion_global
                  fwrite(&sb, sizeof(super_bloque), 1, disco); // escribimos el super_bloque en la particion_global
        }else{
            cout<<"Error:Al crear un nuevo bloque archivo, NO hay bloques libres.\n";
        }
        fclose(disco);

    }else{
        cout<<"Error: no se pudo abrir el disco en 'Crear_bloque_archivo()'";
    }

  return id_nuevo_bloque;
}


void set_contenido_b_archivo(string contenido, int pos_bloque, string ruta)   // inserta contenido(texto) en un bloque_archivo indicado en pos_bloque
{
    cout<<"El contenido que se colocara en el bloque seera:"<<contenido;
    bloque_archivos ba_nuevo;
    dstrcpy(ba_nuevo.b_content, contenido.c_str()); //1)  copiamos contenido al bloque de archivo

    //reescribir o actualizar el bloque

}

                            int tamano = contenido.size();
                            int pos_bloque;

                            for(int b = 0; b<15; b++) // acceder a los bloques del archivo
                            {
                                if(tamano == 0) // nos salimos del for
                                    break;
                                string concatenar = "";
                                int tam = 64; //Valor predeterminado
                                //EXTRAER DE 'contenido' la cantidad de caracteres que no sean mayor a 64 ni que supere 'tamano'
                                if(tamano <64)
                                    tam = tamano;

                                for(int indice=0; indice < tam; indice++ ){ // puede iterar de 0-64 DEPENDE del tamano de contenido

                                    concatenar += contenido[indice];
                                    contenido.erase(0,1); //al leer un caracter lo borramos
                                    tamano--; //actualizamos el tamaño de contenido para evitar errores de memoria
                                }

                                pos_bloque = archivo.i_block[b]; // accedemos a cada bloque del archivo(inodo)

                                if(pos_bloque != -1) // reescribir en un bloque existente
                                {
                                    set_contenido_b_archivo(concatenar, pos_bloque); // Insertamos el contenido en el archivo

                                }else{ // crear primero el bloque y luego reescribirlo
                                   int bloque_usado = Crear_bloque_archivo();
                                   set_contenido_b_archivo(concatenar, bloque_usado); // Insertamos el contenido en el archivo
                                   archivo.i_block[b] = bloque_usado; // Indicarle Al Archivo(Inodo) que bloque se acaba de utilizar
                                   Sobreescribir_inodo(ruta_disco_global, archivo, sb.s_first_ino);
                                }
                            }
*/

#endif // OP_BLOQUES_H
