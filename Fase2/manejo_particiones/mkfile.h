#ifndef MKFILE_H
#define MKFILE_H

#include <iostream>
#include "getterysetter_globales.h"
#include "op_inodos.h"

using namespace std;

/*================= Declaracion de Prototipos =============*/
/*===== Externos (que ya fueron compiladas antes que mkfile.h) ====*/
int Crear_Carpeta(Inodo padre, int pos_padre, string nombre_carpeta); // compilada en 'mkdir.h'
void Journaling(string tipo_operacion, string ruta, int propietario, string contenido); // compilada en 'mkfs.h'
/*===== Internos====*/
bool Crear_Archivo(Inodo padre, int pos_padre, string nombre_archivo_nuevo, string contenido, int propietario);
void Enviar_aJournaling(string ruta, int propietario, string contenido);
/*================= FIN =============*/



void MKFILE(string path, char p, int size, string cont, bool recovery_mode, int propietario)
{
// [el dueño del archivo sera el usuario logeado][debe tener permiso de escritura en la carpeta padre][sobrescribir el archivo]

    if(usuario_global != "")
    {
        string contenido = "";
        //CREAR O RECUPERAR EL CONTENIDO QUE SE QUIERE ESCRIBIR EN EL ARCHIVO (CONT TIENE PRIORIDAD SOBRE SIZE) el contenido es OPCIONAL
        if(recovery_mode) // Si estamos en modo recovery, el contenido de recuperacion se encuentra en 'cont'
        {
            contenido = cont;
        }
        else if(cont != "") // recuperar contenido del disco
        {
            //Lee el contenido de un archivo en el disco duro( NO de la particion virtual)
            FILE *fichero = fopen(cont.c_str(), "r"); // abrir el archivo en modo lectura
            if(fichero != NULL)
            {
                char caracter ;

                while( (caracter = getc(fichero)) != EOF) // leer hasta el fin del fichero
                    contenido = contenido + caracter;

            }else{
                cout<<"mkfile: Error, cont="<<cont<<" NO existe en el disco duro\n";
                return; // salimos de la funcion ya que hubo un error
            }

        }else if(size>0){ // llenar el contenido de 0-9 las veces que indique size

            int sequencia = 0; // para ir de (0-9)+
            for (int var = 0; var < size; var++)
            {
                contenido += std::to_string(sequencia);
                sequencia++;

                if(sequencia >9)
                    sequencia = 0;
            }
        }

        if(p == '1') // Crear directorios recursivamente
        {
            // Crear dir recursivamente y luego crear el archivo
            vector <string> carpetas = split(path, '/');
            int tamano = carpetas.size();
            // 1) Realizar el proceso de busqueda y creacion(si no existe) , comenzando por la raiz
            int pos_inodo_padre = 0;
            Inodo padre = get_inodo(pos_inodo_padre, ruta_disco_global); // obtenemos el inodo raiz (/)

            for(int i = 0; i < tamano-1; i++) // Creamos TODAS las carpetas que NO existan y de ultimo creamos el archivo
            {
                //2) Buscar adentro de la carpeta padre que exista la carpeta que se desea crear
                string nombre_carpeta = carpetas[i]; // Obtenemos cada carpeta que deseamos crear
                int pos_carpeta = buscar_pos_Carpeta(padre, nombre_carpeta); // buscamos la carpeta por nombre, adentro de la carpeta padre

                if(pos_carpeta == -1) // si no existe, debemos crearla, si ya existe (debemos decir que ese sera ahora el Inodo 'padre')
                {
                    int pos_nueva_carpeta = Crear_Carpeta(padre, pos_inodo_padre, nombre_carpeta);
                    pos_inodo_padre = pos_nueva_carpeta;
                    padre = get_inodo(pos_inodo_padre, ruta_disco_global); // ahora la carpeta padre es la que se acaba de crear
                    if(!recovery_mode) // no esta en recovery mode
                        Enviar_aJournaling(path, propietario, contenido);

                }else{ // si existe la carpeta
                    pos_inodo_padre = pos_carpeta; // Ahora la carpeta padre, sera otra
                    padre = get_inodo(pos_inodo_padre, ruta_disco_global); // ahora la carpeta padre es otra
                }
            }

            //3) luego de crear todas las carpetas, creamos el archivo en la carpeta padre
            string nombre_archivo = carpetas[tamano -1];
            bool creado = Crear_Archivo(padre, pos_inodo_padre, nombre_archivo, contenido, propietario);

            if(!creado)
                cout<<"mkfile:Error, No se pudo crear el archivo puede ser que ya no tenga espacio libre\n";


        }else{ // Si alguna carpeta de 'path' no existe, debe dar error

            objeto_existencia ultima_carpeta = Existen_Carpetas(path); // determina si todas las carpetas adentro de path existen retorna y retorna la ultima carpeta

            if(ultima_carpeta.esValido) // implica que todas las carpetas si existen, entonces si se puede crear el archivo en la ultima carpeta
            {
                if(ultima_carpeta.nombre_aCrear != "")
                {
                  bool creado =  Crear_Archivo(ultima_carpeta.padre, ultima_carpeta.pos_inodo_padre, ultima_carpeta.nombre_aCrear, contenido, propietario);
                  if(!recovery_mode) // NO esta en recovery mode
                    Enviar_aJournaling(path, propietario, contenido);
                  if(!creado)
                      cout<<"mkfile:Error, No se pudo crear el archivo puede ser que ya no tenga espacio libre\n";

                }else{
                    cout<<"mkfile: Error, El archivo que desea crear no tiene nombre\n";
                }
            }
        }

    }else{
        cout<<"mkfile: Error, Debe iniciar Sesion para ejecutar este comando\n";
    }
}


