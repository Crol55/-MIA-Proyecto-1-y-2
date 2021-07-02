#ifndef ADMIN_USUARIO_H
#define ADMIN_USUARIO_H

#include <iostream>
#include "lista_enlazada.h" // -> esta cabecera contiene 'estructuras.h'
#include "getterysetter_globales.h"
#include "op_inodos.h" //-> incluye(op_bloques)
#include <string.h>

using namespace std;

bool Buscar_usuario(string registro, string usr, string pwd) // compara el registro con usr y pwd, y determina si el usuario existe
{
    bool existe = false;
    string separador = "";
    string arr[5];
    int contador =0;


    for(int i = 0; i <(int) registro.length(); i++)
    {
        char c = registro[i];
        if(c != ',')
        {
            separador += c;
        }else {
            arr[contador] = separador;
            separador = "";
            contador++;
        }

        if(i == (int) registro.length()-1) // si ya se llego al final del registro.
        {
            arr[contador] = separador;
            contador++;
        }

    }

    if(contador==5) // significa que el registro es de tipo usuario
    { // y solo nos interesan las posiciones 3 y 4 ahi es donde esta (usr, pwd)
     // antes de comparar hay que quitarle los espacios a ambas usr y pwd ya que pueden tener espacios internos

        string usuario = arr[3];
        string password = arr[4];

        if( (usuario.compare(usr) == 0) && (password.compare(pwd) == 0) )
        {
            existe = true;
            id_propietario_g = stoi(arr[0]);
            //cout<<"El propietario es:"<<id_propietario_g<<endl;
        }
    }

  return existe;
}



bool existe_usuario(string registro, string usr, int *autoincremento)
{
    bool existe = false;
    string separador = "";
    string arr[5];
    int contador =0;

    for(int i = 0; i <(int) registro.length(); i++)
    {
        char c = registro[i];
        if(c != ',')
        {
            separador += c;
        }else {
            arr[contador] = separador;
            separador = "";
            contador++;
        }

        if(i == (int) registro.length()-1) // si ya se llego al final del registro.
        {
            arr[contador] = separador;
            contador++;
        }
    }

    if(contador==5) // significa que el registro es un registro de tipo usuario
    { // y solo nos interesa la posicion 3 es ahi donde esta el nombre de usuario

        string u_aux = ""; // variable para concatenar sin espacios en blanco ' '
               u_aux = arr[3];

        // 2) Solo debe comparar con los usuarios que no esten eliminados -> estado != '0'

        if(arr[0] != "0" ) // si el estado es != 0 significa que ese usuario no esta eliminado
        {
            if( u_aux.compare(usr) == 0) // compara los registro de tipo grupo, con el nombre_grupo ingresado por el usuario
                existe = true;

            *autoincremento = *autoincremento + 1; // se incrementa con cada registro que haya recorrido
        }
    }

 return existe;
}



bool Buscar_grupo(string registro, string nombre_grupo, int *autoincremento)
{
    bool existe = false;
    string separador = "";
    string arr[5];
    int contador =0;

    for(int i = 0; i <(int) registro.length(); i++)
    {
        char c = registro[i];
        if(c != ',')
        {
            separador += c;
        }else {
            arr[contador] = separador;
            separador = "";
            contador++;
        }

        if(i == (int) registro.length()-1) // si ya se llego al final del registro.
        {
            arr[contador] = separador;
            contador++;
        }
    }


    if(contador==3) // Es un registro de tipo grupo
    { // y solo nos interesa la posicion 2 es ahi donde esta el nombre

        string gr_aux = ""; // variable para concatenar sin espacios en blanco ' '
               gr_aux = arr[2];

     // 1) Solo debe comparar con los grupos que no esten eliminados -> estado != '0'

        if(arr[0] != "0") // si el estado es != 0 significa que ese grupo no esta eliminado
        {

            cout<<"comparando-> registro interno:"<<gr_aux <<", ingresado por usuario:"<<nombre_grupo<<endl;
            if( gr_aux.compare(nombre_grupo) == 0) // compara el grupo con lo ingresado por el usuario
                existe = 1;

            *autoincremento = *autoincremento + 1; // se incrementa con cada registro que haya recorrido
        }
    }
 return existe;
}



