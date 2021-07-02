#ifndef EXEC_H
#define EXEC_H

#include <iostream>
#include <stdio.h>
//#include "interprete.h"

using namespace std;

/*(exec.h) fue ejecutado en el (interprete.h) por lo tanto interprete ya fue definido
 * y debemos crear un prototipo sin necesidad de incluir (interprete.h)*/


void Interprete(string texto); // prototipo de la cabecera (interprete.h)



void Instruccion_EXEC(string path)
{

FILE *archivo = fopen(path.c_str(), "r"); // abrir el archivo en modo lectura

     if(archivo != NULL)
     {
        bool descartar         = false; // para descartar los comentarios
        string texto           = "";
        string texto_comentado = "";
        char c;

          while( (c=fgetc(archivo)) != EOF)
          {

             if(c != 10) // Lee todos los caracteres entrantes hasta que encuentre el salto de linea
             {
                 if(c == 35) /* El inicio de un comentario c = # */
                   descartar = true;

                  if(!descartar)
                  {
                     texto += c;

                  } else{ /*concatenar lo comentado para luego solo mostrarlo en la consola*/

                      texto_comentado += c;
                  }

             }
             else // ingresa cuando lee el LF (\n) exec -path=/home/carlos/Escritorio/mia_pruebas/instrucciones.sh
             {
                 if(texto != "") // leyo palabras reconocidas por el lenguaje
                 {
                     cout<<texto;
                     if(texto_comentado != "")
                     {
                         cout<<texto_comentado;
                         texto_comentado = ""; // borramos los comentarios para que no los vuelva a colocar
                     }
                     cout<<"\n";

                     Interprete(texto);// llamamos una vez mas al interprete
                 }

                 if(texto_comentado != "") // imprimimos los comentarios
                     cout<<texto_comentado<<endl;

                 /*inicializar denuevo las variables */
                 descartar = false; // para descartar los comentarios
                 texto="";
                 texto_comentado="";
             }

          } /*while que termina hasta leer todo el archivo*/

     }
     else{
        cout<<"Exec: Error al abrir el archivo:"<< path <<endl;
     }



}




#endif // EXEC_H
