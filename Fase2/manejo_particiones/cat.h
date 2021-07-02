#ifndef CAT_H
#define CAT_H


#include <iostream>
#include "getterysetter_globales.h"
#include "op_inodos.h"


void CAT(string file) // recibe la ruta de un archivo(de donde se sacara la info)
{

    if(ruta_disco_global == "")
    {
        puts("CAT: Error, no ha iniciado sesion\n");
        return;
    }

    vector <string> rutas = split(file, '/');
    int tamano_rutas = rutas.size();
    Inodo padre = get_inodo(0, ruta_disco_global); // INICIAMOS EN EL INODO RAIZ(/)

    bool error = false;

    for(int i=0; i < tamano_rutas; i++) // para ingresar a cada carpeta o archivo de ruta
    {
        string nombre = rutas[i]; // extraemos el nombre de la carpeta o archivo a buscar

        //EL INODO PADRE AL MOMENTO DE ENTRAR AQUI DEBE SER SIEMPRE UNA 'CARPETA' NO DEBE SER ARCHIVO A MENOS QUE SE HAYA INGRESADO PARA TRONAR EL PROGRAMA
        if(padre.i_type != '0') // si no es carpeta (entonces es un inodo error o un inodo archivo)
        {
            cout<<"Rep: Error, un archivo no puede contener carpetas adentro, sintaxis erronea\n";
            error = true;
            break; // termina el for
        }

        int pos = existe_Inodo(padre, nombre); // BUSCAMOS SI EL INODO EXISTE ADENTRO DE LA CARPETA PADRE
        if(pos != -1) // si existe el inodo
        {
            padre = get_inodo(pos, ruta_disco_global); // si el inodo existe nos cambiamos hacia ese inodo

        }else{ // no existe el inodo
            cout<<"Rep: Error, El (Archivo o carpeta) identificado por '"<<nombre<<"' no existe en el disco\n";
            error =  true;
            break; // nos salimos del for
        }
    }
    // AL SALIR DEL FOR DEBEMOS TENER EN 'padre' EL INODO AL CUAL SE LE QUIERE APLICAR CAT
    if(!error)
    {
        string contenido = leer_contenido_ino_Tarchivo(padre);
        cout<<"El contenido leeido fue:"<<contenido<<endl;
    }

}
#endif // CAT_H
