#ifndef MKDISK_H
#define MKDISK_H

#include <iostream>
#include <cstdio>
#include "estructuras.h"
#include "string.h"
#include "raid1.h"

using namespace std;


string separar_directorio(string path) // recibe la ruta completa y debe separarla y solo retornar el directorio que se debe crear
{
  char delimitador = '/';
  string dir = "";
  string aux = "";
  char c;
  for(int i = 0; i < (int) path.length(); i++)
  {
    c = path.at(i); // obtenemos el caracter de la posicion i de "path"
    if(c != delimitador) // concatenar
    {
        aux += c;
    }else {
        if(aux != "")
        {
            dir = dir +"/"+ aux;
            aux = "";
        }
    }
  }
 // cout<<"El directorio ya separado es ->"<<nuevo_path<<endl;
 return dir;
}



mbr crear_mbr(int size, char fit) // crea un mbr con los parametros enviados y lo retorna
{
    srand( time(0) );
    int disk_signature = 201314172 + rand(); // generamos un numero aleatorio
    if(disk_signature < 0)
    {   cout<<"el Disk signature fue negativo asi que se modifico";
        disk_signature = disk_signature * (-1);
    }

    time_t tiempo_actual;
           tiempo_actual = time(NULL);

    if (tiempo_actual == ((time_t)-1))
    {
        (void) fprintf(stderr, "Fallo al obtener tiempo actual.\n");
        exit(EXIT_FAILURE);
    }
    // inicializamos el mbr del disco recien creado
    mbr m = { size, tiempo_actual, disk_signature, fit,
             {{'0','p','w',0,0,"nulo"},{'0','p','w',0,0,"nulo"},{'0','p','w',0,0,"nulo"},{'0','p','w',0,0,"nulo"}}
            };
   /* cout<<"mbr_tamano:"<<m.mbr_tamano<<"\ndisk_signature:"<<disk_signature<<"\nfit:"<<fit;
    cout<<"\npart_status:"<<m.particiones[3].part_status<<"\npart_type:"<<m.particiones[3].part_type;
    cout<<"\npart_fit:"<<m.particiones[3].part_fit<<"\npart_start:"<<m.particiones[3].part_start;
    cout<<"\npart_size:"<<m.particiones[3].part_size<<"\npart_name:"<<m.particiones[3].part_name;*/

 return m;
}



void Instruccion_MKDISK(int size, char fit, char unit, string path) // Ejecutara las funciones de MKDISK recibiendo 4 parametros
{
// paso 1) verificar que los parametros sean los aceptados por el comando
 if((size > 0) && (fit != '\0') && (unit != 'b') && (path!=""))
 {
   // CREAR EL DIRECTORIO
    string comando_consola = "mkdir -p '"; // crear un comando que se ejecutara en la consola, para crear directorios recursivamente
    comando_consola +=  separar_directorio(path) + "'";

    system(comando_consola.c_str()); // lo ejecutamos en la consola del sistema operativo.

    // CREAR EL ARCHIVO (SIMULARA EL DISCO)
    FILE *disco ;
          disco = fopen(path.c_str(), "w+b");
          cout<<"ruta->"<<path.c_str()<<".\n";
    if(disco != NULL)
    {
    // DAR EL TAMAÑO AL DISCO INSERTANDO \0
        switch(unit)
        {
            case 'k':{ size = size * 1024; }break; // para crear en kilobytes

            case 'm':{ size = size * (1024*1024);}break; // para crear en megabytes
        }
        char buffer[1024];
        for(int i =0; i < 1024; i++) //Para llenar el buffer
            buffer[i] = '\0';

        for(int i = 0; i < (size/1024); i++) // Para insertar \0 en el disco y darle tamaño
            fwrite(buffer,sizeof(buffer), 1, disco);

     // CREAR EL MBR Y GRABARLO EN EL INICIO DEL DISCO
        mbr nuevo_mbr = crear_mbr(size, fit);
            rewind(disco); // colocarse en la primer posicion del disco..
            fwrite(&nuevo_mbr, sizeof(nuevo_mbr), 1, disco); // Escribimos el mbr recien creado en el disco duro

        puts("Disco creado con exito\n");
        MKDISK_RAID1(nuevo_mbr, path, size); // CREACION DEL DISCO ESPEJO (raid 1)

     fclose(disco); // Cerrar el archivo (DISCO)

    }else{
        puts("MKDISK error -> fallo la creacion del Disco");
    }
 }else{
     cout<<"Error Interno: Comando 'MKDISK' detecto que un parametro tiene un valor incorrecto O falto un parametro.";
 }
}

#endif // MKDISK_H