void LOGIN(string usr, string pwd, string id_montaje)
{
    string ruta_disco = get_ruta(id_montaje); // funcion de "lista_enlazada.h"
    ruta_disco_global = ruta_disco;

    if(ruta_disco != "")
    {
        FILE *disco = fopen(ruta_disco.c_str(),"r+b");
        if(disco != NULL)
        {
            rewind(disco); // colocarse en el byte inicial
            mbr mbr_aux;
            fread(&mbr_aux, sizeof(mbr_aux), 1, disco); /*leemos los 136 bytes iniciales donde sabemos que se encuentra el mbr*/
            string nombre_particion = get_nombre_particion(id_montaje);
            partition p = get_partition(nombre_particion, mbr_aux);
            particion_global = p; // La particion actual sera la global, la que se usara en todos los comandos del sistema de archivos(2da fase)

            Inodo i = get_usersTXT(disco, particion_global); // Nos devuelve el inodo 'users.txt'

            if(i.i_type != 'E') // significa que si encontro el inodo
            {
                string contenido_archivo = leer_contenido_ino_Tarchivo(i); // recuperamos TODO el contenido adentro de los bloques del inodo tipo archivo

             // 2) buscar en el contenido del archivo el usuario con el que se quieren logear
                string registro = ""; /*almacena los registros separados por \n y analiza si coincide con el usuario que se quiere hacer login */
                bool existe = false;

                for(int i = 0; i <(int) contenido_archivo.length(); i++)
                {
                    if(contenido_archivo[i] != '\n')
                    {
                        registro += contenido_archivo[i];

                    }else{ // analizamos el registro

                      cout<<"registro a analizar:"<<registro<<endl;

                      existe = Buscar_usuario(registro, usr, pwd); //Funcion encargada de ver si el registro enviado coincide con el usuario que quiere hacer login

                      if(existe) // si existe el usuario entonces ya no debe seguir buscando en los registros
                      {
                          printf("Bienvenido usuario correcto\n");
                          usuario_global = usr; //
                          grupo_global   = get_grupo(registro);
                          break; // termina la ejecucion del for
                      }
                      registro = "";
                    }
                }

                if(!existe)
                    cout<<"login: Error el usr:"<<usr<<", y el pwd:"<<pwd<<". NO estan registrdos en los usuarios del sistema\n";


            }else{
              printf("login: Error, No se pudo recuperar el inodo 'users.txt' \n");
            }

          fclose(disco);
        }else{
            cout<<"login: Error al abrir el disco\n";
        }

    }else {
        cout<<"login: Error el 'id' ingresado no existe en las particiones montadas\n";
    }
}



void LOGOUT()
{

    if(usuario_global != "")
    {
        usuario_global = ""; // borramos el usuario
        grupo_global   = ""; // borramos el grupo
        //part_global = 0; // borramos la particion que se esta usando en ese momento
        ruta_disco_global = "";
        id_propietario_g = -1;
        cout<<"logout: Sesion de usuario terminada\n";
    }else {
        cout<<"logout:Error no se puede utilizar el comando logout porque no existe sesion activa\n";
    }

}



void MKGRP(string name)
{
    if(usuario_global != "")
    {
        if(usuario_global.compare("root") == 0)
        {
            if(name.length()>10)
            {
                cout<<"mkgrp: Error, el 'name' solo puede tener un maximo de 10 caracteres\n";
                return; // salir de la funcion
            }
            FILE *disco = fopen(ruta_disco_global.c_str(), "r+b");
            if(disco != NULL)
            {
                Inodo userstxt = get_usersTXT(disco, particion_global);
                if(userstxt.i_type != 'E')
                {
                 // 1) recuperar todo el contenido que este en el inodo 'users.txt'
                    string contenido_bloques = "";
                           contenido_bloques = leer_contenido_ino_Tarchivo(userstxt); // obtiene todo el contenido de los bloques de un inodo tipo archivo

                 // 2) Ahora hay que verificar si el grupo ya existe
                    string registro = ""; /*almacena los registros separados por \n*/
                    bool existe = false; // 1=grupo repetido, 0=grupo no repretido
                    int id_grupo = 1; // se incrementa al encontrar un grupo

                    for(int i =0; i<(int) contenido_bloques.length(); i++) // para recorrer TODO el contenido
                    {
                        if(contenido_bloques[i] == '\n') // analizamos el registro
                        {
                            existe = Buscar_grupo(registro, name, &id_grupo); // este metodo es el encargado de ver si el registro enviado coincide con el grupo a crear
                            registro = "";

                            if(existe) // si existe el usuario entonces ya no debe seguir buscando en los registros
                               break; // termina la ejecucion del for

                        }else{
                            registro += contenido_bloques[i];
                        }
                    }

                    if(!existe)
                    { // Aqui es donde debemos agregar el nuevo grupo
                        string nuevo_grupo = "";
                        nuevo_grupo = nuevo_grupo + std::to_string(id_grupo) + ",G," + name + "\n";
                        cout<<"El grupo que se creara es:"<<nuevo_grupo<<" y su longitud es:"<<nuevo_grupo.length()<<endl;


                        contenido_bloques += nuevo_grupo; // concatenamos el nuevo usuario los usuarios existentes...
                        // 5) // Escribir y actualizar los bloques en el disco
                        Inodo nuevo_i = Escribir_contenido_ino_Tarchivo(userstxt, contenido_bloques);
                        // 6) ya que escribio los bloques y actualizo los bloques, solo falta actualizar el inodo_Tarchivo
                        Sobreescribir_inodo_Tarchivo(ruta_disco_global, nuevo_i, 1);

                        puts("Grupo Creado con exito\n");

                    }else {
                        cout<<"mkgrp:Error, NO se puede crear el grupo con el 'name ="<<name<<"' porque ya existe \n";
                    }

                }else{
                    cout<<"mkgrp:Error NO pudo recuperar el inodo 'users.txt'\n";
                }
            }else{
                cout<<"mkgrp: Error al abrir el Disco\n";
            }
        }else{
            cout<<"mkgrp: Error-> El unico que puede utilizar el comando mkgrp es el usuario root\n";
        }

    }else{
        cout<<"mkgrp: Error -> NO hay sesion iniciada en el sistema, utilice 'LOGIN' para iniciar sesion\n";
    }
}



