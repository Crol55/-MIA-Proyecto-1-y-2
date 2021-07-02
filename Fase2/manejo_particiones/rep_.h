#ifndef REP__H
#define REP__H

#include <iostream>
#include "lista_enlazada.h"
#include <stdio.h> // para rewind , fgetc+..etc
#include "getterysetter_globales.h"
#include "op_inodos.h" // get_inodo(), contiene la cabecera 'op_bloques.h'
#include <fstream>

using namespace std;

/*======== Declaracion de prototipos===========*/
/*=== Externos ====*/
string separar_directorio(string path); // prototipo que ya fue compilado en otro .h (funcion de mkdisk.h)
string obtener_extension(string path_reporte); // prototipo que ya fue compilado en otro .h ( rep.h)

// prototipos de la clase rep_.h
void Graficar_sb(super_bloque sb, string path);
void Graficar_bm_inode(super_bloque sb, string path, FILE *disco);
void Graficar_bm_block(super_bloque sb, string path, FILE *disco);
void Graficar_inode(super_bloque sb, string path, FILE *disco, string ruta_disco);
void Graficar_block(super_bloque sb, string path, FILE *disco, string ruta_disco);
void Graficar_Tree(super_bloque sb, string path, FILE *disco, string ruta_disco);
void Graficar_file(string path, string ruta);
void Graficar_ls(string path, string ruta);
string convertir_permisos(int i_perm);
void Graficar_journaling(super_bloque sb, string path, FILE *disco);

/*======== Fin de prototipos===========*/


void REP(string name, string path, string id, string ruta)
{
    //1) Buscar la ruta en la lista de particiones montadas( buscar por id)
    string ruta_disco = get_ruta(id);
    if(ruta_disco != "")
    {
        FILE *disco = fopen(ruta_disco.c_str(), "r"); // abrir en modo lectura
        if(disco != NULL)
        {
            rewind(disco);
            mbr m;
                fread(&m, sizeof(m), 1, disco); // leemos el mbr y lo colocamos en la variable m

            // BUSCAR LA PARTICION QUE SE DESEA GRAFICAR (usando metodos de lista_enlazada.h y getterysetter_globales.h
            partition p = get_partition(get_nombre_particion(id), m);
            // EXTRAER EL SUPER BLOQUE
            super_bloque sb;
                fseek(disco, (p.part_start-1), SEEK_SET); // posicionamos el puntero interno del disco, donde esta el super_bloque
                fread(&sb, sizeof(sb), 1, disco);
            // VERIFICAR QUE LA PARTICION YA TENGA UN SISTEMA DE ARCHIVOS CREADO ( verificando el estado del super bloque)
            if(sb.s_filesystem_type != 0) // verificar que si tenga un super_bloque existente
            {
                if(name.compare("sb")==0)
                    Graficar_sb(sb, path);

                else if(name.compare("bm_inode")==0)
                    Graficar_bm_inode(sb, path, disco);

                else if(name.compare("bm_block")==0)
                    Graficar_bm_block(sb, path, disco);

                else if(name.compare("inode")==0)
                { /*El metodo Graficar_inode utiliza get_inode(internamente usa particion_global)
                    por lo tanto hay que guardar el posible valor que tenga para modificarlo y luego regersarle
                    su valor anterior*/
                    partition temporal = particion_global;
                    particion_global   = p;
                    Graficar_inode(sb, path, disco, ruta_disco);
                    particion_global = temporal; // le regresamos su valor
                }
                else if(name.compare("block")==0)
                { /*El metodo Graficar_block utiliza get_inodo(internamente usa particion_global)
                    por lo tanto hay que guardar el posible valor que tenga para modificarlo y luego regersarle
                    su valor anterior*/
                    partition temporal = particion_global;
                    particion_global   = p;
                    Graficar_block(sb, path, disco, ruta_disco);
                    particion_global = temporal; // le regresamos su valor
                }
                else if(name.compare("tree")==0)
                { /*El metodo Graficar_Tree utiliza get_inodo(internamente usa particion_global)
                    por lo tanto hay que guardar el posible valor que tenga para modificarlo y luego regersarle
                    su valor anterior*/
                    partition temporal = particion_global;
                    particion_global   = p;
                    Graficar_Tree(sb, path, disco, ruta_disco);
                    particion_global = temporal; // le regresamos su valor
                }
                else if(name.compare("file")==0)
                {
                    if(ruta != ""){
                        Graficar_file( path, ruta);
                    }else{
                        cout<<"Rep: Error, para realizar el reporte de 'file' el parametro 'ruta' debe existir\n";
                    }
                }
                else if(name.compare("ls") == 0)
                {
                    if(ruta != ""){
                        Graficar_ls( path, ruta);
                    }else{
                        cout<<"Rep: Error, para realizar el reporte de 'ls' el parametro 'ruta' debe existir\n";
                    }
                }else if(name.compare("journaling") == 0)
                {
                    Graficar_journaling(sb, path, disco);
                }

                else
                    cout<<"Rep: Error, el valor ingresado en 'name ="<<name<<"' es un valor de reporte incorrecto\n";

            }else{
                cout<<"Rep: Error, la particion con el 'id="<<id<<"', NO tiene un sistema de archivos creado (utilizar mkfs)\n";
            }
         fclose(disco);
        }else{
            cout<<"Rep: Error, no se puede abrir el disco\n";
        }
    }else{
        cout<<"Rep: Error, no existe particion montada con el id="<<id<<endl;
    }
}



