#ifndef OP_INODOS_H
#define OP_INODOS_H

#include <iostream>
#include <stdio.h>
#include "op_bloques.h" // ->este header incluye 'getterysetter.h y estructuras.h y vect.h'

/*=================== Prototipos==================*/
/*====Externos=====*/
//void Journaling(string tipo_operacion, string ruta, int propietario, int contenido, string pwd, string grp); // compilada en 'mkfs.h'


string leer_contenido_ino_Tarchivo(Inodo inodo) // Devuelve todo el contenido adentro de los bloques de un INODO DE TIPO ARCHIVO
{
  int size = inodo.i_size;
    printf("EL TAMAÑO ES:%i\n", size);
  string contenido_bloques = ""; // para almacenar TODOS los caracteres que esten en los bloques del inodo 'users.txt'

    // 1) Recuperar la informacion que tiene adentro los bloques activos
    for(int i=0; i<15; i++)
    {
        int posicion_iblock = inodo.i_block[i]; // indica en que posicion se encuentran los bloques de este inodo

        if(posicion_iblock != -1) // solo ingresa a bloques que este activos
        {
            bloque_archivos b_archivo = get_bloque_archivo(posicion_iblock, ruta_disco_global); // devuelve el bloque en la posicion indicada por 'posicion_iblock'
            int cantidad; // para saber cuantos caracteres leer por cada bloque activo
            //cout<<"raw data:"<<b_archivo.b_content<<endl;

            if(size<=64)
            {
                cantidad=size;
            }else{ // es mayor a 64 por lo tanto hay mas bloques por leer, pero el maximo que se puede leer por bloque es 64 asi que hay que restar
                cantidad = 64;
                size = size - cantidad ;
            }

            for(int i=0; i < cantidad; i++)
            {
                contenido_bloques += b_archivo.b_content[i];
            }
        /*sabiendo que los bloques 13 ,14 ,15 son indirectos*/
        }
    }
    //cout<<"sin embargo esto sera lo que retornara->"<< contenido_bloques<<"!"<<endl;
  return contenido_bloques;
}




void Actualizar_bm_INODOs(FILE *disco, int inicio_bm_inodos, int pos) /*Escribe un '1' en el bit_map de inodos, en la posicion que indique 'pos'*/
{

// 1) Moverse hasta la posicion donde debo colocar el 1
    int pos_escritura = inicio_bm_inodos + ( pos * sizeof(char) ); // sizeof(char) = 1
    fseek(disco, pos_escritura-1, SEEK_SET); // posiciona el puntero interno de disco en donde inicia bm_inodo

// 2) Colocar el valor 1 en la posicion inidicada por fseek
    fputc('1',disco);
}




int buscar_nuevo_inodo_libre(FILE *disco, super_bloque sb)
{
// 1) colocarse al inicio del bit_map de inodos
    int inicio_bm_inodo ;
        inicio_bm_inodo = sb.s_bm_inode_start;

// 2) Crear un ciclo for desde inicio_bm_inodo hasta su tamaño completo.
     int tamano =  sb.s_inodes_count ; // Este es el byte donde termina el bm_inodos
     char c;
     int pos = -1;

      fseek(disco, inicio_bm_inodo-1, SEEK_SET); /*posicionar el puntero en el byte inicial del bm_inodos*/

        for(int i = 0; i < tamano; i++)
        {
 // 3) Buscar el valor 0 y devolver la posicion en donde fue encontrado
            c = fgetc(disco); // obtenemos cada caracter y el puntero se va moviendo solito
            if(c=='\0')
            {
                pos = i;
                break;
            }
        }
// 4) Si pos>=0 encontro un inodo libre, sino significa que los inodos ya estan llenos
      printf("La posicion donde se encontro un nuevo_inodo_libre es:%i\n",pos);

 return pos;
}