void MKUSR(string usr, string pwd, string grp)
{

   int t_usr,t_pwd,t_grp;//El tamaño de estos comandos solo puede tener un maximo de 10 caracteres
        t_usr = usr.length();
        t_pwd = pwd.length();
        t_grp = grp.length();

    if((t_usr <= 10) && (t_pwd <= 10) && (t_grp <= 10))
    {
        if(usuario_global != "")
        {//2) Ver que el usuario root sea el que quiere realizar la operacion
            if(usuario_global.compare("root") == 0)
            {
             //3) Recuperar la tabla de inodos 'users.txt'
                FILE *disco = fopen(ruta_disco_global.c_str(), "r+b");

                if(disco != NULL)
                {
                    Inodo userstxt = get_usersTXT(disco, particion_global);

                    if(userstxt.i_type != 'E')
                    {
                     //4) Extraer todo el contenido que este en el inodo 'users.txt'
                        string contenido_bloques = "";
                               contenido_bloques = leer_contenido_ino_Tarchivo(userstxt); // obtiene todo el contenido de los bloques de un inodo tipo archivo


                     //5) Separar contenido_bloques por '\n' (salto de linea) e insertarlo en registro
                        string registro = ""; //almacena los registros separados por \n
                        bool existe_usr = false;
                        bool existe_grp = false;
                        int id_usuario = 1; // se debe incrementar  cada vez que busca un usuario y se incrementara en comparar usuario

                            for(int i =0; i<(int)contenido_bloques.length(); i++)
                            {
                                //1) Buscar que el usuario NO exista y Buscar que el grupo SI exista
                                if(contenido_bloques[i] == '\n') // analizamos el registro
                                {
                                    existe_usr = existe_usuario(registro, usr, &id_usuario); // verifica usaurio

                                    if(!existe_grp){ // solo ingresa si NO ha encontrado aun el grupo en los registros
                                        int val_inutil = 0;// solo es para que no truene la funcion 'Buscar_grupo'
                                        existe_grp = Buscar_grupo(registro, grp, &val_inutil);
                                    }

                                    registro = ""; // limpiamos

                                    if(existe_usr) // si existe el usuario entonces ya no debe seguir buscando en los registros
                                        break; // termina la ejecucion del for

                                }else{
                                    registro += contenido_bloques[i]; // Aqui se forma el registro hasta que encuentre el \n
                                }
                            }

                        if( (!existe_usr) && (existe_grp) )   // si se puede agregar un nuevo usuario
                        {
                            string nuevo_usuario = "";
                                   nuevo_usuario = nuevo_usuario + std::to_string(id_usuario)+",U," + grp+","+usr+","+pwd+"\n";
                                   cout<<"el tamaño del nuevo registro usuario es(40):"<<nuevo_usuario.length()<<endl;
                            contenido_bloques += nuevo_usuario;

                            Inodo nuevo_i = Escribir_contenido_ino_Tarchivo(userstxt, contenido_bloques);
                          // 6) ya que escribio los bloques y actualizo los bloques, solo falta actualizar el inodo_Tarchivo sobreescribiendo la informacion
                            Sobreescribir_inodo_Tarchivo(ruta_disco_global, nuevo_i, 1);

                            printf("usuario Creado con exito\n");

                        }else {
                                if(existe_usr)
                                    puts("mkusr:Error, El usuario ingresado ya existe!");
                                else
                                    puts("mkusr:Error, El grupo ingresado NO existe!");
                        }

                    }else{
                        cout<<"mkusr: No se pudo recuperar el inodo 'users.txt'\n";
                    }

                  fclose(disco);
                }else{

                    cout<<"mkusr: Error, no se pudo abrir el disco\n";
                }

            }else{
                printf("mkusr: Error, El unico que puede utilizar mkgrp es el usuario root\n");
            }

        }else{
            puts("mkusr: Error, No hay sesion iniciada en el sistema, utilize 'LOGIN' para iniciar sesion\n");
        }

    }else {
        cout<<"mkusr: Error, El tamaño maximo de el valor de un parametro debe ser de 10 caracteres\n";
    }



}

#endif // ADMIN_USUARIO_H