void Graficar_journaling(super_bloque sb, string path, FILE *disco)
{
    string nuevo_directorio  = separar_directorio(path); // Separar el directorio
    string extension_reporte = obtener_extension(path); // puede ser .jpg o png

    ofstream fichero;
             fichero.open("journaling.dot");

    if(fichero)
    {
        fichero<<"digraph G{\nnode [shape=none]\na0[label=<<TABLE border=\"1\">\n";
        fichero<<"<TR><TD>Operacion</TD><TD>Nombre</TD><TD>Contenido</TD><TD>Propietario</TD></TR>";

        int inicio_journaling = particion_global.part_start + sizeof(super_bloque);
        int fin_journaling    = sb.s_bm_inode_start -1;

        journaling j_aux;// para ver que parte del journal esta vacio

        fseek(disco, inicio_journaling-1, SEEK_SET); //Mover el puntero del disco al inicio del journaling

        for (int i = 0; i < fin_journaling; i++)
        {
            fread(&j_aux, sizeof(journaling),1, disco); //la lectura se hara secuencial por medio del for
            //cout<<"Que tiene operacion adentro->"<<j_aux.operacion<<".\n";
            if(strcmp(j_aux.operacion, "")!=0) // Significa que ese espacio de journaling esta ocupado
            {
                fichero<<"<TR>\n";
                fichero<<"<TD>"<<j_aux.operacion<<"</TD>";
                fichero<<"<TD>"<<j_aux.nombre<<"</TD>";
                fichero<<"<TD>"<<j_aux.contenido<<"</TD>";
               // fichero<<"<TD>"<<j_aux.size<<"</TD>";
                fichero<<"<TD>"<<j_aux.propietario<<"</TD>";

                fichero<<"</TR>\n";
            }else{ // el journaling esta vacio en cierto punto

                break; // rompe el for y ya no graficar mas cosas
            }
        }
        fichero<<"</TABLE>>];\n}";
        fichero.close();

        // 2) Una vez creado el ls.dot, debemos decirle que tome ese ls.dot y genere una imagen
        string comando_dir = "mkdir -p " + nuevo_directorio;
        system( comando_dir.c_str() ); // creamos los directorios recursivamente

        string comando = "dot -T" + extension_reporte + " journaling.dot -o " + path ;
        system( comando.c_str() ); // creamos la imagen con graphviz
        cout<<"Rep: Imagen del (journaling) Generada correctamente en la ruta="<<path<<"\n";


    }else{
        cout<<"Rep: Error no se pudo crear (journaling.dot) \n";
    }
}