void Crear_INODO(FILE *disco, char i_type, int id_propietario,int i_size, int i_perm, int pos_inodo_padre)
{
    fseek(disco, (particion_global.part_start-1), SEEK_SET); // Colocarnos donde  inicia la particion_global
    super_bloque sb;
    fread(&sb, sizeof(super_bloque), 1, disco);
    int pos_inodo_Actual = sb.s_first_ino;
 // 1) Verificar que inodos libres > 0
    if(sb.s_free_inodes_count > 0)
    {
 // 2) Crear un inodo nuevo
        Inodo inodo;
              inodo.i_uid = id_propietario;
              inodo.i_gid = id_propietario; // aqui hay que realizar una busqueda
              inodo.i_size = i_size;
              inodo.i_atime = get_tiempo();
              inodo.i_ctime = get_tiempo();
              inodo.i_mtime = get_tiempo();
              for(int i = 0; i<15; i++) // coloca en -1 todos los bloques internos del inodo
                  inodo.i_block[i] = -1;
              inodo.i_block[0] = sb.s_first_blo; // como es un INODO NUEVO automaticamente se enlaza al 'primer bloque libre'
              inodo.i_type = i_type; // puede ser: archivo o carpeta
              inodo.i_perm = i_perm;

 // 3) Buscar el primer inodo libre y moverse hacia esa posicion
      int inicio_inodos = sb.s_inode_start;
      int pos_inodo_libre = sb.s_first_ino;
      int pos_escritura_inodo = inicio_inodos + (pos_inodo_libre * sizeof(Inodo) ); // indica en que posicion en el archivo se debe escribir el inodo

 // 4) Escribir el inodo nuevo donde indique -> 'pos_escritura_inodo'
        fseek(disco, pos_escritura_inodo-1, SEEK_SET);
        fwrite(&inodo, sizeof(Inodo), 1, disco); // escribe inodo en la posicion que tiene fseek

 // 5) Poner en '1' en el bitmap de inodo, ya que ese inodo fue ocupado
        Actualizar_bm_INODOs(disco, sb.s_bm_inode_start, pos_inodo_libre); // escribimos un 1 en el bm, para que marque esa posicion como NO LIBRE

 // 6) Buscar un nuevo inodo libre, restar al conteo de inodos libres y realizar esos cambios en el super bloque
        int pos = buscar_nuevo_inodo_libre(disco,sb);
        int conteo = sb.s_free_inodes_count ;

        sb.s_first_ino = pos; // colocamos en el super_bloque el nuevo inodo libre
        sb.s_free_inodes_count = conteo - 1; // restamos al conteo de inodos libres

// 7) Reescribir el super_bloque en la particion para almacenar los cambios
        fseek(disco, (particion_global.part_start-1), SEEK_SET); // Colocarnos donde inicia la particion_global

        fwrite(&sb, sizeof(super_bloque), 1, disco); // reescribimos el super_bloque en la particion_global

// 8) regla: si se crea un inodo, se debe crear tambien un bloque de (carpeta o archivo) , CREAR nuevo bloque

        Crear_bloque(disco, i_type, pos_inodo_padre,pos_inodo_Actual ); // El tipo de bloque que se creara depende del tipo de inodo
    }
    else
        cout<<"Error: No se puede crear un inodo ya que no hay (free inodes) para utilizar\n";
}



Inodo Escribir_contenido_ino_Tarchivo(Inodo inodo, string origen) // Escribe todo el contenido de origen en los bloques de un INODO de tipo archivo y devuelve el inodo actualizado
{
 //1) Actualizar el inodo completo
    int tamano    = origen.length();
    inodo.i_mtime = get_tiempo(); // Actualizamos fecha de la actualizacion del inodo
    inodo.i_size  = tamano; // Actualizamos el tamaño en bytes del contenido del inodo
    cout<<"Cual es el tamaño que debio aparecer?"<<tamano<<endl;
//2) Reescribir la informacion adentro de los bloques

    int inicio = 0; // para ciclo for

    for(int i = 0; i < 15; i++) // para accesar a todos los 15 bloques del archivo
    {
        int pos = inodo.i_block[i]; // ingresamos a cada bloque y sacamos su apuntador

        if(tamano>0) // si esto ocurre significa que aun existen bytes por escribir en el archivo
        {
         //3) Enviar texto que NO sea mayor a 64 , ya que un bloque de archivos solo tiene capacidad para 64 bytes
            string nuevo_contenido = "";
            int cantidad = 0; // para ciclo for

            if(tamano<=64)
            {
                cantidad = tamano;
                tamano = tamano - cantidad;
            }else{
                cantidad = 64;
                tamano = tamano - 64;
            }

            for(int i = inicio; i < (inicio + cantidad); i++) // insertar en texto la cantidad de caracteres indicados por cantidad
            {
                nuevo_contenido += origen[i];
            }
            inicio = inicio +cantidad; // modificar el inicio

        //4) llamar a la funcion encargada de escribir o sobreescribir el bloque en el disco donde 'pos' puede ser ( -1 | >0 )
           int bloque_utilizado = Escribir_bloque_archivo(pos, nuevo_contenido, ruta_disco_global); // debe retornar el numero de bloque que utilizo
                cout<<"Que bloque utilizo:"<<bloque_utilizado<<endl;
        //5) Actualizar el i_block con el nuevo apuntador si es que un nuevo bloque se utilizo
           //if(pos == -1)
              // bloque_utilizado++;
           inodo.i_block[i] = bloque_utilizado;

        }else{
            printf("Los bloques utilizados del inodo archivo fueron (0-14) fueron:%i\n",i-1);
            break;
        }

    }
  return inodo; // devuelve el inodo modificado luego de escribir en los bloques
}


