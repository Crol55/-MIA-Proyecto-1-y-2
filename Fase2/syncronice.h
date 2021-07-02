#ifndef SYNCRONICE_H
#define SYNCRONICE_H

#include <iostream>
#include "lista_enlazada.h"
#include "manejo_particiones/getterysetter_globales.h"
#include <fstream> //ofstream , ifstream
#include "manejo_particiones/op_inodos.h"
#include <curl/curl.h>

using namespace std;

void EnviarA_nodejs(string data); // prototipo

void SYNCRONICE(string id)
{
    // transifere el sistema de archivos identificado con 'id' a la base de datos
    // Debe de estar montada la particion sino dara un error
    string ruta_disco = get_ruta(id); // funcion de "lista_enlazada.h"
    if(ruta_disco == "")
    {
        cout<<"syncronice: Error, la particion con el id="<<id<<", no esta montada(utilizar comando mount).\n";
        return;
    }
    // si llega aqui implica que si esta montada la particion
    FILE *disco = fopen(ruta_disco.c_str(), "rb"); // abrir en modo lectura
    if(disco != NULL)
    {
        mbr mbr_aux;
        fread(&mbr_aux, sizeof(mbr_aux), 1, disco); /*leemos los 136 bytes iniciales donde sabemos que se encuentra el mbr*/

        string nombre_particion = get_nombre_particion(id); // funcion de 'lista_enlazada.h'
        partition particion = get_partition(nombre_particion, mbr_aux);

        string dataJSON; // ser el string que se enviara a c++
        ofstream salida;
                 salida.open("cescritura.json"); // Archivo en la carpeta del proyecto

        if(salida)
        {
            salida<<"{\n";
            salida<<"   \"ruta\": \""<<ruta_disco<<"\",\n";
            salida<<"   \"tamano\": "<<mbr_aux.mbr_tamano<<",\n";
            salida<<"   \"particion\": {\n";
            salida<<"       \"part_name\": \""<<particion.part_name<<"\",\n";
            salida<<"       \"part_status\": \""<<particion.part_status<<"\",\n";
            salida<<"       \"part_type\": \""<<particion.part_type<<"\",\n";
            salida<<"       \"part_fit\": \""<<particion.part_fit<<"\",\n";
            salida<<"       \"part_start\": "<<particion.part_start<<",\n";
            salida<<"       \"part_size\": "<<particion.part_size<<"\n";
            salida<<"   },\n";
            // obtener informacion del super bloque
            fseek(disco, (particion.part_start -1), SEEK_SET); // posicionarse en el inicio de la particion
            super_bloque sb;
            fread(&sb, sizeof(super_bloque), 1, disco); // leer el superbloque
            salida<<"   \"super_bloque\": {\n";
            salida<<"       \"total_inodos\": "<<sb.s_inodes_count<<",\n";
            salida<<"       \"total_bloques\": "<<sb.s_block_count<<",\n";
            salida<<"       \"inodos_disponibles\": "<<sb.s_free_inodes_count<<",\n";
            salida<<"       \"bloques_disponibles\": "<<sb.s_free_blocks_count<<"\n";
            salida<<"   },\n";


            //1)  Obtener el inicio del bm_inodos
            int inicio_bm_inodos = sb.s_bm_inode_start;

            // 2) Colocarse con fseek al inicio_bm_inodos
            fseek(disco, inicio_bm_inodos -1, SEEK_SET);
            char c;
            // 3)  Recorrer desde 0 hasta el tamaño del bm_inodos
            partition temp = particion_global; // porque 'get_inodo(i,ruta)' utiliza la particion global entonces debemos almacenarla antes
            particion_global = particion;

            salida<<"   \"inodos\": [\n";
            bool comma = false;
            for(int i=0; i< sb.s_inodes_count; i++)
            {
                c = fgetc(disco); // obtenemos cada caracter y el puntero se va moviendo solito
                if(c == '1') // indica que hay un inodo activo en el bitmap
                {
                    if(comma) // Al iniciar el For (NO VA a colocar ','), HASTA EL SIGUIENTE
                        salida<<",\n";

                    Inodo inodo = get_inodo(i, ruta_disco);
                    salida<<"       {\n";
                     salida<< "          \"id\":"<<i<<",\n";
                     salida<< "          \"i_uid\": "<<inodo.i_uid<<",\n";
                     salida<< "          \"i_gid\" : "<<inodo.i_gid<<",\n";
                     salida<< "          \"i_size\" : "<<inodo.i_size<<",\n";
                     //salida<< "          \"i_atime\": \""<<ctime(&inodo.i_atime)<<"\",\n";
                     //salida<< "          \"i_ctime\": \""<<inodo.i_ctime<<"\",\n";
                     //salida<< "          \"i_mtime\": \""<<ctime(&inodo.i_mtime)<<"\",\n";
                     salida<< "          \"i_type\": \""<<inodo.i_type<<"\",\n";
                     salida<< "          \"i_iperm\" : "<<inodo.i_perm<<",\n";

                     char tipo_inodo ;// 0 carpeta, 1 bloque
                          tipo_inodo = inodo.i_type ;

                     switch(tipo_inodo)
                     {
                         case '0': // El inodo es de carpeta
                         {
                             bloque_carpetas bc;
                             bool comma2 =false;
                             salida<<"          \"bloque_c\":[\n";
                             for(int j= 0; j<15; j++) // recorrer todos los bloques_carpeta
                             {
                                 int pos_bloqueC = inodo.i_block[j];
                                 if(pos_bloqueC != -1) // implica que esta activo
                                 {
                                     bc = get_bloque_carpeta(pos_bloqueC, ruta_disco); // metodo de 'getterYsetter.h'
                                     //salida<< "\"bloqueC_\":"<<pos_bloqueC;

                                     for(int k = 0; k < 4; k++)
                                     {

                                         if(bc.b_content[k].b_inodo != -1)
                                         {
                                             if(comma2)
                                                 salida<<",\n";
                                             salida<<"              {\n";
                                             salida<<"               \"nombre\": \""<<bc.b_content[k].b_name<<"\",\n";
                                             salida<<"               \"inodo\": "<<bc.b_content[k].b_inodo;
                                             salida<<"\n              }";
                                             comma2 = true;
                                         }
                                     }
                                 }
                             }
                             salida<<"\n            ]\n";
                         }break;

                     case '1': // El inodo es de archivo
                     {
                         bloque_archivos ba;
                         int size_aux    = inodo.i_size; // para saber la cantidad de caracteres que se deben leer de los bloques (DEPENDE DEL SIZE DEL INODO)
                         salida<<"       \"bloque_a\": [\n";
                         bool comma3 =false;
                         for(int j=0; j<15; j++)
                         {
                             int pos_bloqueA = inodo.i_block[j];

                             if(pos_bloqueA != -1) // solo ingresa si hay bloque activo
                             {
                                 ba = get_bloque_archivo(pos_bloqueA, ruta_disco);

                                 string concat = "";
                                 int iterador = 64; // valor por defecto ya que los bloques tienen 64 bytes(maximo)

                                 if(size_aux < 64)
                                     iterador = size_aux; // para que no lea 64 caracteres, ya que si lee de mas puede contener basura
                                 else{
                                     size_aux = size_aux - 64;
                                 }
                                 char caracter;
                                 for(int k=0; k<iterador; k++) // extrae los caracteres del bloque
                                 {
                                     caracter = ba.b_content[k];
                                     if(caracter != '\n')
                                     {
                                         concat += caracter; // concatena los caracteres
                                     }else{
                                         concat += "<br/>"; // para que en graphviz si puedan darse los saltos de linea
                                     }

                                 }
                                 if(comma3)
                                     salida<<",\n";
                                 salida<<"{\"contenido\" : "<< "\""<<concat<<"\"}";
                                 comma3 = true;
                             }
                         }
                         salida<<"]\n";
                     }break;
                     }// switch
                    salida<<"       }"; // para separar cada objeto inodo
                    comma = true;
                }// if
            } // for
            salida<<"\n    ]\n";
            salida<<"}";
            particion_global = temp; // volvemos a colocar si es que tenia alguna particion global..

          salida.close();
        }else{
            cout<<"syncronice: Error al abrir el archivo (JSON)\n";
        }

      fclose(disco); // cerrar el discooooo....

      ifstream ifs; // leer el archivo recien escrito para leerlo como texto y evitar errores
      ifs.open("cescritura.json");
      if(ifs)
      {
          string data;
          while(ifs >> data)
            dataJSON += data;

          ifs.close();
      }else{
          cout<<"syncronice: Error al leer el archivo (JSON)\n";
      }

      if(dataJSON != "")
          EnviarA_nodejs(dataJSON);
      else
          cout<<"EnviarA_nodejs() data no tiene contenido a enviar";

    }else{
        cout<<"syncronice: Error al abrir el disco virtual.\n";
    }

}



void EnviarA_nodejs(string data)
{

    CURL *curl;
    CURLcode res;
    //string prueba = "texto de prueba";
    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5000/syncronice");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        // Enviar request al servidor, res obtendra el codigo de retorno
            res = curl_easy_perform(curl);
            /* Check for errors */
            if(res != CURLE_OK)
              fprintf(stderr, "curl_easy_perform() fallo respuesta del servidor: %s\n",
                      curl_easy_strerror(res));

            /* always cleanup */
            curl_easy_cleanup(curl);
    }

}
//exec -path="/home/carlos/Escritorio/prueb/p2.sh"
#endif // SYNCRONICE_H
