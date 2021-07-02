#ifndef MKDIR_H
#define MKDIR_H


#include <iostream>
#include "op_inodos.h"
using namespace std;


/*================== Declaracion de prototipos==================*/
// ========= INTERNOS=============
int Crear_Carpeta(Inodo padre, int pos_padre, string nombre_carpeta); // crea una carpeta nueva y devuelve la posicion donde fue creada
// ==========EXTERNOS=============
void Journaling(string tipo_operacion, string ruta, int propietario, string contenido); // ya fue compilado en 'mkfs.h'
/*=================== FIN =======================*/

void MKDIR(string path, char p)
{
    if(p == '1') // Crear carpetas recursivamente
    {
        vector <string> carpetas = split(path, '/');
        int tamano = carpetas.size();
        // 1) Realizar el proceso de busqueda y creacion(si no existe) , comenzando por la raiz
        int pos_inodo_padre = 0;
        Inodo padre = get_inodo(pos_inodo_padre, ruta_disco_global); // obtenemos el inodo raiz (/)

        for(int i = 0; i < tamano; i++)
        {
            //2) Buscar adentro de la carpeta padre que exista la carpeta que se desea crear
            string nombre_carpeta = carpetas[i]; // Obtenemos cada carpeta que deseamos crear
            int pos_carpeta = buscar_pos_Carpeta(padre, nombre_carpeta); // buscamos la carpeta por nombre, adentro de la carpeta padre

            if(pos_carpeta == -1) // si no existe, debemos crearla
            {
              int pos_nueva_carpeta = Crear_Carpeta(padre, pos_inodo_padre, nombre_carpeta);
              pos_inodo_padre = pos_nueva_carpeta;
              padre = get_inodo(pos_inodo_padre, ruta_disco_global); // ahora la carpeta padre es la que se acaba de crear

            }else{ // si ya existe (debemos decir que ese sera ahora el Inodo 'padre')
                pos_inodo_padre = pos_carpeta; // Ahora la carpeta padre, sera otra
                padre = get_inodo(pos_inodo_padre, ruta_disco_global); // ahora la carpeta padre es otra
            }
        }
        Journaling("mkdir", path, 1,"");

    }else{ // Solo crear la ultima carpeta

        objeto_existencia ultima_carpeta = Existen_Carpetas(path); // determina si existen todas las carpetas excepto la ultima

        if(ultima_carpeta.esValido) // implica que todas las carpetas si existen, entonces si se puede crear el archivo en la ultima carpeta
        {
            if(ultima_carpeta.nombre_aCrear != "")
            {
              int pos_nueva_carpeta =  Crear_Carpeta(ultima_carpeta.padre, ultima_carpeta.pos_inodo_padre, ultima_carpeta.nombre_aCrear);

              if(pos_nueva_carpeta == -1)
                  cout<<"mkdir:Error, La carpeta que desea Crear ya existe, Eliminela o cambie nombre\n";
              else
                  Journaling("mkdir", path, 1,""); // si crea exitosamente la carpeta, entonces almacenamos en el journaling

            }else{
                cout<<"mkdir: Error, la carpeta que desea crear no tiene nombre\n";
            }
        }
    }
}



