#ifndef REP_H
#define REP_H


#include <iostream>
#include "lista_enlazada.h"
#include <fstream> // para ofstream

using namespace std;

string separar_directorio(string path); // prototipo que ya fue compilado (funcion de mkdisk.h)


string obtener_extension(string path_reporte)
{
    string extension_reporte = ""; // puede ser .jpg o .png
    bool concatenar = false;

    for(int i=0; i< (int) path_reporte.length() ; i++) // recorrer el path_reporte y extraer su extension
    {
        if(path_reporte[i] == '.')
            concatenar = true;

        if( (concatenar) && (path_reporte[i]!='.') ) // solo concatenar lo que este al lado derecho del '.'
            extension_reporte += path_reporte[i];
    }
  return extension_reporte;
}


void Graficar_MBR(mbr m, string path_reporte) // Escribe un archivo DOT en el disco duro para despues generar una grafica
{
    string extension_reporte = ""; // puede ser .jpg o .png
           extension_reporte = obtener_extension(path_reporte);

    ofstream archivo;
            archivo.open("mbr.dot");
    if(archivo)
    {
        archivo<< "digraph G{\nnode [shape=none]\na0[label=<<TABLE border=\"1\">\n";
        archivo<< "<TR>\n<TD>mbr_tamano</TD>\n<TD>";
        archivo<< m.mbr_tamano << "</TD>\n</TR>";

        archivo<< "\n<TR>\n<TD>mbr_fecha_creacion</TD>\n";
        archivo<< "<TD>"<< ctime(&m.mbr_fecha_creacion)<< "</TD>\n</TR>";

        archivo<< "<TR>\n<TD>mbr_disk_signature</TD>\n";
        archivo<< "<TD>"<< m.mbr_disk_signature<<"</TD>\n</TR>";

        archivo<< "<TR>\n<TD>disk_fit</TD>\n<TD>"<<m.disk_fit<<"</TD>\n</TR>";

        for(int i = 0; i < 4; i++)
        {
            if(m.particiones[i].part_status == '1') // solo graficar particiones activas
            {
                archivo << "<TR>\n<TD bgcolor=\"Chocolate\">part_name</TD>\n<TD>"<<m.particiones[i].part_name<<"</TD>\n</TR>\n" ;
                archivo << "<TR>\n<TD>part_status</TD>\n<TD>"<<m.particiones[i].part_status<<"</TD>\n</TR>\n";
                archivo << "<TR>\n<TD>part_type</TD>\n<TD>"<<m.particiones[i].part_type<<"</TD>\n</TR>\n";
                archivo << "<TR>\n<TD>part_fit</TD>\n<TD>"<<m.particiones[i].part_fit<<"</TD>\n</TR>\n";
                archivo << "<TR>\n<TD>part_start</TD>\n<TD>"<<m.particiones[i].part_start<<"</TD>\n</TR>\n";
                archivo << "<TR>\n<TD>part_size</TD>\n<TD>"<<m.particiones[i].part_size<<"</TD>\n</TR>\n";
            }
        }
        archivo<< "</TABLE>>];\n}";

        archivo.close();

        // una vez creado el mbr.dot, debemos decirle al sistema operativo que tome ese mbr.dot y genere una imagen
        string dir = "mkdir -p " + separar_directorio(path_reporte);
        system( dir.c_str() ); // creamos los directorios recursivamente

        string comando = "dot -T" + extension_reporte + " mbr.dot -o " + path_reporte ;
        system( comando.c_str() ); // creamos la imagen con graphviz
        cout<<"Rep: Imagen del (mbr) Generada correctamente en la ruta="<<path_reporte<<"\n";

    }else{
        cout<<"Rep: Error al abrir el archivo mbr.dot\n";
    }


}



