#ifndef INTERPRETE_H
#define INTERPRETE_H


#include <iostream>
#include "analizador_lexico.h"
#include "mkdisk.h"
#include "rmdisk.h"
#include "fdisk.h"
#include "mount.h"
#include "rep.h"
#include "exec.h"
#include "unmount.h"
#include "manejo_particiones/mkfs.h" // internamente compila, 'Getterysetter_globales.h', 'journaling.h'
#include "manejo_particiones/admin_usuario.h"
#include "manejo_particiones/mkdir.h"
#include "manejo_particiones/mkfile.h"
#include "manejo_particiones/cat.h"
#include "manejo_particiones/rep_.h" // reportes de 2da fase
#include "manejo_particiones/loss.h"
#include "manejo_particiones/recovery.h"
#include "syncronice.h" // parte del proyecto 2.(base de datos)
using namespace std;



string StoMinusculas(string texto)
{
    string nuevo="";
    int tamano = texto.length();
    for(int i  = 0;  i < tamano; i++) // para recorrer todo el texto entrante.
    {
        nuevo += (char) tolower(texto.at(i));
    }

 return nuevo;
}


void Interprete(string texto)
{
 // cout<<"Recibido en el interprete:"<< texto <<endl;
  int estado = 0;
  bool error = false;
  char parametro = '\0'; // para ver que parametro esta activo en ese momento
  //============= Variables globales utilizadas en parametros de una instruccion =============================
  int tamano = 0;
  char unidad = '\0';
  char tipo_ajuste = '\0'; // utilizado por fit
  char tipo_particion = '\0'; // utilizado por type
  string nombre_particion = "";
  char tipo_borrado = '\0';
  int espacio = 0;
  char operacion = '+';  /*para utilizarlo en reduccion o agregacion de espacio de una particion*/
  string ruta = "";
  string id_disco = "";
  char instruccion = '\0';

  /*===UTILIZADO PARA MKFS=====*/
  char fs = '2'; // para saber que sistema de archivos utilizar 2=ext2 (por defecto) y 3 =ext3
  char type_formateo = 'c'; // por defecto tiene el formateo FULL = c (completo)
  /*========================================================*/
  /*=======utilizado para login y mkusr=============================*/
  string usr = "";
  string pwd = "";
  string grp = "";
  /*=======================================================*/
  /*=======utilizado para mkfile=============================*/
    string cont = "";
    char p = '0'; // usado por mkfile y mkdir
  /*=======================================================*/
    string Ruta = ""; // para rep(reportes)
    string file = ""; // para cat
    string jerarquia = ""; // utilizado para fdisk, para saber que comandos ignorar(esta creo que es obsoleta)
    int entradas_a_interprete = 0;

    while( (texto.length() >0) && (!error) )
    {
        Token t =  analizador_lexico(&texto); //Enviar la cadena multiples veces para que reconozca un token
        entradas_a_interprete++; // determina cuantas veces se ejecuto el interprete por comando

        switch(estado) // El switch trabajara como un analizador sintactico
        {
            case 0:{ // Se esperaran comandos: mkdisk, fdisk, rmdisk, mount, unmound, exec, rep,mkfs
                string lex_m = StoMinusculas( t.lexema );

                if(lex_m.compare("mkdisk") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'm';
                    tipo_ajuste = 'f'; /* por defecto tiene primer ajsute(FF)*/
                    unidad = 'm'; /*Por defecto tendra Megabytes (m) */

                }else if(lex_m.compare("rmdisk") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'r';

                }else if(lex_m.compare("fdisk") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'f';
                    tipo_ajuste = 'w'; // tiene peor ajsute(WF) por defecto
                    tipo_particion = 'p'; // tiene primaria por defecto
                    unidad = 'k';   /*Por defecto sera kilobytes(k) */

                }else if(lex_m.compare("mount") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'M';

                }else if(lex_m.compare("unmount") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'u';

                }else if(lex_m.compare("rep") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'R';

                }else if(lex_m.compare("exec") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'e';

                }else if(lex_m.compare("mkfs") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'F';

                }else if(lex_m.compare("login") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'l';

                }else if(lex_m.compare("logout") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'L';

                }else if(lex_m.compare("mkgrp") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'g';

                }else if(lex_m.compare("mkusr") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'U';

                }else if(lex_m.compare("mkfile") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'a';

                }else if(lex_m.compare("mkdir") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'd';

                }else if(lex_m.compare("pause") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'p';

                }else if(lex_m.compare("loss") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'P';

                }else if(lex_m.compare("recovery") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'A';

                }else if(lex_m.compare("cat") == 0){ // instruccion agregada en 2da fase
                    estado = 1;
                    instruccion = 'c';

                }else if(lex_m.compare("syncronice") == 0){ // instruccion agregada para el proyecto 2(base de datos)
                    estado = 1;
                    instruccion = 's';

                }else{
                    cout<<"Error, se esperaba(mkdisk|rmdisk|fdisk|mount|unmount|rep|exec) y se obtuvo:"<<t.lexema<<"!"<<endl;
                    error = true;
                }
            }break;

            case 1:{ // se espera un (espacio) o multiples

                if(t.num_token==2) /*Es un espacio o multiples espacios*/
                   estado=2; // ir a case 2
                else{
                    cout<<"ERROR!! Se esperaba espacio en blanco y se obtuvo:"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 2:{ // se espera un guion -

                if(t.num_token==3) // espera el guion -
                    estado=3; // ir a case 3
                else
                {
                    cout<<"Error se esperaba '-' y se obtuvo:"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 3: // Aqui se espera, size, fit, unit, path, delete,name, etc......
            {
                 string lex_m = StoMinusculas( t.lexema );
                 estado = 4; // ir a case 4

                 if(lex_m.compare("size") == 0){
                     parametro = 's';
                     if(jerarquia == "") // usado en fdisk
                        jerarquia = "size";

                 }else if(lex_m.compare("unit") == 0){
                     parametro = 'u';

                 }else if(lex_m.compare("fit") == 0){
                     parametro = 'f';

                 }else if(lex_m.compare("type") == 0){
                     parametro = 't';

                 }else if(lex_m.compare("delete") == 0){
                     parametro = 'd';
                     if(jerarquia == "")// usado en fdisk
                        jerarquia = "delete";

                 }else if(lex_m.compare("add") == 0){
                     parametro = 'a';
                     if(jerarquia == "")// usado en fdisk
                        jerarquia = "add";

                 }else if(lex_m.compare("name") == 0){
                     parametro = 'n';

                 }else if(lex_m.compare("id") == 0){
                     parametro = 'i';

                 }else if(lex_m.compare("path") == 0){
                     parametro = 'p';

                 }else if(lex_m.compare("fs") == 0){ // parametro agregado en 2da fase
                     parametro = 'F';

                 }else if(lex_m.compare("usr") == 0){ // parametro agregado en 2da fase
                     parametro = 'U';

                 }else if(lex_m.compare("pwd") == 0){ // parametro agregado en 2da fase
                     parametro = 'P';

                 }else if(lex_m.compare("grp") == 0){ // parametro agregado en 2da fase
                     parametro = 'g';

                 }else if(lex_m.compare("p") == 0){ // parametro agregado en 2da fase
                     estado = 1; // para no buscar valores
                     p = '1';

                 }else if(lex_m.compare("cont") == 0){ // parametro agregado en 2da fase
                     parametro = 'c';

                 }else if(lex_m.compare("ruta") == 0){ // parametro agregado en 2da fase
                     parametro = 'r';

                 }else if(lex_m.compare("file") == 0){ // parametro agregado en 2da fase
                     parametro = 'e';

                 }else{
                     cout<<"Error se esperaba (size|unit|path|fit|type|delete|add|name|id|fs|usr|pwd|grp) y se obtuvo"<<t.lexema<<endl;
                     error = true;
                 }


            }break;

            case 4: // Se espera que venga ' = '
            {
                if(t.num_token == 4) // ' = '
                {
                    if(parametro == 's')
                       estado=5;
                    else if(parametro == 'u')
                       estado=6;
                    else if(parametro == 'f')
                       estado=7;
                    else if(parametro == 't')
                       estado=8;
                    else if(parametro == 'n')
                       estado=9;
                    else if(parametro == 'd')
                       estado=10;
                    else if(parametro == 'a')
                       estado=11;
                    else if(parametro == 'p')
                       estado=13;
                   else if(parametro == 'i')
                       estado=14;
                   else if(parametro == 'F')
                       estado=15;
                   else if(parametro == 'U')
                       estado=16;
                   else if(parametro == 'P')
                       estado=17;
                   else if(parametro == 'g')
                       estado=18;
                   else if(parametro == 'c')
                       estado=19;
                   else if(parametro == 'r')
                       estado=20;
                    else if(parametro == 'e')
                        estado=21;
                }
                else
                {
                    cout<<"Error se esperaba ' = ' y se obtuvo"<< t.lexema<< endl;
                    error = true;
                }
            }break;

            case 5:
            {
                estado = 1; // Regresamos para poder leer mas posibles parametros
                    if(t.num_token==5)
                    {
                      tamano = stoi(t.lexema);
                     // cout<<"El tamaño que se almaceno en size es"<< tamano<< endl;
                    }
                    else
                    {
                      cout<<"Error se esperaba '(numero)' y se obtuvo "<< t.lexema<< endl;
                      error = true;
                    }
            }break;

            case 6: // parametro de unit
            {
                estado=1; /*volvemos para recibir mas parametros*/
                    string lex_m = StoMinusculas( t.lexema );

                    if(lex_m.compare("k") == 0)
                        unidad='k';
                    else if(lex_m.compare("m") == 0)
                        unidad='m';
                    else if(lex_m.compare("b") == 0)
                        unidad='b';
                    else
                    {
                        cout<<"Error en los parametros de unit se esperaba 'b'o'k'o'm' y se obtuvo:"<<t.lexema<<endl;
                        error= true;
                    }
            }break;

            case 7: // parametro de fit
            {
                estado=1; /*regresamos por si hay que leer mas parametros*/
                    string lex_m = StoMinusculas(t.lexema);

                      if(lex_m.compare("bf") == 0)
                          tipo_ajuste = 'b';
                      else if(lex_m.compare("ff") == 0)
                          tipo_ajuste = 'f';
                      else if(lex_m.compare("wf") == 0)
                          tipo_ajuste = 'w';
                      else{
                          cout<<"Error se esperaba 'bf,ff,wf' y se obtuvo:"<<t.lexema<<endl;
                          error= true;
                      }
            }break;

            case 8: // parametro de type
            {
                estado=1;
                string lex_m = StoMinusculas(t.lexema);

                     if(lex_m.compare("p") == 0)
                         tipo_particion = 'p';
                     else if(lex_m.compare("e") == 0)
                         tipo_particion = 'e';
                     else if(lex_m.compare("l") == 0)
                         tipo_particion = 'l';
                     else if(lex_m.compare("fast") == 0) // Agregado en 2da fase (mkfs)
                         type_formateo = 'r';
                     else if(lex_m.compare("full") == 0) // Agregado en 2da fase (mkfs)
                         type_formateo = 'c';
                     else{
                         cout<<"Error se esperaba 'p,l,e' y se obtuvo:"<<t.lexema<<endl;
                         error = true;
                     }
            }break;

            case 9: // valores de name
            {
                estado=1;
                if(t.num_token ==1 || t.num_token ==5 || t.num_token ==7 || t.num_token ==20)
                {
                    nombre_particion = t.lexema;
               /* if(t.num_token == 1) // se reciben letras
                {
                        nombre_particion += t.lexema;
                }
                else if(t.num_token == 7) // se reciben letras con espacios intercalados
                {
                    nombre_particion = t.lexema;
                    string texto = "";
                    int longitud = t.lexema.length();
                        for(int i = 0; i < longitud; i++)
                        {
                            char c = t.lexema[i];
                                if(c != '\\') // para quitarle estos caracteres ya que viene asi la cadena usualmente
                                {
                                    texto += c;
                                }

                        }
                    nombre_particion = texto;*/

                }else{
                    cout<<"Error se esperaba 'caracteres' y se obtuvo"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 10:// parametro de delete
            {
                estado=1;
                string lex_m = StoMinusculas(t.lexema);

                if(lex_m.compare("fast") == 0)
                {
                    tipo_borrado='r'; // r -> rapido

                }else if(lex_m.compare("full") == 0)
                {
                    tipo_borrado='c'; // c-> completo
                }
                 else
                 {
                       cout<< "Error se esperaba 'fast|full' y se obtuvo:"<< t.lexema;
                       error = true;
                 }

            }break;

            case 11:// Parametro de add
            {
                estado=1; /*Regresamos para poder leer mas parametros*/

                 if(t.num_token==5) // numero positivo
                 {
                   espacio = stoi(t.lexema);
                   //printf("El tamaño que se almaceno en add es %d\n",espacio);

                 }
                 else if(t.num_token==3) // ' - ' -> para numero negativo
                 {
                     estado=12; // ir a case 12
                    operacion = '-'; /*nos indica que es un valor negativo*/
                 }
                  else
                  {
                    cout<<"Error se esperaba '(numero)' | '-' y se obtuvo"<<t.lexema<<endl;
                    error = true;
                  }
            }break;

            case 12:
            {
                estado = 1;

                if(t.num_token==5) // numero positivo
                {
                  espacio = stoi(t.lexema);
                  printf("El tamaño que se almaceno en add es %d\n",espacio);

                }else {
                    cout<<"Error se esperaba '(numero)' y se obtuvo"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 13: // valores de path
            {
                estado = 1;

                if((t.num_token == 6) || (t.num_token == 7))
                {
                    ruta = t.lexema;
                }else {
                    cout<<"Error: Se esperaba path y se obtuvo->"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 14:
            {
                estado=1;

                if(t.num_token == 1) // se reciben letras
                {

                        id_disco = t.lexema;
                        cout<<"el id de la particion es :"<<t.lexema<<endl;
                }
                else{
                    cout<<"Error en parametro(id), Se esperaba (letras) y se obtuvo:"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 15: // valor para fs
            {
                estado = 1;
                string lex_m = StoMinusculas(t.lexema);

                if(lex_m.compare("2fs") == 0)
                    fs = '2';
                else if(lex_m.compare("3fs") == 0)
                    fs = '3';
                else
                {
                      cout<< "Error se esperaba '2fs|3fs' y se obtuvo:"<< t.lexema<<endl;
                      error = true;
                }

            }break;

            case 16: // valor para usr
            {
                estado = 1;
                if(t.num_token ==1 || t.num_token ==5 || t.num_token ==7 || t.num_token ==20)
                {
                    usr = t.lexema;

                }else {
                   cout<<"Error: se detecto un valor erroneo en el parametro 'usr', el valor erroneo fue:"<<t.lexema<<endl;
                }
            }break;

            case 17: // valor para pwd
            {
                estado = 1;
                if(t.num_token ==1 || t.num_token ==5 || t.num_token ==7 || t.num_token ==20)
                {
                    pwd = t.lexema;

                }else {
                   cout<<"Error: se detecto un valor erroneo en el parametro 'pwd', el valor erroneo fue:"<<t.lexema<<endl;
                }
            }break;

            case 18: // valor para grp
            {
                estado = 1;
                if(t.num_token ==1 || t.num_token ==5 || t.num_token ==7 || t.num_token ==20)
                {
                    grp = t.lexema;

                }else {
                   cout<<"Error: se detecto un valor erroneo en el parametro 'grp', el valor erroneo fue:"<<t.lexema<<endl;
                }
            }break;

            case 19: // valor para grp
            {
                estado = 1;
                if((t.num_token == 6) || (t.num_token == 7))
                {
                    cont = t.lexema;
                }else {
                    cout<<"Error: Se esperaba 'cont' y se obtuvo->"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 20: // parametro de path
            {
                estado = 1;

                if((t.num_token == 6) || (t.num_token == 7))
                {
                    Ruta = t.lexema;
                }else {
                    cout<<"Error: Se esperaba 'ruta(letras)' y se obtuvo->"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 21: // parametro de file
            {
                estado = 1;

                if((t.num_token == 6) || (t.num_token == 7))
                {
                    file = t.lexema;
                }else {
                    cout<<"Error: Se esperaba 'file(letras)' y se obtuvo->"<<t.lexema<<endl;
                    error = true;
                }
            }break;

        }
    }


    if(!error) // Si no hay errores en el analisis se procede a ejecutar las instrucciones
    {

      if( (estado != 1) && (estado != 2)) // implica que un parametro quedo inconcluso (ej: -fit = )
      {
          puts("Interprete: Error-> Se detecto que un parametro no tiene asignado un valor.");
          return;
      }

      switch(instruccion) // Aqui ya sabemos que instruccion se quiere ejecutar
      {
          case 'm':// Quiere ejecutar 'MKDISK'
          {
            if((tipo_particion=='\0') && (tipo_borrado=='\0') && (espacio==0) && (nombre_particion=="") && (id_disco=="")) // Si alguno de estos valores por defecto cambia, significa que ingreso un parametro que no pertenece a mkdisk
            {
              puts("===========================Instruccion MKDISK====================================\n");
                Instruccion_MKDISK(tamano,tipo_ajuste,unidad,ruta);
              puts("=================================FIN MKDISK======================================\n\n\n");
              puts("\n\n\n\n\n\n\n\n");
            }else {
              cout<<"Error en la instruccion 'MKDISK': se ingreso un parametro erroneo"<<endl;
            }

          }break;

          case 'r': // Quiere ejecutar 'RMDISK'
          {
            if((tamano==0)&&(unidad=='\0')&&(tipo_ajuste=='\0')&&(tipo_particion=='\0')&&(tipo_borrado=='\0')&&(espacio==0)&&(nombre_particion=="")&&(id_disco=="")) // valores por defecto
            {
                puts("===========================Instruccion RMDISK===================================");
                    Instruccion_RMDISK(ruta);
                puts("==================================FIN RMDISK====================================\n\n\n\n");
                puts("\n\n\n\n\n\n\n\n");
            }else {
                cout<<"Error en la instruccion 'RMDISK': se ingreso un parametro erroneo"<<endl;
            }
          }break;

          case 'f': // Quiere ejecutar un 'FDISK'
          {
            if(id_disco == "") // este parametro es el unico que no viene, por eso debe ser vacio -> ""
            {
                puts("===========================Instruccion FDISK====================================\n");
                  Instruccion_FDISK(operacion, tamano, ruta, tipo_particion, tipo_ajuste,tipo_borrado,nombre_particion, espacio, unidad);
                puts("\n=================================FIN FDISK=====================================\n\n\n\n");
                puts("\n\n\n\n\n\n\n\n");
            }else {
                cout<<"Error en la instruccion 'FDISK': se ingreso un parametro erroneo"<<endl;
            }

          }break;

          case 'M': // Quiere ejecutar 'Mount'
          {
            puts("===========================Instruccion MOUNT===================================");

            if((ruta!="" && nombre_particion!=""))
                Instruccion_MOUNT(ruta, nombre_particion); /*metodo de comandos.h*/
            else
                printf("Los parametros de MOUNT son erroneos");
            puts("=================================FIN MOUNT=====================================\n\n\n\n");
            puts("\n\n\n\n\n\n\n\n");
          }break;

          case 'u': // Quiere ejecutar 'Unmount'
          {
            if((tamano==0)&&(unidad=='\0')&&(tipo_ajuste=='\0')&&(tipo_particion=='\0')&&(tipo_borrado=='\0')&&(espacio==0)&&(nombre_particion=="")&&(ruta=="")) // valores por defecto
            {
                puts("===========================Instruccion UNMOUNT===================================");
                    Instruccion_UNMOUNT(id_disco);
                puts("==================================FIN UNMOUNT====================================\n\n\n");
                puts("\n\n\n\n\n\n\n\n");
            }else {
                cout<<"Interprete: Error en la instruccion 'UNMOUNT', se ingreso un parametro erroneo\n";
            }

          }break;

          case 'R': // Quiere ejecutar 'REP'
          {
            puts("===========================Instruccion REP===================================");
            if( (nombre_particion!="") && (ruta!="") && (id_disco!="")  )
            {

                if(( nombre_particion.compare("mbr")==0)  || (nombre_particion.compare("disk")==0) )
                {
                     Instruccion_REP(ruta, nombre_particion, id_disco);
                }else{
                    REP(nombre_particion, ruta, id_disco, Ruta);
                }
            }
            else
                printf("Los parametros de el comando REP son erroneos \n");

            puts("=================================FIN REP====================================\n\n\n");
            puts("\n\n\n\n\n\n\n\n");
          }break;

          case 'e': // Quiere ejecutar 'Exec'
          {
            puts("===========================Instruccion EXEC===============================");
                if(ruta!="")
                   Instruccion_EXEC(ruta);
                else
                   printf("LOS PARAMETROS DEL COMANDO EXEC SON ERRONEOS por lo tanto no se puede ejecutar...\n");

            puts("=================================FIN EXEC=================================\n\n\n");
          }break;

          case 'F': // Quiere ejecutar 'mkfs'
          {
            puts("\n===========================Instruccion MKFS===============================\n\n");

            if(id_disco != "")
                MKFS(id_disco, type_formateo, fs);
            else
                cout<<"Interprete: Error, el comando mkfs no tiene sus parametros completos\n";

            puts("=================================FIN MKFS=================================\n\n\n");
          }break;

          case 'l': // Quiere ejecutar 'login'
          {
            puts("\n===========================Instruccion LOGIN===============================\n\n");
            if( (usr != "") && (pwd != "") && (id_disco != ""))
            {
                LOGIN(usr, pwd, id_disco);
            }else{
                cout<<"Interprete: Error, No se pudo ejecutar el comando LOGIN ya que hacen falta parametros o estan erroneos.\n";
            }

            puts("=================================FIN LOGIN=================================\n\n\n");
          }break;

          case 'L': // Quiere ejecutar 'logout'
          {
              puts("\n===========================Instruccion LOGOUT===============================\n\n");

                  LOGOUT();
              puts("=================================FIN LOGOUT=================================\n\n\n");
          }break;

          case 'g': // Quiere ejecutar 'mkgrp'
          {
              puts("\n===========================Instruccion MKGRP===============================\n\n");

                  if(nombre_particion != "")
                      MKGRP(nombre_particion); // enviamos name
                  else
                      cout<<"Interprete: Error, no se puede ejecutar el comando MKGRP porque faltan parametros\n";

              puts("=================================FIN MKGRP=================================\n\n\n");
          }break;

          case 'U': // Quiere ejecutar 'mkusr'
          {
              puts("\n===========================Instruccion MKUSR===============================\n\n");

                  if((usr != "") && (pwd != "") && (grp != ""))
                      MKUSR(usr, pwd, grp);
                  else
                      cout<<"Interprete: Error, no se puede ejecutar el comando MKUSR porque faltan parametros\n";

              puts("=================================FIN MKUSR=================================\n\n\n");
          }break;

          case 'a': // Quiere ejecutar 'mkfile'
          {
              puts("\n===========================Instruccion MKFILE===============================\n\n");

                  if( ruta != "")
                      MKFILE(ruta, p, tamano, cont,false, id_propietario_g);
                  else
                      cout<<"Interprete: Error, no se puede ejecutar el comando MKFILE porque faltan parametros\n";

              puts("=================================FIN MKFILE=================================\n\n\n");
          }break;

          case 'd': // Quiere ejecutar 'mkdir'
          {
              puts("\n===========================Instruccion MKDIR===============================\n\n");

                  if( ruta != "")
                      MKDIR(ruta, p);
                  else
                      cout<<"Interprete: Error, no se puede ejecutar el comando MKDIR porque faltan parametros\n";

              puts("=================================FIN MKDIR=================================\n\n\n");
          }break;

          case 'p': // Quiere ejecutar 'pause'
          {
              puts("\n===========================Instruccion PAUSE===============================\n\n");

                cout<<"Ingrese un caracter:";
                if(entradas_a_interprete == 1)
                {
                    while(getchar() != '\n') // para recibir los caracteres entrantes
                    {}
                }else{
                    cout<<"Interprete: Error el comando pause no se puede ejecutar porque tiene parametros que no pertenecen a el\n";
                }

              puts("=================================FIN PAUSE=================================\n\n\n");
          }break;

          case 'P': // Quiere ejecutar 'LOSS'
          {
              puts("\n===========================Instruccion LOSS===============================\n\n");
              if(id_disco != ""){

                LOSS(id_disco);
              }else{
                cout<<"Interprete: Error, el comando 'Loss' no se puede ejecutar porque le faltan parametros\n";
              }

              puts("=================================FIN LOSS=================================\n\n\n");
          }break;

          case 'A': // Quiere ejecutar 'recovery'
          {
              puts("\n===========================Instruccion RECOVERY===============================\n\n");
              if(id_disco != ""){
                RECOVERY(id_disco);
              }else{
                cout<<"Interprete: Error, el comando 'Recovery' no se puede ejecutar porque le faltan parametros\n";
              }

              puts("=================================FIN RECOVERY=================================\n\n\n");
          }break;

          case 'c': // Quiere ejecutar 'CAT'
          {
              puts("\n===========================Instruccion CAT===============================\n\n");
              if(file != "")
              {
                  CAT(file);
              }else{
                  puts("Interprete: Error, el comando 'CAT' no puede ejecutarse, porque le faltan parametros");
              }

              puts("=================================FIN CAT=================================\n\n\n");
          }break;

          case 's': // Quiere ejecutar 'SYNCRONICE'
          {
              puts("\n===========================Instruccion SYNCRONICE===============================\n\n");
              if(id_disco != "")
              {
                  SYNCRONICE(id_disco);
              }else{
                  puts("Interprete: Error, el comando 'SYNCRONICE' no puede ejecutarse, porque le falta el parametro 'id' ");
              }

              puts("=================================FIN SYNCRONICE=================================\n\n\n");
          }break;
      }

    }
    else {
        cout<<"ERROR! No se pudo ejecutar la instruccion ya que tiene errores."<<endl;
    }
}


#endif // INTERPRETE_H