int Crear_Carpeta(Inodo padre, int pos_padre, string nombre_carpeta) // devuelve en que posicion creo la carpeta
{
    //1) Buscar la carpeta que se desea crear
    int pos_inodo_carpeta = existe_Inodo(padre, nombre_carpeta); // Verificar si ya existe o no la carpeta a crear

    if(pos_inodo_carpeta == -1) // El Inodo carpeta no existe en el disco
    {
        //2) Buscarle un espacio vacio en los bloques internos del inodo padre
        int pos_nueva_carpeta = -1; // para saber que posicion de inodo utilizo del disco....
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
                        //4)   actualizamos el bloque de carpetas del con el nuevo nombre y a que inodo carpeta apuntara
                            super_bloque sb = get_super_bloque();
                            strcpy(bloque_cont.b_name, nombre_carpeta.c_str());
                            bloque_cont.b_inodo = sb.s_first_ino; // el bloque apuntara al inodo libre que se creara abajo en 'Crear_INODO'
                            pos_nueva_carpeta   = sb.s_first_ino;
                            bc.b_content[x] = bloque_cont;

                         //5) Actualizamos el bloque de carpetas al disco fisico
                            Actualizar_bloque_carpeta(bc, pos);
                            Crear_INODO(disco, '0', 1, 0, 664, pos_padre); // Creamos el inodo carpeta

                         fclose(disco);
                        }else{

                            puts("Error en 'Crear_Carpeta': No se pudo Abrir el disco\n");

                        }
                      return pos_nueva_carpeta;//para salir de la funcion
                    }
                }
            }
            else // Encontro un bloque vacio adentro del inodo de carpeta
            {
                FILE *disco = fopen(ruta_disco_global.c_str(), "r+b");
                if(disco != NULL)
                {
                    super_bloque sb = get_super_bloque(); // obtenemos el super_bloque antes de que cree la carpeta para no tenerla actualizada
                    pos_nueva_carpeta = sb.s_first_ino;
                 //1)  Asignar un nuevo bloque libre al apuntador del inodo padre
                    padre.i_block[i]  = sb.s_first_blo;
                    Sobreescribir_inodo(ruta_disco_global, padre, pos_padre); // Actualizamos el inodo padre en el disco

                 //2) Creamos el NUEVO bloque_carpeta VACIO para el inodo padre
                    Crear_bloque(disco, '0',-1, '0'); //pos_padre se envian como -1 para que ya no cree el (. y ..) ya que solo el primer bloqe tiene . y..

                 //3) Debemos sacar el bloque_carpeta recien creado para colocarle info
                    int pos_b = sb.s_first_blo; // este sera el bloque recien creado
                    bloque_carpetas bc;
                                    bc = get_bloque_carpeta(pos_b, ruta_disco_global); // metodo de 'getterYsetter_globales.h'
                                    bc.b_content[0].b_inodo = sb.s_first_ino; // este sera el inodo nuevo .. creado lineas abajo
                                    strcpy(bc.b_content[0].b_name, nombre_carpeta.c_str());
                    Sobrescribir_bloque_carpeta(bc, pos_b); // volvemos a escribir el bloque_carpetas al disco con la info actualizada

                  //3) Escribimos el nuevo inodo de tipo Carpeta
                    Crear_INODO(disco, '0', 1, 0, 664, pos_padre);

                 fclose(disco);
                }else{
                    puts("Error en 'Crear_Archivo: No se pudo Abrir el disco");
                }

              return pos_nueva_carpeta;
            }
        } // cierre for

    }
  return -1; // si retorna -1 significa que la carpeta que desea crear ya existe.
}


void MKDIR_RECOVERY(string path)// metodo utilizado especificamente en 'recovery.h'
{
    puts("ESTOY INGRESANDO EN ESTA MIERDA???????????????????????????????");
        vector <string> carpetas = split(path, '/');
        int tamano = carpetas.size();
        // 1) Realizar el proceso de busqueda y creacion(si no existe) , comenzando por la raiz
        int pos_inodo_padre = 0;
        Inodo padre = get_inodo(pos_inodo_padre, ruta_disco_global); // obtenemos el inodo raiz (/)

        for(int i = 0; i < tamano; i++)
        {
            //2) Buscar adentro de la carpeta padre que exista la carpeta que se desea crear
            string nombre_carpeta = carpetas[i]; // Obtenemos cada carpeta que deseamos crear
            int pos_carpeta = buscar_pos_Carpeta(padre, nombre_carpeta); // buscamos la carpeta por nombre, adentro de la carpeta padre

            if(pos_carpeta == -1) // si no existe, debemos crearla
            {
              int pos_nueva_carpeta = Crear_Carpeta(padre, pos_inodo_padre, nombre_carpeta);
              pos_inodo_padre = pos_nueva_carpeta;
              padre = get_inodo(pos_inodo_padre, ruta_disco_global); // ahora la carpeta padre es la que se acaba de crear

            }else{ // si ya existe (debemos decir que ese sera ahora el Inodo 'padre')
                pos_inodo_padre = pos_carpeta; // Ahora la carpeta padre, sera otra
                padre = get_inodo(pos_inodo_padre, ruta_disco_global); // ahora la carpeta padre es otra
            }
        }

}
#endif // MKDIR_H