void  Graficar_ls(string path, string ruta)
{
    string nuevo_directorio  = separar_directorio(path); // Separar el directorio
    string extension_reporte = obtener_extension(path); // puede ser .jpg o png

    vector <string> rutas = split(ruta, '/');
    int tamano_rutas = rutas.size();
    Inodo padre = get_inodo(0, ruta_disco_global); // INICIAMOS EN EL INODO RAIZ(/)
    bool error = false;

    for(int ruta=0; ruta < tamano_rutas; ruta++) // para ingresar a cada carpeta de 'ruta'
    {
        string nombre_carpeta = rutas[ruta]; // extraemos el nombre de la carpeta
        if(nombre_carpeta.compare("") == 0) // si esta vacio significa que esa es la raiz, entonces debe salirse ya que la raiz se obtuvo arriba
        {
            break;
        }
        //EL PADRE SIEMPRE DEBE SER UNA CARPETA sino debe dar error
        if(padre.i_type != '0') // si no es carpeta
        {
            cout<<"Rep: Error,No encontro la carpeta que desea graficar\n";
            error = true;
            break;
        }

        int pos = existe_Inodo(padre, nombre_carpeta); // BUSCAMOS SI EL INODO EXISTE ADENTRO DE LA CARPETA PADRE
        if(pos != -1) // si existe el inodo
        {
            padre = get_inodo(pos, ruta_disco_global); // si el inodo existe nos cambiamos hacia ese inodo

        }else{ // no existe el inodo
            cout<<"Rep: Error, La carpeta identificado por '"<<nombre_carpeta<<"' no existe en el disco\n";
            error =  true;
            break; // nos salimos del for
        }
    }

    ofstream fichero;
             fichero.open("ls.dot"); // creamos el ls.dot
    fichero<<"digraph G{\nnode [shape=none]\n";

    if(!error) // implica que encontro todas las carpetas, verificar que la ultima sea una carpeta
    {
        if(padre.i_type == '0') // verificar que el ultimo haya sido una carpeta
        {

            if(fichero)
            {
                fichero<<"a0[label=<<TABLE border=\"1\">\n";
                fichero<<"<TR><TD>Permisos</TD><TD>Owner</TD><TD>Grupo</TD><TD>Size(en Bytes)</TD><TD>Fecha</TD><TD>Hora</TD><TD>Tipo</TD><TD>Name</TD></TR>";
                // Recorrer todo lo que este interno del inodo que se desea hacer el reporte

                for (int i = 0; i < 15; i++)
                {
                    int pos_bloque_carpeta = padre.i_block[i]; // acceder a todos los posibles bloques de carpeta

                    if(pos_bloque_carpeta != -1) // solo bloques activos
                    {

                        bloque_carpetas bc_aux = get_bloque_carpeta(pos_bloque_carpeta, ruta_disco_global);
                        for(int bc = 0; bc < 4; bc++)
                        {
                            if(bc_aux.b_content[bc].b_inodo != -1) // solo graficar bloque usado
                            {
                                if((strcmp(bc_aux.b_content[bc].b_name, ".")!= 0) && (strcmp(bc_aux.b_content[bc].b_name,"..")!=0) ) // Para NO graficar (.) y (..)
                                {
                                    fichero<<"<TR>\n";
                                    // recuperar el inodo (archivo o carpeta)
                                    Inodo inodo = get_inodo(bc_aux.b_content[bc].b_inodo, ruta_disco_global);
                                    string c_permiso = convertir_permisos(inodo.i_perm);
                                    string i_tipo = "Archivo";

                                    if(inodo.i_type == '0')
                                        i_tipo = "Carpeta";

                                    fichero<<"<TD>"<<c_permiso<<"</TD>\n"; // permisos
                                    fichero<<"<TD>"<<inodo.i_uid<<"</TD>\n";  // owner
                                    fichero<<"<TD>"<<inodo.i_gid<<"</TD>\n";  // grupo
                                    fichero<<"<TD>"<<inodo.i_size<<"</TD>\n"; // size
                                    fichero<<"<TD>"<<get_fecha(inodo.i_ctime)<<"</TD>\n"; // fecha
                                    fichero<<"<TD>"<<get_hora(inodo.i_ctime) <<"</TD>\n"; // hora
                                    fichero<<"<TD>"<<i_tipo<<"</TD>\n"; // tipo
                                    fichero<<"<TD>"<<bc_aux.b_content[bc].b_name<<"</TD>\n"; // name
                                    fichero<<"</TR>\n";
                                }

                            }

                        }

                    }

                }
                fichero<<"</TABLE>>];\n";
            }
           cout<<"Rep: Imagen del (ls) Generada correctamente en la ruta="<<path<<"\n";
        }else{
            if(padre.i_type == '\0') // esta vacio, esto implica que se utilizo loss..
                cout<<"Rep: Error, no se puede generar el reporte (ls)\n";
            else
            cout<<"Rep: Error, no se puede realizar el reporte 'ls' sobre un archivo, solo a una carpeta\n";
        }
    }
    fichero<<"}\n";
    fichero.close();
    string comando_dir = "mkdir -p " + nuevo_directorio;
    system( comando_dir.c_str() ); // creamos los directorios recursivamente

    string comando = "dot -T" + extension_reporte + " ls.dot -o " + path ;
    system( comando.c_str() ); // creamos la imagen con graphviz

}


string convertir_permisos(int i_perm) // convierte 777 -> -rw-rw-rw-, (depende del valor ingresado)
{

    string permiso = to_string(i_perm); // siempre sera de 3 digitos
    string permiso_convertido = "-";

    for(int i=0; i< 3; i++)
    {
        char ch = permiso[i]; // sacamos los valores de permiso por ejemplo, [7][7][7]
        switch(ch)
        {
            case '0':   permiso_convertido+="---";  break;
            case '1':   permiso_convertido+="---";  break;
            case '2':   permiso_convertido+="-w-";  break;
            case '3':   permiso_convertido+="-w-";  break;
            case '4':   permiso_convertido+="r--";  break;
            case '5':   permiso_convertido+="r--";  break;
            case '6':   permiso_convertido+="rw-";  break;
            case '7':   permiso_convertido+="rw-";  break;
        }
    }
 return permiso_convertido;
}