void Sobreescribir_inodo_Tarchivo(string ruta_disco, Inodo inodo_modificado, int pos)// sobreescribe un inodo de tipo archivo en la posicion indicada por 'pos'
{
    FILE *disco = fopen(ruta_disco.c_str(), "r+b");

    if(disco != NULL)
    {
        //1) Abrir el super bloque de la particion
        rewind(disco);
        fseek(disco, (particion_global.part_start -1),SEEK_SET);// mover el puntero hasta donde esta el inicio del super bloque
        super_bloque sb;
        fread(&sb, sizeof(super_bloque), 1, disco); // leemos el super bloque

        //2) ir hasta donde esta el inodo indicado por 'pos'
        int inicio_inodos;
            inicio_inodos = sb.s_inode_start ;

        int pos_escritura;
             pos_escritura = inicio_inodos + (pos * sizeof(Inodo) ); // indica el byte donde inicia el inodo a reescribir

        fseek(disco, (pos_escritura -1),SEEK_SET); // puntero del disco se coloca donde estan los inodos
        fwrite(&inodo_modificado, sizeof(Inodo), 1, disco); // reescribe el inodo donde indico 'pos'

      fclose(disco);
    }else{
        printf("Error en 'Sobreescribir_inodo_Tarchivo': no se pudo abrir el disco\n");
    }

}



void Sobreescribir_inodo(string ruta_disco, Inodo inodo_modificado, int pos)
{
    FILE *disco = fopen(ruta_disco.c_str(), "r+b");

    if(disco != NULL)
    {
        //1) Abrir el super bloque de la particion
        rewind(disco);
        fseek(disco, (particion_global.part_start -1),SEEK_SET);// mover el puntero hasta donde esta el inicio del super bloque
        super_bloque sb;
        fread(&sb, sizeof(super_bloque), 1, disco); // leemos el super bloque

        //2) ir hasta donde esta el inodo indicado por 'pos'
        int inicio_inodos;
            inicio_inodos = sb.s_inode_start ;

        int pos_escritura;
             pos_escritura = inicio_inodos + (pos * sizeof(Inodo) ); // indica el byte donde inicia el inodo a reescribir

        fseek(disco, (pos_escritura -1),SEEK_SET); // puntero del disco se coloca donde estan los inodos
        fwrite(&inodo_modificado, sizeof(Inodo), 1, disco); // reescribe el inodo donde indico 'pos'

      fclose(disco);
    }else{
        printf("Error en 'Sobreescribir_inodo_Tarchivo': no se pudo abrir el disco\n");
    }
}


Inodo get_inodo(int posicion, string ruta_disco) // funcion que busca un Inodo en el Disco duro, por medio de su 'posicion'
{
    //1) abrir el archivo que representa al disco
    FILE *disco = fopen(ruta_disco.c_str(), "r+b");
    Inodo ino;
          ino.i_type = 'E'; // lo inicializamos en E=Error, por si acaso no se puede recuperar el Inodo
    if(disco != NULL)
    {
     //2) extraer el super_bloque de la particion global(particion en la cual se esta trabajando actualmente)
       fseek(disco, (particion_global.part_start - 1), SEEK_SET); // -> aqui inicia el super_bloque
       super_bloque sb;
       fread(&sb, sizeof(super_bloque), 1, disco); // obtenemos el super_bloque

     //3) posicionarse donde comienzan los Inodos y moverse con posicion
         int inicio_inodos = sb.s_inode_start;
         int mov = inicio_inodos + (posicion * sizeof(Inodo) ) ;
             fseek(disco, (mov-1), SEEK_SET); // nos posiciona donde inicia el inodo buscado

                 fread(&ino, sizeof(Inodo), 1, disco); // lee y coloca en ba, el bloque en esa posicion
                 return ino;

      fclose(disco);
     }else{
       printf("Error: no se puede aperturar el disco en 'get_inodo'\n");
    }

 return ino;
}