bool Crear_Archivo(Inodo padre, int pos_padre, string nombre_archivo_nuevo, string contenido, int propietario) // funcion que crear un archivo en disco, y returna true si lo creo, y false si no lo creo
{
    //1) Verificar si se debe sobrescribir el archivo
    int pos_inodo_archivo = existe_Inodo(padre, nombre_archivo_nuevo); // esta funcion se encarga de ver si ya existe o no el inodo archivo
    cout<<"importante: El contenido es:"<<contenido<<endl;

    if(pos_inodo_archivo == -1) // El Inodo archivo no existe en el disco
    {
        //2) Buscarle un espacio vacio en los bloques internos del inodo padre
        for(int i = 0; i<15; i++)
        {
            int pos = padre.i_block[i];

            if(pos != -1) // Usar bloque de carpeta activo
            {
                bloque_carpetas bc = get_bloque_carpeta(pos, ruta_disco_global); // metodo de 'getterYsetter_globales.h'

                for(int x = 0; x<4; x++)
                {
                    content bloque_cont = bc.b_content[x];

                    if(bloque_cont.b_inodo == -1)
                    {
                        printf("En que i la encontre:%i, y en x:%i\n",i,x);
                    //3) insertar aqui la nueva creacion del inodo y el contenido de este inodo debe ser modificado y reescrito en el disco
                        FILE *disco = fopen(ruta_disco_global.c_str(), "r+b");
                        if(disco != NULL)
                        {
                        //4)   actualizamos el bloque de carpetas del con el nuevo nombre y a que inodo archivo apuntara
                            super_bloque sb = get_super_bloque();
                            strcpy(bloque_cont.b_name, nombre_archivo_nuevo.c_str());
                            bloque_cont.b_inodo = sb.s_first_ino; // el bloque apuntara al inodo libre que se creara abajo en 'Crear_INODO'
                            bc.b_content[x] = bloque_cont;

                         //5) Actualizamos el bloque de carpetas al disco fisico
                            Actualizar_bloque_carpeta(bc, pos);
                            Crear_INODO(disco, '1', propietario, 0, 664, pos_padre); // Creamos el inodo ARCHIVO y automaticamente crea su bloque de archivo(vacio)
                            fclose(disco);
                          //6 ) llenar el Archivo con contenido
                            Inodo archivo = get_inodo(sb.s_first_ino, ruta_disco_global); // obtenemos el Archivo(INODO) recien creado
                                  archivo = Escribir_contenido_ino_Tarchivo(archivo, contenido); // escribe el contenido del archivo
                            Sobreescribir_inodo(ruta_disco_global, archivo, sb.s_first_ino); // actualizamos el inodo en el disco


                        }else{
                            puts("Error en 'Crear_Archivo': No se pudo Abrir el disco\n");
                        }
                      return true;//para salir de la funcion
                    }
                }
            }
            else // Encontro un bloque vacio adentro del inodo de carpeta
            {

                FILE *disco = fopen(ruta_disco_global.c_str(), "r+b");
                if(disco != NULL)
                {
                    super_bloque sb = get_super_bloque(); // obtenemos el super_bloque antes de que cree la carpeta para no tener la actualizada
              //1)  Asignar un nuevo bloque libre al apuntador del inodo padre
                    padre.i_block[i] = sb.s_first_blo;
                    Sobreescribir_inodo(ruta_disco_global, padre, pos_padre); // Actualizamos el inodo padre en el disco

              //2) Creamos el NUEVO bloque_carpeta VACIO para el inodo padre
                    Crear_bloque(disco, '0',-1, '0'); //pos padre se envian como -1 para que ya no cree el (. y ..) ya que solo el primer bloqe tiene . y..

               //3) Debemos sacar el bloque_carpeta recien creado para colocarle info
                    int pos_b = sb.s_first_blo; // este sera el bloque nuevo
                    bloque_carpetas bc;
                                    bc = get_bloque_carpeta(pos_b, ruta_disco_global); // metodo de 'getterYsetter_globales.h'
                                    bc.b_content[0].b_inodo = sb.s_first_ino; // este sera el inodo nuevo .. creado lineas abajo
                                    strcpy(bc.b_content[0].b_name, nombre_archivo_nuevo.c_str());
                    Sobrescribir_bloque_carpeta(bc, pos_b); // volvemos a escribir el bloque_carpetas al disco con la info actualizada

                //3) Escribimos el nuevo inodo de tipo archivo
                    Crear_INODO(disco, '1', 1, 0, 664, pos_padre);
                    fclose(disco);
                    //4 ) llenar el Archivo con contenido
                      Inodo archivo = get_inodo(sb.s_first_ino, ruta_disco_global); // obtenemos el Archivo(INODO) recien creado
                            archivo = Escribir_contenido_ino_Tarchivo(archivo, contenido); // escribe el contenido del archivo y actualiza el inodo archivo
                      Sobreescribir_inodo(ruta_disco_global, archivo, sb.s_first_ino);

                }else{
                    puts("Error en 'Crear_Archivo: No se pudo Abrir el disco");
                }

              return true;
            }
        } // cierre for


    }else{ // El Inodo archivo, si existe en el disco (debe ser sobrescrito)
        cout<<"El archivo ya existia, asi que sera sobrescrito\n";
    }
  return false;
}