void Graficar_DISK(mbr m, string path_reporte)
{
    partition part[4]; /*servira para ordenar las particiones*/
    int part_tamano = 0; /*para saber cuantas veces ordenar las particiones*/

    for(int i = 0; i < 4; i++)
    {
       if(m.particiones[i].part_status == '1') /*para grarficar solo nos interesan las particiones activas*/
       {
           part[part_tamano] = m.particiones[i]; /*lo insertamos sin ordenarlo*/
           part_tamano++;
       }
    }
    /*AHORA DEBEMOS ORDENAR LAS PARTICIONES DE MENOR A MAYOR EN ESPACIO YA QUE PUEDEN ESTAR DESORDENADAS */

    for(int i =0; i<part_tamano; i++)
    {
        for(int j=0; j<(part_tamano-1); j++)
        {
            if(part[j].part_start > part[j+1].part_start) /*se debe intercambiar las casillas*/
            {
                 partition aux = part[j];
                 part[j] = part[j+1];
                 part[j+1] = aux;
            }
        }
       // imprimir_info_particion(part[i]);
    }

    int size_disco = m.mbr_tamano;

    ofstream archivo;
    archivo.open("disk.dot");
    if(archivo)
    {
        archivo<<"digraph G{\nnode [fontname=\"Arial\"];\nnode [shape=none]\na0[label=<<TABLE border=\"2\" cellspacing=\"1\" cellpadding=\"20\">\n";
        archivo<<"<TR>\n<TD rowspan=\"2\" bgcolor=\"brown\">  MBR<BR/>"<< ((double)136 / (double) size_disco)*100 <<"%</TD>\n";

        int indice = 136 ; /*inicia despues del mbr*/
        int conta = 0; /*contador para poder saltar al siguiente particion, solo salta cuando es igual su limite inf y superior*/

        while(conta<part_tamano)
        {
          /*debemos extraer cada bloque para ver en donde hay espacios vacios*/
            printf("LO QUE VA A COMPARAR indice+1:%d == part_start:%d\n",indice+1,part[conta].part_start);
            if( (indice+1) == (part[conta].part_start) )
            {
                  indice = indice + (part[conta].part_size) ;

                  if(part[conta].part_type=='p'){
                    archivo<<"<TD rowspan=\"2\" bgcolor=\"Chocolate\"> PRIMARIA<BR/>"<<((double)(part[conta].part_size)/(double)size_disco)*100<<" %<BR/>"<<part[conta].part_name<<"</TD>\n";

                  }else if(part[conta].part_type=='e'){
                      archivo<< "<TD bgcolor=\"ForestGreen\"> EXTENDIDA<BR/>"<< (((double)part[conta].part_size)/(double)size_disco)*100 <<" %<BR/>"<<part[conta].part_name<<"</TD>\n";
                  }

              conta++; /*Solo cuando coinciden podeos saltar al siguiente bloque*/
            }
             else
            { /*Esto significa que SI hay un espacio disponible entre inicio y un bloque(partition) cualquierda*/

                int  espacio_libre = (part[conta].part_start - 1) - indice;
                archivo<< "<TD rowspan=\"2\" bgcolor=\"DimGray\"> LIBRE <BR/>"<< ((double)espacio_libre/(double)size_disco)*100 << " %</TD>\n";

                indice = indice + espacio_libre  ; /*+ (part[i].part_size)*/
            }
        }

       /*Al salir de buscar espacio hay 2 posibilidades
              1 -> si indice es == al total del disco, significa que si encontro todos los espacios libres del disco
              2 -> si indice != total de diso, significa que olvido la ultima parte del disco
             */
             if(indice == (m.mbr_tamano)) /*Considero todos los posibles espacios libres existentes..*/
             {
                   printf("SI ENCONTRE PARTICION y llegue al final");

             }
              else/*NO considero todos los espacios libres le falta el ultimo*/
             {
               // printf("indice:%d\n",indice);
                //printf("tamano_disco:%d\n",m.mbr_tamano);
                //printf("FALTO ANALIZAR EL ULTIMO ESPACIO \n");

                int ultimo_espacio = m.mbr_tamano - indice;
                archivo<< "<TD rowspan=\"2\" bgcolor=\"DimGray\"> LIBRE <BR/>"<<((double)ultimo_espacio/(double)size_disco)*100<<" %</TD>\n" ;
             }
         archivo<<"</TR>\n</TABLE>>];\n}";
         archivo.close();

         string dir = "mkdir -p " + separar_directorio(path_reporte);
         system( dir.c_str() ); // creamos los directorios recursivamente

         string extension_reporte = ""; // puede ser .jpg o .png
                   extension_reporte = obtener_extension(path_reporte);

         string comando = "dot -T" + extension_reporte + " disk.dot -o " + path_reporte ;
         system( comando.c_str() ); // creamos la imagen con graphviz


    }else {
        cout<<"Rep: Error al abrir el archivo (disk.dot)\n";
    }

}



void Instruccion_REP(string path, string name, string id)
{
 /*SOLO SE PUEDEN MOSTRAR REPORTES DE PARTICIONES MONTADAS en la lista_enlazada.h*/

 //REP UTILIZA "id" para realizar cualquier accion por esto hay que usar "id" en la lista enladaza para buscar la ruta

    string ruta = get_ruta(id); /*Busca la ruta que tiene el id_disco montando(MOUNT) funcion de "lista_enlazada.h"*/

    FILE *disco = fopen(ruta.c_str(),"r+b"); // abre para lectura y escritura al inicio del disco(archivo)

    if(disco!=NULL)
    {
        mbr aux; /*para sacar el mbr del disco*/

        int mbr_leido = fread(&aux, sizeof(aux), 1, disco);
        fseek(disco, 0, SEEK_SET);

        if(mbr_leido == 1)
        {
            if(name == "disk")
            {
                Graficar_DISK(aux, path);
            }
            else if( name == "mbr"){ /*el reporte entonces sera mbr */

                Graficar_MBR(aux, path);
            }else{
                cout<<"Rep: Error no se puede generar el reporte con el name->"<<name<<".Se esperaba (disk | mbr)"<<endl;
            }

        }

        fclose(disco);
    }else{
           printf("REP: Error el disco NO existe o la particion NO esta montada \n");
    }



}
#endif // REP_H