int buscar_pos_Carpeta(Inodo padre, string nombre_carpeta) // Retorna la posicion de donde se encuentra el inodo identificado por 'nombre_carpeta'
{
    //1) recibir el inodo y buscar adentro de sus bloques de archivo el que tenga 'nombre_carpeta'
    int pos; // para igualarlo a todos los bloques no nulos(-1) del inodo
    for(int i = 0; i < 15; i++) // para recorrer cada bloque
    {
      pos = padre.i_block[i];
     //2) Verificar si el bloque esta activo, solo ingresar a los activos
        if(pos != -1) // implica que el bloque_carpeta esta activo
        {
     // 3) extraer el bloque_carpeta del disco
            bloque_carpetas bc_aux = get_bloque_carpeta(pos, ruta_disco_global);

     // 4) Verificar el nombre de las carpetas o archivos (exceptuando el . y el ..)
            for(int x=0; x<4; x++) // para recorrer el contenido de los bloques de carpeta
            {
                content contenido = bc_aux.b_content[x] ;

                if(strcmp(contenido.b_name, nombre_carpeta.c_str()) ==0) //comparacion
                {
                    puts("Encontro la carpeta");
                    return contenido.b_inodo; //retornamos la posicion del inodo al que corresponde dicha carpeta
                }

            }

        }

    }
    return -1; // si llega hasta aqui implica que el inodo_carpeta buscado no existe
}


objeto_existencia Existen_Carpetas(string path) // funcion que busca que todas las carpetas existan, y retorna la posicion del inodo de la ultima carpeta
{
    objeto_existencia existe_carpeta;
                      existe_carpeta.esValido = true;

    int  pos_inodo = 0; // para saber en que inodo carpeta quiere crear el archivo (la 0 es la raiz)
    Inodo padre = get_inodo(pos_inodo, ruta_disco_global); //Aqui recuperamos el Inodo RAIZ(/)
    //1) RECORRER TODAS LAS CARPETAS (excepto la ultima )
    vector<string> splitear = split(path,'/');
    int tamano = splitear.size();
    for(int i = 0; i < tamano-1; i++) // no ingresar a la ultima posicion del arreglo(ya que esa es la que se deasea crear)
    {
     //2) BUSCAR QUE LAS CARPETAS EXISTAN Y RETORNAR EN QUE POSICION DEL DISCO ESTAN
         string nombre_carpeta = splitear[i];
         pos_inodo = buscar_pos_Carpeta(padre, nombre_carpeta); // busca donde se localiza el inodo Carpeta por medio de su nombre

         if(pos_inodo != -1) // si encontro la posicion donde esta la carpeta
         {
             padre = get_inodo(pos_inodo, ruta_disco_global); // recuperar el inodo de esa carpeta

         }else{ // NO encontro la posicion donde esta la carpeta, eso implica que no existe la carpeta
             cout<<"Error, La carpeta con el nombre:"<<nombre_carpeta<<", no Existe, debe crearla previamente o utilizar el comando'-p'\n";
             existe_carpeta.esValido = false;
             break; // termina el for
         }
    }

    existe_carpeta.nombre_aCrear   = splitear[tamano - 1];
    existe_carpeta.padre           = padre;
    existe_carpeta.pos_inodo_padre = pos_inodo;

 return existe_carpeta; // DEBEN existir todas las carpetas internas de 'path', caso contrario devuelve -1
}


int existe_Inodo(Inodo Carpeta_padre, string nombre_inodo) // Busca un (archivo|carpeta) adentro de una carpeta padre y retorna donde lo encontro
{
    //1) recibir el inodo y buscar adentro de sus bloques de archivo el que tenga 'nombre_carpeta'
    int pos; // para igualarlo a todos los bloques no nulos(-1) del inodo
    for(int i = 0; i < 15; i++) // para recorrer cada bloque
    {
      pos = Carpeta_padre.i_block[i];
      //2) Verificar si el bloque esta activo, solo ingresar a los activos
        if(pos != -1) // implica que el bloque_carpeta esta activo
        {
      // 3) extraer el bloque_carpeta del disco
            bloque_carpetas bc_aux = get_bloque_carpeta(pos, ruta_disco_global);

      // 4) Verificar el nombre de las carpetas o archivos (exceptuando el . y el ..)
            for(int x=0; x<4; x++) // extrae todo el contenido adentro de un bloque_carpeta
            {
                content contenido = bc_aux.b_content[x] ;

                if(strcmp(contenido.b_name, nombre_inodo.c_str())==0) //comparacion
                {
                    puts("Encontro el inodo");
                    puts(contenido.b_name);
                    return contenido.b_inodo; //retornamos la posicion del inodo al que corresponde dicha carpeta o archivo
                }

            }

        }

    }
 return -1; // si llega hasta aqui implica que el archivo buscado no existe
}


#endif // OP_INODOS_H