void Graficar_file( string path, string ruta)
{

    string nuevo_directorio  = separar_directorio(path); // Separar el directorio
    //1) CREAR LOS DIRECTORIOS (si no existen ,pero esto lo hace el sistema operativo)
    string comando = "mkdir -p "+ nuevo_directorio;

    vector <string> rutas = split(ruta, '/');
    int tamano_rutas = rutas.size();
    Inodo padre = get_inodo(0, ruta_disco_global); // INICIAMOS EN EL INODO RAIZ(/)
    bool error = false;

    for(int ruta=0; ruta < tamano_rutas; ruta++) // para ingresar a cada carpeta o archivo de ruta
    {
        string nombre = rutas[ruta]; // extraemos el nombre de la carpeta o archivo a buscar

        //EL INODO PADRE AL MOMENTO DE ENTRAR AQUI DEBE SER SIEMPRE UNA 'CARPETA' NO DEBE SER ARCHIVO A MENOS QUE SE HAYA INGRESADO PARA TRONAR EL PROGRAMA
        if(padre.i_type != '0') // si no es carpeta (entonces es un inodo error o un inodo archivo)
        {
            if(padre.i_type == '\0')
                cout<<"Rep: Error, NO se puede graficar un INODO VACIO (El reporte NO se realizo)\n";
            else
                cout<<"Rep: Error, un archivo no puede contener carpetas adentro, sintaxis erronea\n";
            error = true;
            break;
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

    ofstream fichero;
             fichero.open(path); // creamos el archivo.txt
    if(!error) // implica que encontro todas las carpetas y por ultimo el archivo
    {
        if(padre.i_type == '1') // verificar que el ultimo haya sido un archivo
        {

            if(fichero)
            {
                string contenido = leer_contenido_ino_Tarchivo(padre); // leemos todo lo que tenga adenro el contenido
                fichero << contenido;
                cout<<"Rep: El comando file ha generado el reporte(.txt) correctamente en la ruta:"<<path<<endl;

            }
        }else{
            cout<<"Rep: Error, no se puede realizar el reporte 'file' a una carpeta, solo a un archivo\n";
        }

    }
    fichero.close();
}


void Graficar_sb(super_bloque sb, string path)
{
    string nuevo_directorio  = separar_directorio(path); // Separar el directorio
    string extension_reporte = obtener_extension(path); // puede ser .jpg o png

    //1) CREAR UN ARCHIVO(sb.dot)
    ofstream ofs;
             ofs.open("sb.dot");

    if(ofs) // si el archivo esta ABIERTO
    {
        ofs<<"digraph G{\nnode [shape=none]\na0[label=<<TABLE border=\"1\">\n";
        ofs<<"<TR>\n<TD bgcolor=\"Chocolate\">s_filesystem_type</TD>\n<TD>"<<sb.s_filesystem_type<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD bgcolor=\"ForestGreen\">s_inodes_count</TD>\n<TD>"<<sb.s_inodes_count<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD bgcolor=\"ForestGreen\">s_block_count</TD>\n<TD>"<<sb.s_block_count<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD bgcolor=\"Orange\">s_free_blocks_count</TD>\n<TD>"<<sb.s_free_blocks_count<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD bgcolor=\"Orange\">s_free_inodes_count</TD>\n<TD>"<<sb.s_free_inodes_count<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD>s_mtime</TD>\n<TD>"<<ctime(&sb.s_mtime)<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD>s_umtime</TD>\n<TD>"<<ctime(&sb.s_umtime)<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD>s_mnt_count</TD>\n<TD>"<<sb.s_mnt_count<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD>s_magic</TD>\n<TD>"<<sb.s_magic<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD>s_inode_size</TD>\n<TD>"<<sb.s_inode_size<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD>s_block_size</TD>\n<TD>"<<sb.s_block_size<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD>s_mtime</TD>\n<TD>"<<ctime(&sb.s_mtime)<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD bgcolor=\"GreenYellow\">s_first_ino</TD>\n<TD>"<<sb.s_first_ino<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD bgcolor=\"GreenYellow\">s_first_blo</TD>\n<TD>"<<sb.s_first_blo<<"</TD>\n</TR>";

        ofs<<"<TR>\n<TD>s_bm_inode_start</TD>\n<TD>"<<sb.s_bm_inode_start<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD>s_bm_block_start</TD>\n<TD>"<<sb.s_bm_block_start<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD>s_inode_start</TD>\n<TD>"<<sb.s_inode_start<<"</TD>\n</TR>";
        ofs<<"<TR>\n<TD>s_block_start</TD>\n<TD>"<<sb.s_block_start<<"</TD>\n</TR>";
        ofs<<"</TABLE>>];\n}";
        ofs.close();

        // 2) Una vez creado el sb.dot, debemos decirle que tome ese sb.dot y genere una imagen
        string comando_dir = "mkdir -p " + nuevo_directorio;
        system( comando_dir.c_str() ); // creamos los directorios recursivamente

        string comando = "dot -T" + extension_reporte + " sb.dot -o " + path ;
        system( comando.c_str() ); // creamos la imagen con graphviz
        cout<<"Rep: Imagen del (sb) Generada correctamente en la ruta="<<path<<"\n";

    }else{
        cout<<"Rep: Error al abrir el archivo 'sb.dot'\n";
    }

}



void Graficar_bm_inode(super_bloque sb, string path, FILE *disco)
{
    string nuevo_directorio  = separar_directorio(path); // Separar el directorio

    //1) CREAR LOS DIRECTORIOS (si no existen ,pero esto lo hace el sistema operativo)
    string comando = "mkdir -p "+ nuevo_directorio;
    system(comando.c_str());

    FILE *fichero = fopen(path.c_str(), "w"); // crea el bm_inode.TXT
    //2) MOVER EL PUNTERO HASTA DONDE INICIA EL BITMAP DE INODOS
    int inicio_bm_inodos = sb.s_bm_inode_start;
    fseek(disco, inicio_bm_inodos-1, SEEK_SET);

    if(fichero != NULL)
    {
        char c;
        int conta=0;
        // 3)  RECORRER DESDE 0 HASTA EL TAMAÑO DE bm_inodos
        for(int i=0; i< sb.s_inodes_count; i++)
        {
            // 4) Leer el bitmap del disco y escribir el resultado en el TXT
            conta++;
            c = fgetc(disco); // obtenemos cada caracter y el puntero se va moviendo solito
            if(c == '\0'){
                fputc('0' ,fichero); // Sacamos los bytes (1 o 0) del disco y los graficamos en fichero
            }else{

                fputc(c ,fichero); // Sacamos los bytes (1 o 0) del disco y los graficamos en fichero
            }

            fputc(' ',fichero); fputc(' ',fichero); fputc(' ',fichero); fputc(' ',fichero);
            if(conta==20)
            {
               fputc('\n',fichero);
               conta=0;
            }
        }
        cout<<"Rep: Imagen del (bm_inode) Generada correctamente en la ruta="<<path<<"\n";
        fclose(fichero);
    }else{
        cout<<"rep: Error al abrir el archivo (bm_block)\n";
    }
}



void Graficar_bm_block(super_bloque sb, string path, FILE *disco)
{
    string nuevo_directorio  = separar_directorio(path); // Separar el directorio

    //1) CREAR LOS DIRECTORIOS (si no existen ,pero esto lo hace el sistema operativo)
    string comando = "mkdir -p "+ nuevo_directorio;
    system(comando.c_str());

    FILE *fichero = fopen(path.c_str(), "w"); // crea el bm_inode.TXT
    //2) MOVER EL PUNTERO HASTA DONDE INICIA EL BITMAP DE INODOS
    int inicio_bm_inodos = sb.s_bm_block_start;
    fseek(disco, inicio_bm_inodos-1, SEEK_SET);

    if(fichero != NULL)
    {
        char c;
        int conta=0;
        // 3)  RECORRER DESDE 0 HASTA EL TAMAÑO DE bm_inodos
        for(int i=0; i< sb.s_block_count; i++)
        {
            // 4) Leer el bitmap del disco y escribir el resultado en el TXT
            conta++;
            c = fgetc(disco); // obtenemos cada caracter y el puntero se va moviendo solito
            if(c == '\0'){
                fputc('0' ,fichero); // Sacamos los bytes (1 o 0) del disco y los graficamos en fichero
            }else{

                fputc(c ,fichero); // Sacamos los bytes (1 o 0) del disco y los graficamos en fichero
            }

            fputc(' ',fichero); fputc(' ',fichero); fputc(' ',fichero); fputc(' ',fichero);
            if(conta==20)
            {
               fputc('\n',fichero);
               conta=0;
            }
        }
        cout<<"Rep: Imagen del (bm_block) Generada correctamente en la ruta="<<path<<"\n";
        fclose(fichero);
    }else{
        cout<<"rep: Error al abrir el archivo (bm_inode)\n";
    }
}



void Graficar_inode(super_bloque sb, string path, FILE *disco, string ruta_disco)
{
    //Graficar los inodos sin unirlos, despues se uniran
    string nuevo_directorio  = separar_directorio(path); // Separar el directorio
    string extension_reporte = obtener_extension(path); // puede ser .jpg o png

    ofstream fichero;
             fichero.open("inode.dot");
    if(fichero)
    {
        //1) colocarse al inicio del bitmap de inodos
        int inicio_bm_inodos = sb.s_bm_inode_start;
        fseek(disco, inicio_bm_inodos -1, SEEK_SET);
        //2) leer TODOS los caracteres 1's del bitmap y luego ir a buscar el inodo para graficarlo
        char c;
        int contador_id = 0; // se usara en el nombre que tendra el inodo en graphviz (ej:inodo_0)
        int contador = -1;

        fichero <<"digraph G{\nrankdir=LR;\nnode [shape=none]\n";
        for(int i=0; i<sb.s_inodes_count; i++) // para recorrer el bitmap
        {
            c = fgetc(disco); // obtenemos cada caracter y el puntero se va moviendo solito
            if(c == '1') // crea las tablas de los inodos usados
            {
                Inodo inodo =  get_inodo(i, ruta_disco); // recuperamos el inodo con la pos 'i'  aqui mueve el puntero por lo tanto hay que reposicionarlo

                fichero<<"Inodo_"<<contador_id; // -> muy importante ya que indica el numero de inodo
                fichero<<"[label=<<TABLE border=\"1\">\n";
                fichero<<"<TR>\n<TD colspan=\"2\" align=\"center\" bgcolor=\"Chocolate\">Inodo "<<i<<"</TD></TR>\n";
                fichero<<"<TR>\n<TD>i_uid</TD>\n<TD>"<<inodo.i_uid<<"</TD>\n</TR>\n";
                fichero<<"<TR>\n<TD>i_gid</TD>\n<TD>"<<inodo.i_gid<<"</TD>\n</TR>\n";
                fichero<<"<TR>\n<TD>i_size</TD>\n<TD>"<<inodo.i_size<<"</TD>\n</TR>\n";

                fichero<<"<TR>\n<TD>i_atime</TD>\n<TD>"<<ctime(&inodo.i_atime)<<"</TD>\n</TR>\n";
                fichero<<"<TR>\n<TD>i_ctime</TD>\n<TD>"<<ctime(&inodo.i_ctime)<<"</TD>\n</TR>\n";
                fichero<<"<TR>\n<TD>i_mtime</TD>\n<TD>"<<ctime(&inodo.i_mtime)<<"</TD>\n</TR>\n";

                for(int j = 0; j<15; j++)
                {
                    if(inodo.i_block[j] != -1) // solo los que esten activos
                        fichero<<"<TR>\n<TD>i_block</TD>\n<TD>"<<inodo.i_block[j]<<"</TD>\n</TR>\n";
                }

                fichero<<"<TR>\n<TD bgcolor=\"Orange\">i_type</TD>\n<TD>"<<inodo.i_type<<"</TD>\n</TR>\n";
                fichero<<"<TR>\n<TD>i_perm</TD>\n<TD>"<<inodo.i_perm<<"</TD>\n</TR>\n";
                fichero<<"<TR>\n<TD>i_mtime</TD>\n<TD>"<<ctime(&inodo.i_mtime)<<"</TD>\n</TR>\n";
                fichero<<"</TABLE>>];\n";
                contador++;
                contador_id++;
            }
        }

        for(int i = 0; i < contador; i++) // realizar las uniones en graphviz de los inodos
        {
            fichero<<"Inodo_"<<i<<"->"<<"Inodo_"<<i+1<<";\n";
        }

        fichero<<"\n}\n";
        fichero.close();

        // 2) Una vez creado el sb.dot, debemos decirle que tome ese sb.dot y genere una imagen
        string comando_dir = "mkdir -p " + nuevo_directorio;
        system( comando_dir.c_str() ); // creamos los directorios recursivamente

        string comando = "dot -T" + extension_reporte + " inode.dot -o " + path ;
        system( comando.c_str() ); // creamos la imagen con graphviz
        cout<<"Rep: Imagen del (inode) Generada correctamente en la ruta="<<path<<"\n";
    }else{
        cout<<"Rep: Error al abrir (inode.dot)\n";
    }



}



void Graficar_block(super_bloque sb, string path, FILE *disco, string ruta_disco)
{
    string nuevo_directorio  = separar_directorio(path); // Separar el directorio
    string extension_reporte = obtener_extension(path); // puede ser .jpg o png

    ofstream fichero;
             fichero.open("block.dot"); // Crea o reescribe
    if(fichero)
    {
        /*Para poder graficar los bloques debemos saber si es un bloque de CARPETA o de ARCHIVO y eso solo se puede determinar por medio del INODO*/

        //1)  Obtener el inicio del bm_inodos
        int inicio_bm_inodos = sb.s_bm_inode_start;

       // 2) Colocarse con fseek al inicio_bm_inodos
        fseek(disco, inicio_bm_inodos -1, SEEK_SET);
        char c;
        int contador_id = 0; // para colocarle un identificador al bloque para poder realizar las uniones en graphviz (ej: bloque_(contador_id)
        int contador_cantidad = -1; // para Cuando se quieran realizar las uniones se conozca la cantidad de bloques existentes (ej: bloque_1 -> bloque_2)

       // 3)  Recorrer desde 0 hasta el tamaño del bm_inodos
        fichero <<"digraph G{\nrankdir=LR;\nnode [shape=none]\n";
        for(int i=0; i< sb.s_inodes_count; i++)
        {
            c = fgetc(disco); // obtenemos cada caracter y el puntero se va moviendo solito
            if(c == '1') // indica que hay un inodo activo en el bitmap
            {
                Inodo inodo = get_inodo(i, ruta_disco);
                char tipo_inodo ;// 0 carpeta, 1 bloque
                     tipo_inodo = inodo.i_type ;

                switch(tipo_inodo)
                {
                    case '0': // El inodo es de carpeta
                    {
                        bloque_carpetas bc;
                        for(int j= 0; j<15; j++) // recorrer todos los bloques_carpeta
                        {
                            int pos_bloqueC = inodo.i_block[j];
                            if(pos_bloqueC != -1) // implica que esta activo
                            {

                                bc = get_bloque_carpeta(pos_bloqueC, ruta_disco); // metodo de 'getterYsetter.h'
                                fichero<<"bloque_"<<contador_id;
                                fichero<<"[label=<<TABLE border=\"1\">\n";
                                fichero<<"<TR>\n<TD colspan=\"2\" bgcolor=\"Chocolate\">Bloque Carpeta "<<pos_bloqueC <<"</TD>\n</TR>\n";
                                fichero<<"<TR>\n<TD>b_name</TD><TD>b_inodo</TD>\n</TR>\n";

                                for(int k = 0; k < 4; k++)
                                {
                                    if(bc.b_content[k].b_inodo != -1)
                                    {
                                        fichero<<"<TR>\n<TD>"<<bc.b_content[k].b_name<<"</TD>\n<TD>"<<bc.b_content[k].b_inodo<<"</TD>\n</TR>\n";
                                    }
                                }
                                fichero<<"</TABLE>>];\n";
                                contador_id++;
                                contador_cantidad++;
                            }
                        }

                    }break;

                    case '1': // El inodo es de archivo
                    {
                        bloque_archivos ba;
                        int size_aux    = inodo.i_size; // para saber la cantidad de caracteres que se deben leer de los bloques (DEPENDE DEL SIZE DEL INODO)

                        for(int j=0; j<15; j++)
                        {
                            int pos_bloqueA = inodo.i_block[j];

                            if(pos_bloqueA != -1) // solo ingresa si hay bloque activo
                            {

                                ba = get_bloque_archivo(pos_bloqueA, ruta_disco);
                                cout<<"QUE PUTAS PUTAS PUTAS LEYO DEL BLOQUE:"<<ba.b_content<<"!\n";
                                fichero<<"bloque_"<<contador_id;
                                fichero<<"[label=<<TABLE border=\"1\">\n";
                                fichero<<"<TR>\n<TD colspan=\"2\" bgcolor=\"Orange\">Bloque Archivo "<<pos_bloqueA<<"</TD>\n</TR>\n";
                                // determinar cuantos caracteres se van a leer de cada bloque que se accede
                                string concat = "";
                                int iterador = 64; // valor por defecto ya que los bloques tienen 64 bytes(maximo)

                                if(size_aux < 64)
                                    iterador = size_aux; // para que no lea 64 caracteres, ya que si lee de mas puede contener basura
                                else{
                                    size_aux = size_aux - 64;
                                }

                                char caracter;
                                cout<<"CUANTO VAS A LEER"<<iterador<<endl;
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

                                fichero<<"<TR>\n<TD>"<<concat<<"</TD>\n</TR>\n";
                                fichero<<"</TABLE>>];\n";
                                contador_id++;
                                contador_cantidad++;
                            }
                        }
                    }break;
                }
            }
        }

        //4) Realizar las uniones entre los bloques
        for (int i = 0; i < contador_cantidad; i++) {
            fichero<<"bloque_"<<i<<"->"<<"bloque_"<<i+1<<";\n";
        }
        fichero<<"\n}\n";
        fichero.close();

        string comando_dir = "mkdir -p " + nuevo_directorio;
        system( comando_dir.c_str() ); // creamos los directorios recursivamente (no los crea si ya existen)

        string comando = "dot -T" + extension_reporte + " block.dot -o " + path ;
        system( comando.c_str() ); // creamos la imagen con graphviz
        cout<<"Rep: Imagen del (block) Generada correctamente en la ruta="<<path<<"\n";

    }else{
        cout<<"Rep: Error al abrir o crear (block.dot)\n";
    }
}



void Graficar_Tree(super_bloque sb, string path, FILE *disco, string ruta_disco)
{
    string nuevo_directorio  = separar_directorio(path); // Separar el directorio
    string extension_reporte = obtener_extension(path); // puede ser .jpg o png

    ofstream fichero;
             fichero.open("tree.dot"); // Crea o reescribe
    if(fichero)
    {
        //1)  Obtener el inicio del bm_inodos
        int inicio_bm_inodos = sb.s_bm_inode_start;

        // 2) Colocarse con fseek al inicio_bm_inodos
        fseek(disco, inicio_bm_inodos -1, SEEK_SET);
        char c;
        // 3)  Recorrer desde 0 hasta el tamaño del bm_inodos
        fichero<<"digraph G{\nrankdir=LR;node [shape=none]\n";
        for(int i=0; i< sb.s_inodes_count; i++)
        {
            c = fgetc(disco); // obtenemos cada caracter y el puntero se va moviendo solito
            if(c == '1') // indica que hay un inodo activo en el bitmap
            {
                Inodo inodo = get_inodo(i, ruta_disco);

                fichero<<"Inodo_"<<i; // -> muy importante ya que indica el numero de inodo
                fichero<< "[label=<<TABLE border=\"1\">\n";
                fichero<<"<TR><TD colspan=\"2\" bgcolor=\"CadetBlue\">INODO "<<i<<"</TD></TR>\n";
                fichero<< "<TR>\n<TD>i_uid</TD>\n<TD>"<<inodo.i_uid<<"</TD>\n</TR>\n";
                fichero<< "<TR>\n<TD>i_gid</TD>\n<TD>"<<inodo.i_gid<<"</TD>\n</TR>\n";
                fichero<< "<TR>\n<TD>i_size</TD>\n<TD>"<<inodo.i_size<<"</TD>\n</TR>\n";
                fichero<< "<TR>\n<TD>i_atime</TD>\n<TD>"<<ctime(&inodo.i_atime)<<"</TD>\n</TR>\n";
                fichero<< "<TR>\n<TD>i_ctime</TD>\n<TD>"<<ctime(&inodo.i_ctime)<<"</TD>\n</TR>\n";
                fichero<< "<TR>\n<TD>i_mtime</TD>\n<TD>"<<ctime(&inodo.i_mtime)<<"</TD>\n</TR>\n";
                for(int j = 0; j<15; j++)
                {
                    if(inodo.i_block[j] != -1)
                        fichero<< "<TR>\n<TD>i_block</TD>\n<TD>"<<inodo.i_block[j]<<"</TD>\n</TR>\n";
                }
                fichero<< "<TR>\n<TD>i_type</TD>\n<TD>"<<inodo.i_type<<"</TD>\n</TR>\n";
                fichero<< "<TR>\n<TD>i_iperm</TD>\n<TD>"<<inodo.i_perm<<"</TD>\n</TR>\n";
                fichero<< "</TABLE>>];\n";


                char tipo_inodo ;// 0 carpeta, 1 bloque
                     tipo_inodo = inodo.i_type ;

                switch(tipo_inodo)
                {
                    case '0': // El inodo es de carpeta
                    {
                        bloque_carpetas bc;
                        for(int j= 0; j<15; j++) // recorrer todos los bloques_carpeta
                        {
                            int pos_bloqueC = inodo.i_block[j];
                            if(pos_bloqueC != -1) // implica que esta activo
                            {
                                bc = get_bloque_carpeta(pos_bloqueC, ruta_disco); // metodo de 'getterYsetter.h'
                                fichero<< "bloqueC_"<<pos_bloqueC;
                                fichero<< "[label=<<TABLE border=\"1\">\n";
                                fichero<< "<TR>\n<TD colspan=\"2\" bgcolor=\"Orange\">Bloque Carpeta "<< pos_bloqueC << "</TD>\n</TR>\n";
                                fichero<< "<TR>\n<TD>b_name</TD><TD>b_inodo</TD>\n</TR>\n";

                                for(int k = 0; k < 4; k++)
                                {
                                    if(bc.b_content[k].b_inodo != -1)
                                    {
                                      fichero<< "<TR>\n<TD>"<<bc.b_content[k].b_name<<"</TD>\n<TD>"<<bc.b_content[k].b_inodo<<"</TD>\n</TR>\n";

                                    }
                                }
                                fichero<< "</TABLE>>];\n";
                                /*Aprovechando las caracteristicas de graphviz, podemos realizar las conexiones a nodos que aun no
                                  existen y luego cuando existan se van a realizar las conexiones
                                */
                                for(int k = 0; k < 4; k++) // realiza las conexiones del bloque tipo carpeta con los inodos a los que apunta
                                {
                                    if(bc.b_content[k].b_inodo != -1)
                                    {
                                      fichero<< "bloqueC_"<<pos_bloqueC<<"->Inodo_"<<bc.b_content[k].b_inodo<<";\n";

                                    }
                                }


                                fichero<<"Inodo_"<<i<<"->bloqueC_"<<pos_bloqueC<<";\n";
                            }
                        }

                    }break;

                    case '1': // El inodo es de archivo
                    {
                        bloque_archivos ba;
                        int size_aux    = inodo.i_size; // para saber la cantidad de caracteres que se deben leer de los bloques (DEPENDE DEL SIZE DEL INODO)
                        for(int j=0; j<15; j++)
                        {
                            int pos_bloqueA = inodo.i_block[j];

                            if(pos_bloqueA != -1) // solo ingresa si hay bloque activo
                            {
                                ba = get_bloque_archivo(pos_bloqueA, ruta_disco);
                                fichero<<"bloqueA_"<<pos_bloqueA;
                                fichero<<"[label=<<TABLE border=\"1\">\n";
                                fichero<< "<TR>\n<TD colspan=\"2\" bgcolor=\"ForestGreen\">Bloque Archivo "<< pos_bloqueA<<"</TD>\n</TR>\n";

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

                                fichero<< "<TR>\n<TD>"<<concat<<"</TD>\n</TR>\n";
                                fichero<< "</TABLE>>];\n";
                                fichero<< "Inodo_"<<i<<"->bloqueA_"<<pos_bloqueA<<";\n" ;
                            }
                        }
                    }break;
                }// switch
            }// if
        } // for

        fichero<<"\n}\n";
     fichero.close();

     string comando_dir = "mkdir -p " + nuevo_directorio;
     system( comando_dir.c_str() ); // creamos los directorios recursivamente (no los crea si ya existen)

     string comando = "dot -T" + extension_reporte + " tree.dot -o " + path ;
     system( comando.c_str() ); // creamos la imagen con graphviz
     cout<<"Rep: Imagen del (Tree) Generada correctamente en la ruta="<<path<<"\n";
    }else{
        cout<<"Rep: Error al abrir (tree.dot) \n";
    }

}
#endif // REP__H