void Enviar_aJournaling(string ruta, int propietario, string contenido) // metodo que prepara la informacion que se enviara al journaling
{
    /*Si el contenido es mayor a 64 bytes se debe de separar
     el journaling en mkfile y contenido para poder recuperar despues
    */
    int tam = contenido.size();
    if(tam <= 64)
        Journaling("mkfile", ruta, propietario, contenido);

    else{ // el contenido es mayor a 64 bytes
        string nuevo;
        for (int i = 0; i < 64; i++)
            nuevo = nuevo + contenido[i];

        Journaling("mkfile", ruta, propietario, nuevo);

        nuevo.clear(); // limpiamos nuevo
        int t_total = contenido.size();
        for (int i = 64; i < t_total; i++) // desde 64 hasta el fin de contenido
        {
            nuevo = nuevo + contenido[i];
            // se pueden dar 2 casos, el primero que nuevo se llene (64) o que llegue al fin de contenido sin llenarse
            if(nuevo.size()==64)
            {
                Journaling("contenido", ruta, propietario, nuevo);
                nuevo.clear();

            }else if( (i == t_total-1) && (nuevo != "")){

                Journaling("contenido", ruta, propietario, nuevo);
                // no es necesario limpiar nuevo, ya que esta es la ultima iteracion del ciclo for
            }
        }
    }
}

#endif // MKFILE_H
